#pragma once
#include "mw_utility.h"
#include "mw_security.h"
#include <psapi.h>

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
	inline HANDLE give_handle_to_other_process(HANDLE target_process,
		HANDLE handle_to_give, bool inherit_handle = FALSE,
		DWORD desired_access = 0, DWORD options = DUPLICATE_SAME_ACCESS)
	{
		HANDLE target_process_handle = nullptr;
		DuplicateHandle(GetCurrentProcess(), handle_to_give, target_process,
			&target_process_handle, desired_access, inherit_handle, options);
		GET_ERROR_MSG_OUTPUT();
		return target_process_handle;
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
	/// <remarks>封装自CreateProcess</remarks>
	/// <param name="new_process_info">[out]返回进程信息，其包含进程和主线程句柄和ID</param>
	/// <param name="command_line">传给新进程的命令行，其中应该包含可执行文件的路径</param>
	/// <param name="process_work_dir">新进程的工作目录，默认值则与父进程相同</param>
	/// <param name="inherit_handle">是否将父进程可继承的句柄继承给新进程</param>
	/// <param name="creation_flags">控制优先级类和进程创建的标志</param>
	/// <param name="process_attributes">进程安全属性</param>
	/// <param name="thread_attributes">线程安全属性</param>
	/// <param name="environment">环境块，默认值则与父进程相同</param>
	/// <param name="startup_info">启动信息</param>
	/// <returns>是否成功</returns>
	inline bool create_process(process_info& new_process_info, const std::tstring& command_line,
		const std::tstring& process_work_dir = _T(""), BOOL inherit_handle = FALSE, DWORD creation_flags = 0,
		LPSECURITY_ATTRIBUTES process_attributes = nullptr,
		LPSECURITY_ATTRIBUTES thread_attributes = nullptr,
		LPVOID environment = nullptr, LPSTARTUPINFO startup_info = nullptr)
	{
		STARTUPINFO temp;
		PROCESS_INFORMATION proc;
		TCHAR* temp_str = new TCHAR[command_line.size() + 1];
		_tcscpy_s(temp_str, command_line.size() + 1, command_line.c_str());
		if (!startup_info)
		{
			ZeroMemory(&temp, sizeof(STARTUPINFO));
			temp.cb = sizeof(STARTUPINFO);
			startup_info = &temp;
		}

		auto is_ok = CreateProcess(nullptr, temp_str, process_attributes,
			thread_attributes, inherit_handle, creation_flags,
			environment, tstring_to_pointer(process_work_dir), startup_info, &proc);
		GET_ERROR_MSG_OUTPUT();
		delete[] temp_str;

		new_process_info.process_handle = mw::safe_handle(proc.hProcess);
		new_process_info.thread_handle = mw::safe_handle(proc.hThread);
		new_process_info.process_id = proc.dwProcessId;
		new_process_info.thread_id = proc.dwThreadId;

		return is_ok;
	}

	/// <summary>
	/// 创建一个要求管理员的新进程(向用户弹出UAC对话框)
	/// </summary>
	/// <param name="file">新进程可执行文件的路径</param>
	/// <param name="command_line">新进程的命令行(不包含可执行文件路径)</param>
	/// <returns>是否成功</returns>
	inline bool create_process_admin(const std::tstring& file, const std::tstring& command_line = TEXT(""))
	{
		SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
		sei.lpVerb = _T("runas");
		sei.lpFile = file.c_str();
		sei.nShow = SW_SHOWNORMAL;
		if (command_line != _T(""))
			sei.lpParameters = command_line.c_str();

		auto val = ShellExecuteEx(&sei);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 判断当前线程所在进程的令牌是否是管理员组
	/// </summary>
	/// <returns>当前进程是否是管理员组</returns>
	inline bool is_current_process_admin()
	{
		auto admin_sid = mw::create_admin_sid();
		BOOL is_admin;
		CheckTokenMembership(nullptr, admin_sid.get(), &is_admin);
		GET_ERROR_MSG_OUTPUT();
		return is_admin;
	}


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
		GET_ERROR_MSG_OUTPUT();
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
		GET_ERROR_MSG_OUTPUT();
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
		GET_ERROR_MSG_OUTPUT();
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
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 结束该调用进程及其所有线程，有DLL谨慎使用该函数，具体看文档
	/// </summary>
	/// <param name="exit_code">进程和所有线程的退出代码</param>
	[[noreturn]] inline void exit_process(UINT exit_code)
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
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 获取指定进程的计时信息，单位为100纳秒(ns)，FILETIME为两个32位值组成(兼容32位程序)
	/// </summary>
	/// <param name="process_handle">指定进程的句柄，PROCESS_QUERY_INFORMATION或PROCESS_QUERY_LIMITED_INFORMATION访问权限</param>
	/// <param name="creation_time">[out]进程创建时间</param>
	/// <param name="exit_time">[out]进程退出时间，若进程仍然运行，退出时间是没有定义的</param>
	/// <param name="kernel_time">[out]进程执行内核代码所用时间量(进程下所有线程内核代码所用时间之和)</param>
	/// <param name="user_time">[out]进程执行应用程序代码所用时间量(进程下所有线程应用程序代码所用时间之和)，注意该时间可能超过实际时间(多核CPU情况)</param>
	/// <returns>操作是否成功</returns>
	inline bool get_process_times(HANDLE process_handle, FILETIME& creation_time,
		FILETIME& exit_time, FILETIME& kernel_time, FILETIME& user_time)
	{
		auto val = GetProcessTimes(process_handle, &creation_time, &exit_time, &kernel_time, &user_time);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 设置指定进程的优先级类，该值与进程的每个线程的优先级值一起确定每个线程的基本优先级(base priority level)，注意这个优先级只影响CPU调度
	/// </summary>
	/// <param name="process_handle">指定进程句柄，该句柄必须具有PROCESS_SET_INFORMATION访问权限</param>
	/// <param name="priority_class">优先级值，它应该是特定的宏的一个，具体请看文档</param>
	/// <returns>操作是否成功</returns>
	inline bool set_priority_class(HANDLE process_handle, DWORD priority_class  = NORMAL_PRIORITY_CLASS)
	{
		auto val = SetPriorityClass(process_handle, priority_class);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 获取指定进程的优先级类，该值与进程的每个线程的优先级值一起确定每个线程的基本优先级(base priority level)，注意这个优先级只影响CPU调度
	/// </summary>
	/// <param name="process_handle">指定的进程句柄，该句柄必须具有PROCESS_QUERY_INFORMATION或PROCESS_QUERY_LIMITED_INFORMATION访问权限</param>
	/// <returns>若成功，返回指定进程的优先级类的值，若失败，返回0</returns>
	inline DWORD get_priority_class(HANDLE process_handle)
	{
		auto val = GetPriorityClass(process_handle);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 禁用或启用系统临时提高指定进程中所有线程的优先级的能力(若禁用则系统不能再临时提升该进程下任何一个线程的优先级，包括之后创建的任何线程)
	/// </summary>
	/// <param name="process_handle">进程的句柄，该句柄必须具有PROCESS_SET_INFORMATION访问权限</param>
	/// <param name="is_disable_priority_boost">若为TRUE，则禁用动态提升，若为FALSE启用动态提升</param>
	/// <returns>操作是否成功</returns>
	inline bool set_priority_class_boost(HANDLE process_handle, BOOL is_disable_priority_boost)
	{
		auto val = SetProcessPriorityBoost(process_handle, is_disable_priority_boost);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 获取指定进程的优先级提升控制状态，若为TRUE表示该进程下所有线程动态提升已被禁用，若为FALSE表示该进程下所有线程动态提升已启用
	/// </summary>
	/// <param name="process_handle">进程句柄，该句柄必须具有PROCESS_QUERY_INFORMATION或PROCESS_QUERY_LIMITED_INFORMATION访问权限</param>
	/// <param name="is_disable_priority_boost">[out]用于接收优先级动态提升控制状态，为TRUE表示该进程动态提升已被禁用，若为FALSE表示该进程动态提升已启用</param>
	/// <returns>操作是否成功</returns>
	inline bool get_priority_class_boost(HANDLE process_handle, BOOL& is_disable_priority_boost)
	{
		auto val = GetProcessPriorityBoost(process_handle, &is_disable_priority_boost);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 为指定进程下所有线程设置处理器关联掩码，使得指定进程下的线程只能在CPU的一个子集上运行。注意不要在DLL上调用该函数，详见文档
	/// </summary>
	/// <remarks>该关联掩码还会影响之后创建的子进程的线程的关联掩码，但不会影响之前创建的子进程</remarks>
	/// <param name="process_handle">指定要设置关联掩码的进程，该句柄必须具有PROCESS_SET_INFORMATION访问权限</param>
	/// <param name="process_affinity_mask">位掩码，它应该是系统掩码的一个子集。每一位代表对应的CPU是否可被使用，如0x5，即0101，CPU 0，CPU 2可以使用</param>
	/// <returns>操作是否成功</returns>
	inline bool set_process_affinity_mask(HANDLE process_handle, DWORD_PTR process_affinity_mask)
	{
		auto val = SetProcessAffinityMask(process_handle, process_affinity_mask);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 获取指定进程的进程关联掩码和系统的系统关联掩码，进程关联掩码是一个位向量，其中每一位代表允许进程运行的处理器。系统关联掩码是一个位向量，其中每一位代表配置到系统中的处理器
	/// </summary>
	/// <remarks>进程关联掩码是系统关联掩码的子集。进程只允许在配置到系统中的处理器上运行。因此，当系统关联掩码为处理器指定 0 位时，进程关联掩码不能为处理器指定 1 位</remarks>
	/// <param name="process_handle">需要关联掩码的进程的句柄，此句柄必须具有PROCESS_QUERY_INFORMATION或PROCESS_QUERY_LIMITED_INFORMATION访问权限</param>
	/// <param name="process_affinity_mask">[out]接收指定进程的关联掩码的变量</param>
	/// <param name="system_affinity_mask">[out]接收系统关联掩码的变量</param>
	/// <returns>操作是否成功</returns>
	inline bool get_process_affinity_mask(HANDLE process_handle, DWORD_PTR& process_affinity_mask, DWORD_PTR& system_affinity_mask)
	{
		auto val = GetProcessAffinityMask(process_handle, &process_affinity_mask, &system_affinity_mask);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 获取与指定进程有关的内存使用情况的信息
	/// </summary>
	/// <param name="process_handle">指定进程的句柄，该句柄必须具有PROCESS_QUERY_INFORMATION或PROCESS_QUERY_LIMITED_INFORMATION访问权限</param>
	/// <param name="process_memory_counters">[out]用户分配的PROCESS_MEMORY_COUNTERS结构体的指针，用于接收指定进程内存使用信息</param>
	/// <returns>操作是否成功</returns>
	inline BOOL get_process_memory_info(HANDLE process_handle, PROCESS_MEMORY_COUNTERS& process_memory_counters)
	{
		process_memory_counters.cb = sizeof(PROCESS_MEMORY_COUNTERS);
		auto val = GetProcessMemoryInfo(process_handle, &process_memory_counters, sizeof(PROCESS_MEMORY_COUNTERS));
		GET_ERROR_MSG_OUTPUT();
		return val;
	}


}//mw