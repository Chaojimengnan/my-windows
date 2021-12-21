#pragma once
#include <sddl.h>

namespace mw {

	class private_namespace;

	/// <summary>
	/// 创建并返回一个安全属性
	/// </summary>
	/// <param name="inherit_handle">继承开关</param>
	/// <param name="security_descriptor">安全描述符</param>
	/// <returns>返回一个安全属性</returns>
	inline std::shared_ptr<SECURITY_ATTRIBUTES> make_security_attribute(bool inherit_handle = false,
		void* security_descriptor = nullptr)
	{
		std::shared_ptr<SECURITY_ATTRIBUTES> security(new SECURITY_ATTRIBUTES());
		security->nLength = sizeof(SECURITY_ATTRIBUTES);
		security->bInheritHandle = inherit_handle;
		security->lpSecurityDescriptor = security_descriptor;
		return security;
	}

	/// <summary>
	/// 创建并返回一个管理员sid
	/// </summary>
	/// <returns>返回一个管理员sid</returns>
	inline std::shared_ptr<SID> create_admin_sid()
	{
		std::shared_ptr<SID> admin_sid = std::make_shared<SID>();
		ZeroMemory(admin_sid.get(), sizeof(admin_sid));
		DWORD sid_size = sizeof(admin_sid);
		CreateWellKnownSid(WinBuiltinAdministratorsSid, nullptr, admin_sid.get(), &sid_size);
		GET_ERROR_MSG_OUTPUT();
		return admin_sid;
	}

	/// <summary>
	/// 创建并返回一个专有空间
	/// </summary>
	/// <param name="...args">构造函数参数</param>
	/// <returns>返回一个专有空间</returns>
	template <typename... Args>
	inline std::shared_ptr<private_namespace> make_private_namespace(Args&&... args)
	{
		return std::shared_ptr<private_namespace>(new private_namespace(std::forward<Args>(args)...));
	}

	/// <summary>
	/// 专有空间类，用于创建专有空间，需要管理员权限，不过创建的句柄不会再命名冲突
	/// </summary>
	class private_namespace {
	public:
		/// <summary>
		/// 专有空间构造函数
		/// </summary>
		/// <param name="boundary_name">边界名字</param>
		/// <param name="namespace_name">专有空间名字</param>
		private_namespace(const std::tstring& boundary_name, const std::tstring& namespace_name)
		{
			boundary_handle = CreateBoundaryDescriptor(boundary_name.c_str(), 0);
			auto admin_sid = create_admin_sid();
			AddSIDToBoundaryDescriptor(&boundary_handle, admin_sid.get());
			GET_ERROR_MSG_OUTPUT();
			auto security = make_security_attribute();
			ConvertStringSecurityDescriptorToSecurityDescriptor(
				_T("D:(A;;GA;;;BA)"),
				SDDL_REVISION_1, &security.get()->lpSecurityDescriptor, nullptr);
			GET_ERROR_MSG_OUTPUT();

			namespace_handle = CreatePrivateNamespace(security.get(), boundary_handle, namespace_name.c_str());
			GET_ERROR_MSG_OUTPUT();
			LocalFree(security.get()->lpSecurityDescriptor);
		}
		~private_namespace()
		{
			ClosePrivateNamespace(namespace_handle, 0);
			DeleteBoundaryDescriptor(boundary_handle);
		}

		/// <summary>
		/// 获得边界的句柄
		/// </summary>
		/// <returns>返回边界的句柄</returns>
		HANDLE get_boundary_handle() { return boundary_handle; }
		/// <summary>
		/// 获得专有空间的句柄
		/// </summary>
		/// <returns>返回专有空间的句柄</returns>
		HANDLE get_namespace_handle() { return namespace_handle; }
	private:
		HANDLE boundary_handle = nullptr;
		HANDLE namespace_handle = nullptr;
	};

}//mw