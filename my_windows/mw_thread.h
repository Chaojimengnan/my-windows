#pragma once
#include <process.h>


namespace mw {

	/// <summary>
	/// 获得调用线程所在线程的伪句柄(即不需要CloseHandle)
	/// </summary>
	/// <returns>返回当前线程的伪句柄。</returns>
	inline HANDLE get_current_thread()
	{
		return GetCurrentThread();
	}

	/// <summary>
	/// 获取调用线程的线程标识符,在线程终止之前，线程标识符在整个系统中唯一标识该进程。
	/// </summary>
	/// <returns>返回调用线程的线程标识符</returns>
	inline DWORD get_current_thread_id()
	{
		return GetCurrentThreadId();
	}

	/// <summary>
	/// 获取指定线程的线程标识符(TID)
	/// </summary>
	/// <param name="target_thread">指定线程的句柄，它必须具有THREAD_QUERY_INFORMATION或THREAD_QUERY_LIMITED_INFORMATION访问权限</param>
	/// <returns>若失败返回0，否则返回线程标识符</returns>
	inline DWORD get_thread_id(HANDLE target_thread)
	{
		auto val = GetThreadId(target_thread);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 结束该调用线程，C/C++中不要用这个函数，直接线程函数返回，这样才能正确调用析构函数和其他自动清理，就算要用也用_endthreadex。其他事项请看文档
	/// </summary>
	/// <param name="exit_code">线程的退出代码</param>
	inline void exit_thread(DWORD exit_code)
	{
		ExitThread(exit_code);
	}

	/// <summary>
	/// 终结一个线程，这是一个危险的函数，只能在最极端的情况下使用。详情看文档。
	/// </summary>
	/// <param name="thread_handle">要终止的线程的句柄，该句柄必须具有`THREAD_TERMINATE`访问权限</param>
	/// <param name="exit_code">线程的退出代码</param>
	/// <returns>操作是否成功</returns>
	inline bool terminate_thread(HANDLE thread_handle, DWORD exit_code)
	{
		auto val = TerminateThread(thread_handle, exit_code);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 减少线程的挂起计数。当挂起计数递减为零时，恢复线程的执行。
	/// </summary>
	/// <remarks>若返回0，则指定线程未挂起，如果返回1，则指定线程已挂起但已重新启动，如果返回值大于1，则指定线程仍处于挂起状态</remarks>
	/// <param name="thread_handle">要重新启动的线程的句柄，该句柄必须具有THREAD_SUSPEND_RESUME访问权限</param>
	/// <returns>若函数成功，返回线程之前的挂起计数，若失败返回-1</returns>
	inline DWORD resume_thread(HANDLE thread_handle)
	{
		auto val = ResumeThread(thread_handle);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 获取指定线程的退出代码，该函数立即返回
	/// </summary>
	/// <param name="target_thread">指定线程的句柄，该句柄必须具有THREAD_QUERY_INFORMATION或THREAD_QUERY_LIMITED_INFORMATION访问权限</param>
	/// <param name="exit_code">[out]线程退出代码，若线程尚未终止且函数成功，返回STILL_ACTIVE，如果线程终止且函数成功，则返回值是退出线程的退出代码，具体看文档</param>
	/// <returns>操作是否成功</returns>
	inline bool get_thread_exit_code(HANDLE target_thread, DWORD& exit_code)
	{
		auto val = GetExitCodeThread(target_thread, &exit_code);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 创建要在调用进程的虚拟地址空间内执行的线程，使用C/C++运行库的程序不要使用该函数创建线程！而是使用_beginthreadex
	/// </summary>
	/// <param name="thread_function">新创建的线程的线程函数的地址</param>
	/// <param name="parameter">将作为参数传给线程函数，它可以是程序定义的任意值或指针</param>
	/// <param name="thread_id">[out]返回新创建线程的ID，可以为NULL，则不返回线程ID</param>
	/// <param name="thread_attributes">线程安全属性</param>
	/// <param name="creation_flags">控制线程创建的标志，一般为0，可以为CREATE_SUSPENDED，该标志将在创建完新线程后暂停线程</param>
	/// <param name="stack_size">堆栈的初始大小，以字节为单位，系统将此值舍入到最近的页面，若为0，则使用可执行程序默认大小</param>
	/// <returns>若成功，返回值是新线程的句柄，若失败返回NULL</returns>
	inline HANDLE create_thread(LPTHREAD_START_ROUTINE thread_function, LPVOID parameter = nullptr, LPDWORD thread_id = nullptr,
		LPSECURITY_ATTRIBUTES thread_attributes = nullptr, DWORD creation_flags = 0, size_t stack_size = 0)
	{
		auto val = CreateThread(thread_attributes, stack_size, thread_function, parameter, creation_flags, thread_id);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 创建要在调用进程的虚拟地址空间内执行的线程，使用C/C++运行库的程序应该使用该函数来创建线程，该函数内部调用CreateThread，并做了额外工作保证运行库线程安全
	/// </summary>
	/// <param name="thread_function">新创建的线程的线程函数的地址</param>
	/// <param name="parameter">将作为参数传给线程函数，它可以是程序定义的任意值或指针</param>
	/// <param name="thread_id">[out]返回新创建线程的ID，可以为NULL，则不返回线程ID</param>
	/// <param name="thread_attributes">线程安全属性</param>
	/// <param name="creation_flags">控制线程创建的标志，一般为0，可以为CREATE_SUSPENDED，该标志将在创建完新线程后暂停线程</param>
	/// <param name="stack_size">堆栈的初始大小，以字节为单位，系统将此值舍入到最近的页面，若为0，则使用可执行程序默认大小</param>
	/// <returns>若成功，返回值是新线程的句柄，若失败返回NULL</returns>
	inline HANDLE c_create_thread(LPTHREAD_START_ROUTINE thread_function, LPVOID parameter = nullptr, LPDWORD thread_id = nullptr,
		LPSECURITY_ATTRIBUTES thread_attributes = nullptr, DWORD creation_flags = 0, size_t stack_size = 0)
	{
		auto val = (HANDLE)_beginthreadex((void*)thread_attributes, (unsigned int)stack_size, 
			(_beginthreadex_proc_type)thread_function, (void*)parameter, (unsigned int)creation_flags, (unsigned int*)thread_id);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 结束该调用线程，使用C/C++运行库的程序应该调用该函数而不是ExitThread，但是并不鼓励调用该函数，应该是线程函数自然返回。
	/// </summary>
	/// <param name="exit_code">线程的退出代码</param>
	inline void c_exit_thread(DWORD exit_code)
	{
		_endthreadex(exit_code);
	}

	/// <summary>
	/// 挂起指定线程，64位程序应该使用Wow64SuspendThread来挂起一个WOW64线程(32位线程)。不建议使用该函数挂起其他线程(有可能发生死锁)
	/// </summary>
	/// <remarks>
	/// 此函数主要供调试器使用。它不应该用于线程同步。如果调用线程尝试获取挂起线程拥有的同步对象，
	/// 则在拥有同步对象（例如互斥锁或临界区）的线程上调用 SuspendThread可能会导致死锁。为避免这种情况，应用程序中不是调试器的线程应该通知另一个线程挂起自己。
	/// 目标线程必须设计为监视此信号并做出适当的响应。
	/// 具体事项看文档
	/// </remarks>
	/// <param name="thread_handle">指定线程句柄，该句柄必须具有THREAD_SUSPEND_RESUME访问权限</param>
	/// <returns>若函数成功，返回之前的挂起次数，否则返回-1</returns>
	inline DWORD suspend_thread(HANDLE thread_handle)
	{
		auto val = SuspendThread(thread_handle);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 根据线程ID获取一个存在线程的内核对象句柄
	/// </summary>
	/// <param name="thread_id">指定要打开的线程ID</param>
	/// <param name="inherit_handle">若为TRUE，则返回的句柄是可继承句柄，否则不是</param>
	/// <param name="desired_access">新句柄的访问权限</param>
	/// <returns>若成功，返回指定线程的句柄，否则返回NULL</returns>
	inline HANDLE open_thread(DWORD thread_id, bool inherit_handle = false, DWORD desired_access = 0)
	{
		auto val = OpenThread(desired_access, inherit_handle, thread_id);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 使调用线程挂起milliseconds长的时间，注意有些情况下不要调用该函数，会引发死锁，请看文档
	/// </summary>
	/// <param name="milliseconds">挂起的时间，若为0，则表示调用线程放弃当前时间片(系统直接调度其他线程)，若为INFINITE，则直接挂起</param>
	inline VOID sleep(DWORD milliseconds)
	{
		Sleep(milliseconds);
	}

	/// <summary>
	/// 调用线程将执行权交给另一个准备好在当前处理器上运行的线程。操作系统选择下一个要执行的线程。该函数相比于Sleep传入0，允许执行低优先级线程，而Sleep则不然
	/// </summary>
	/// <remarks>其他事项请看文档</remarks>
	/// <returns>若调用该函数时没有其他线程可以运行，返回FALSE，否则返回非零值</returns>
	inline BOOL switch_to_thread()
	{
		return SwitchToThread();
	}

	/// <summary>
	/// 该函数只用于超线程CPU，调用线程放弃控制权交给该处理器的另一个线程
	/// </summary>
	inline VOID yield_processor() 
	{
		YieldProcessor();
	}

	/// <summary>
	/// 获取指定线程的计时信息，单位为100纳秒(ns)，FILETIME为两个32位值组成(兼容32位程序)
	/// </summary>
	/// <param name="thread_handle">指定线程的句柄，THREAD_QUERY_INFORMATION或THREAD_QUERY_LIMITED_INFORMATION访问权限</param>
	/// <param name="creation_time">[out]线程创建时间</param>
	/// <param name="exit_time">[out]线程退出时间，若线程仍然运行，退出时间是没有定义的</param>
	/// <param name="kernel_time">[out]线程执行内核代码所用时间量</param>
	/// <param name="user_time">[out]线程执行应用程序代码所用时间量</param>
	/// <returns>操作是否成功</returns>
	inline bool get_thread_times(HANDLE thread_handle, FILETIME& creation_time,
		FILETIME& exit_time, FILETIME& kernel_time, FILETIME& user_time)
	{
		auto val = GetThreadTimes(thread_handle, &creation_time, &exit_time, &kernel_time, &user_time);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 获取指定线程的上下文，64位应用程序可以使用Wow64GetThreadContext获取WOW64线程的上下文(32位线程)。在调用之前请先调用SuspendThread挂起指定线程
	/// </summary>
	/// <param name="thread_handle">指定线程的句柄，请确保指定线程已被挂起，否则无法获得有效上下文，该句柄必须具有THREAD_GET_CONTEXT访问权限</param>
	/// <param name="context">[in,out]一个用于接收上下文的CONTEXT结构体</param>
	/// <param name="context_flags">对应于CONTEXT结构体的ContextFlags成员，指定获取上下文的哪些部分，它可以是以CONTEXT_开头的宏的组合</param>
	/// <returns>操作是否成功</returns>
	inline bool get_thread_context(HANDLE thread_handle, CONTEXT& context, DWORD context_flags = CONTEXT_FULL)
	{
		context.ContextFlags = context_flags;
		auto val = GetThreadContext(thread_handle, &context);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 设置指定线程的上下文，64位应用程序可以使用Wow64SetThreadContext获取WOW64线程的上下文(32位线程)。在调用之前请先调用SuspendThread挂起指定线程
	/// </summary>
	/// <param name="thread_handle">指定线程的句柄，请确保指定线程已被挂起，否则结果是不可预测的。该句柄必须具有THREAD_SET_CONTEXT访问权限</param>
	/// <param name="context">用于设置上下文的CONTEXT结构体，其ContextFlags成员指定了设置哪些内容，它是以CONTEXT_开头的宏的组合</param>
	/// <returns>操作是否成功</returns>
	inline bool set_thread_context(HANDLE thread_handle, const CONTEXT& context)
	{
		auto val = SetThreadContext(thread_handle, &context);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

};//mw