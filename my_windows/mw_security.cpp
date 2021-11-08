#include "mw_security.h"
#include <sddl.h>

namespace mw {

	std::shared_ptr<SECURITY_ATTRIBUTES> make_security_attribute(bool inherit_handle, void* security_descriptor)
	{
		std::shared_ptr<SECURITY_ATTRIBUTES> security(new SECURITY_ATTRIBUTES());
		security->nLength = sizeof(SECURITY_ATTRIBUTES);
		security->bInheritHandle = inherit_handle;
		security->lpSecurityDescriptor = security_descriptor;
		return security;
	}

	std::shared_ptr<SID> create_admin_sid()
	{
		std::shared_ptr<SID> admin_sid = std::make_shared<SID>();
		ZeroMemory(admin_sid.get(), sizeof(admin_sid));
		DWORD sid_size = sizeof(admin_sid);
		CreateWellKnownSid( WinBuiltinAdministratorsSid, nullptr, admin_sid.get(), &sid_size);
		GET_ERROR_MSG_OUTPUT(std::cout);
		return admin_sid;
	}

	private_namespace::private_namespace(const std::string& boundary_name, const std::string& namespace_name)
	{
		boundary_handle = CreateBoundaryDescriptorA(boundary_name.c_str(), 0);
		auto admin_sid = create_admin_sid();
		AddSIDToBoundaryDescriptor(&boundary_handle, admin_sid.get());
		GET_ERROR_MSG_OUTPUT(std::cout);
		auto security = make_security_attribute();
		ConvertStringSecurityDescriptorToSecurityDescriptorA(
			"D:(A;;GA;;;BA)",
			SDDL_REVISION_1, &security.get()->lpSecurityDescriptor, nullptr);
		GET_ERROR_MSG_OUTPUT(std::cout);

		namespace_handle = CreatePrivateNamespaceA( security.get(), boundary_handle, namespace_name.c_str());
		GET_ERROR_MSG_OUTPUT(std::cout);
		LocalFree(security.get()->lpSecurityDescriptor);
	}
	private_namespace::~private_namespace()
	{
		ClosePrivateNamespace(namespace_handle, 0);
		DeleteBoundaryDescriptor(boundary_handle);
	}
};