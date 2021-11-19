#pragma once

namespace mw {

	/// <summary>
	/// 将当前线程所在进程的一个句柄复制给目标进程(target_process)，并返回复制产生的句柄(注意该句柄应该是内核对象的句柄)
	/// </summary>
	/// <remarks>注意，返回的新句柄应该传给目标进程，源进程不要使用这个句柄(封装自DuplicateHandle)</remarks>
	/// <param name="target_process">目标进程，可以是本身</param>
	/// <param name="handle_to_give">要复制的句柄</param>
	/// <param name="inherit_handle">新句柄是否可以被目标进程新创建的子进程继承</param>
	/// <param name="desired_access">复制的新句柄的访问权限</param>
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
	inline HMODULE get_module_handle(const std::tstring& module_name = _T(""))
	{
		auto val = GetModuleHandle(tstring_to_pointer(module_name));
		GET_ERROR_MSG_OUTPUT(std::tcout);
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
	MW_API bool create_process(process_info& new_process_info, const std::tstring& command_line,
			const std::tstring& process_work_dir = _T(""), BOOL inherit_handle = FALSE, DWORD creation_flags = 0,
			LPSECURITY_ATTRIBUTES process_attributes = nullptr,
			LPSECURITY_ATTRIBUTES thread_attributes = nullptr,
			LPVOID environment = nullptr, LPSTARTUPINFO startup_info = nullptr);

	/// <summary>
	/// 创建一个要求管理员的新进程(向用户弹出UAC对话框)
	/// </summary>
	/// <param name="file">新进程可执行文件的路径</param>
	/// <param name="command_line">新进程的命令行(不包含可执行文件路径)</param>
	/// <returns>是否成功</returns>
	MW_API bool create_process_admin(const std::tstring& file, const std::tstring& command_line = TEXT(""));

	/// <summary>
	/// 判断当前线程所在进程的令牌是否是管理员组
	/// </summary>
	/// <returns>当前进程是否是管理员组</returns>
	MW_API bool is_current_process_admin();


	/// <summary>
	/// 获得调用线程所在进程的伪句柄(即不需要CloseHandle)
	/// </summary>
	/// <returns>返回当前进程的伪句柄。</returns>
	inline HANDLE get_current_process()
	{
		return GetCurrentProcess();
	}

	/// <summary>
	/// 获取调用进程的进程标识符,在进程终止之前，进程标识符在整个系统中唯一标识该进程。
	/// </summary>
	/// <returns>返回调用进程的进程标识符</returns>
	inline DWORD get_current_process_id()
	{
		return GetCurrentProcessId();
	}

	/// <summary>
	/// 获取指定进程的进程标识符(PID)
	/// </summary>
	/// <param name="target_process">指定进程的句柄，该句柄必须有PROCESS_QUERY_INFORMATION或PROCESS_QUERY_LIMITED_INFORMATION访问权限</param>
	/// <returns>若失败返回0，否则返回指定进程标识符</returns>
	inline DWORD get_process_id(HANDLE target_process)
	{
		auto val = GetProcessId(target_process);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 获取指定线程所在进程的标识符(PID)
	/// </summary>
	/// <param name="thread_handle">指定线程句柄，该句柄必须具有 THREAD_QUERY_INFORMATION或THREAD_QUERY_LIMITED_INFORMATION</param>
	/// <returns>函数失败返回0，否则返回指定线程所在进程的标识符</returns>
	inline DWORD get_process_id_from_thread(HANDLE thread_handle)
	{
		auto val = GetProcessIdOfThread(thread_handle);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 获取指定进程的退出代码，该函数立即返回
	/// </summary>
	/// <param name="target_process">指定进程的句柄，该句柄必须具有PROCESS_QUERY_INFORMATION或PROCESS_QUERY_LIMITED_INFORMATION访问权限</param>
	/// <param name="exit_code">[out]进程退出代码，若进程尚未终止且函数成功，返回STILL_ACTIVE，如果进程终止且函数成功，则返回值是退出进程的退出代码，具体看文档</param>
	/// <returns>操作是否成功</returns>
	inline bool get_process_exit_code(HANDLE target_process, DWORD& exit_code)
	{
		auto val = GetExitCodeProcess(target_process, &exit_code);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 终止指定进程及其所有线程，如是进程自行终止，该函数在停止调用线程并且不返回，否则该函数是异步的，它启动终止并立即返回，更多事项看文档。
	/// </summary>
	/// <remarks>不到万不得已，不要调用该函数</remarks>
	/// <param name="target_process">要终止的进程的句柄，该句柄必须具有PROCESS_TERMINATE访问权限</param>
	/// <param name="exit_code">指定由于此调用而终止的进程和线程使用的退出代码</param>
	/// <returns>操作是否成功</returns>
	inline bool terminate_process(HANDLE target_process, UINT exit_code)
	{
		auto val = TerminateProcess(target_process, exit_code);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 结束该调用进程及其所有线程，有DLL谨慎使用该函数，具体看文档
	/// </summary>
	/// <param name="exit_code">进程和所有线程的退出代码</param>
	inline void exit_process(UINT exit_code)
	{
		ExitProcess(exit_code);
	}

	/// <summary>
	/// 根据进程ID获取一个存在进程的内核对象句柄
	/// </summary>
	/// <param name="thread_id">指定要打开的进程ID</param>
	/// <param name="inherit_handle">若为TRUE，则返回的句柄是可继承句柄，否则不是</param>
	/// <param name="desired_access">新句柄的访问权限</param>
	/// <returns>若成功，返回指定进程的句柄，否则返回NULL</returns>
	inline HANDLE open_process(DWORD process_id, bool inherit_handle = false, DWORD desired_access = 0)
	{
		auto val = OpenProcess(desired_access, inherit_handle, process_id);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

}//mw