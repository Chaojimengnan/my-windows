#pragma once

namespace mw {

	/// <summary>
	/// 将当前线程所在进程的一个句柄复制给目标进程(target_process)，并返回复制产生的句柄(注意该句柄应该是内核对象的句柄)
	/// </summary>
	/// <remarks>注意，返回的新句柄应该传给目标进程，源进程不要使用这个句柄(封装自DuplicateHandle)</remarks>
	/// <param name="target_process">目标进程</param>
	/// <param name="handle_to_give">要复制的句柄</param>
	/// <param name="inherit_handle">新句柄是否可以被目标进程新创建的子进程继承</param>
	/// <param name="desired_access">新句柄的访问设置</param>
	/// <param name="options">可选行为</param>
	/// <returns>复制产生的新句柄</returns>
	MW_API HANDLE give_handle_to_other_process(HANDLE target_process,
		HANDLE handle_to_give, bool inherit_handle = FALSE,
		DWORD desired_access = 0, DWORD options = DUPLICATE_SAME_ACCESS);


	/// <summary>
	/// 获取指定模块的模块句柄，该模块必须已经被当前进程加载(该函数在多线程不可靠，用EX版本)
	/// </summary>
	/// <param name="module_name">模块名字(exe或dll)，若为""，则返回主调进程的可执行文件的句柄(基地址)</param>
	/// <returns>返回指定模块的句柄(基地址)</returns>
	inline HMODULE get_module_handle(const std::string& module_name = "")
	{
		auto val = GetModuleHandleA(string_to_pointer(module_name));
		GET_ERROR_MSG_OUTPUT(std::cout);
		return val;
	}

	/// <summary>
	/// 进程信息结构体
	/// </summary>
	struct process_info
	{
		/// <summary>使用智能指针包装的进程句柄，当销毁时自动关闭句柄</summary>
		std::shared_ptr<HANDLE> process_handle; 
		/// <summary>使用智能指针包装的线程句柄，当销毁时自动关闭句柄</summary>
		std::shared_ptr<HANDLE> thread_handle; 
		/// <summary>进程ID</summary>
		DWORD process_id;	
		/// <summary>线程ID</summary>
		DWORD thread_id;
	};
	
	/// <summary>
	/// 创建一个新进程
	/// </summary>
	/// <remarks>封装自CreateProcessA</remarks>
	/// <param name="new_process_info">[out]返回进程信息，其包含进程和主线程句柄和ID</param>
	/// <param name="command_line">传给新进程的命令行，其中应该包含可执行文件的路径</param>
	/// <param name="process_work_dir">新进程的工作目录，默认值则与父进程相同</param>
	/// <param name="inherit_handle">是否将父进程可继承的句柄继承给新进程</param>
	/// <param name="creation_flags">创建标志</param>
	/// <param name="process_attributes">进程安全属性</param>
	/// <param name="thread_attributes">线程安全属性</param>
	/// <param name="environment">环境块，默认值则与父进程相同</param>
	/// <param name="startup_info">启动信息</param>
	/// <returns>是否成功</returns>
	MW_API bool create_process(process_info& new_process_info, const std::string& command_line,
			const std::string& process_work_dir = "", BOOL inherit_handle = FALSE, DWORD creation_flags = 0,
			LPSECURITY_ATTRIBUTES process_attributes = nullptr,
			LPSECURITY_ATTRIBUTES thread_attributes = nullptr,
			LPVOID environment = nullptr, LPSTARTUPINFOA startup_info = nullptr);

	/// <summary>
	/// 创建一个要求管理员的新进程(向用户弹出UAC对话框)
	/// </summary>
	/// <param name="file">新进程可执行文件的路径</param>
	/// <param name="command_line">新进程的命令行(不包含可执行文件路径)</param>
	/// <returns>是否成功</returns>
	MW_API bool create_process_admin(const std::string& file, const std::string& command_line = "");

	/// <summary>
	/// 判断当前线程所在进程的令牌是否是管理员组
	/// </summary>
	/// <returns>当前进程是否是管理员组</returns>
	MW_API bool is_current_process_admin();


}//mw