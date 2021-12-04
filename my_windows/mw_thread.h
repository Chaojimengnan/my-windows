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
	[[deprecated(_T("你不应该使用该函数，而是使用c_exit_thread"))]]
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
	[[deprecated(_T("你不应该使用该函数，而是使用c_create_thread"))]]
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

	/// <summary>
	/// 设置指定线程相对优先级，该值与线程所在进程的优先级类一起确定线程的基本优先级(base priority level)，注意这个优先级只影响CPU调度
	/// </summary>
	/// <param name="thread_handle">指定要设置相对优先级的线程，其句柄必须具有THREAD_SET_INFORMATION或THREAD_SET_LIMITED_INFORMATION访问权限</param>
	/// <param name="priority">线程相对优先级值，它应该是特定宏的一个，请看文档</param>
	/// <returns>操作是否成功</returns>
	inline bool set_thread_priority(HANDLE thread_handle, int priority = THREAD_PRIORITY_NORMAL)
	{
		auto val = SetThreadPriority(thread_handle, priority);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 获取指定线程相对优先级，该值与线程所在进程的优先级类一起确定线程的基本优先级(base priority level)，注意这个优先级只影响CPU调度
	/// </summary>
	/// <param name="thread_handle">指定线程局部，其句柄必须具有THREAD_QUERY_INFORMATION或THREAD_QUERY_LIMITED_INFORMATION访问权限</param>
	/// <returns>若成功，返回值是线程相对优先级，若失败，返回值是THREAD_PRIORITY_ERROR_RETURN</returns>
	inline int get_thread_priority(HANDLE thread_handle)
	{
		auto val = GetThreadPriority(thread_handle);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 禁用或启用系统临时提高线程优先级的能力(若禁用则系统不能再临时提升指定线程的优先级)
	/// </summary>
	/// <param name="thread_handle">线程句柄，句柄必须具有THREAD_SET_INFORMATION或THREAD_SET_LIMITED_INFORMATION访问权限</param>
	/// <param name="is_disable_priority_boost">若为TRUE，则禁用动态提升，若为FALSE启用动态提升</param>
	/// <returns>操作是否成功</returns>
	inline bool set_thread_priority_boost(HANDLE thread_handle, BOOL is_disable_priority_boost)
	{
		auto val = SetThreadPriorityBoost(thread_handle, is_disable_priority_boost);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 获取指定线程的优先级提升控制状态，若为TRUE表示该线程动态提升已被禁用，若为FALSE表示该线程动态提升已启用
	/// </summary>
	/// <param name="thread_handle">线程的句柄，句柄必须具有THREAD_QUERY_INFORMATION或THREAD_QUERY_LIMITED_INFORMATION访问权限</param>
	/// <param name="is_disable_priority_boost">[out]用于接收优先级动态提升控制状态，为TRUE表示该线程动态提升已被禁用，若为FALSE表示该线程动态提升已启用</param>
	/// <returns>操作是否成功</returns>
	inline bool get_thread_priority_boost(HANDLE thread_handle, BOOL& is_disable_priority_boost)
	{
		auto val = GetThreadPriorityBoost(thread_handle, &is_disable_priority_boost);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 为指定线程设置处理器关联掩码，使得指定线程只能在CPU的一个子集上运行
	/// </summary>
	/// <param name="thread_handle">设置其关联掩码的线程的句柄，此句柄必须具有THREAD_SET_INFORMATION或THREAD_SET_LIMITED_INFORMATION访问权限以及THREAD_QUERY_INFORMATION或THREAD_QUERY_LIMITED_INFORMATION访问权限。</param>
	/// <param name="thread_affinity_mask">位掩码，它应该是所在进程掩码的一个子集。每一位代表对应的CPU是否可被使用，如0x5，即0101，CPU 0，CPU 2可以使用</param>
	/// <returns>操作是否成功</returns>
	inline DWORD_PTR set_thread_affinity_mask(HANDLE thread_handle, DWORD_PTR thread_affinity_mask)
	{
		auto val = SetThreadAffinityMask(thread_handle, thread_affinity_mask);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 为线程设置首选处理器。系统尽可能在其首选处理器上调度线程
	/// </summary>
	/// <param name="thread_handle">要设置其首选处理器的线程的句柄。句柄必须具有 THREAD_SET_INFORMATION 访问权限</param>
	/// <param name="ideal_processor">线程的首选处理器的编号。该值从0开始到MAXIMUM_PROCESSORS。如果此参数为MAXIMUM_PROCESSORS，则函数返回当前的首选处理器而不更改它</param>
	/// <returns>若成功，返回值是前一个首选处理器编号，若失败，返回值是(DWORD)-1</returns>
	inline DWORD set_thread_ideal_processor(HANDLE thread_handle, DWORD ideal_processor)
	{
		auto val = SetThreadIdealProcessor(thread_handle, ideal_processor);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

namespace sync {

	/// <summary>
	/// 执行两个32位值的原子加法，要对64位值进行操作，请使用InterlockedExchangeAdd64，addend参数指向的变量必须在32位边界上对齐(使用_aligned_malloc)
	/// </summary>
	/// <param name="addend">[in, out]指向变量的指针，此变量的值将替换为操作的结果</param>
	/// <param name="value">要添加到addend参数指向的变量的值</param>
	/// <returns>该函数返回addend参数的初始值</returns>
	inline LONG interlocked_exchange_add(LONG volatile& addend, LONG value)
	{
		return InterlockedExchangeAdd(&addend, value);
	}

	/// <summary>
	/// 执行两个64位值的原子加法，要对32位值进行操作，请使用InterlockedExchangeAdd，addend参数指向的变量必须在64位边界上对齐(使用_aligned_malloc)
	/// </summary>
	/// <param name="addend">[in, out]指向变量的指针，此变量的值将替换为操作的结果</param>
	/// <param name="value">要添加到addend参数指向的变量的值</param>
	/// <returns>该函数返回addend参数的初始值</returns>
	inline LONG64 interlocked_exchange_add64(LONG64 volatile& addend, LONG64 value)
	{
		return InterlockedExchangeAdd64(&addend, value);
	}

	/// <summary>
	/// 执行递增（增加一）的原子操作给指定的32位变量的值，要对64位值进行操作，请使用InterlockedIncrement64函数，addend参数指向的变量必须在32位边界上对齐(使用_aligned_malloc)
	/// </summary>
	/// <param name="addend">[in, out]指向要递增的变量的指针</param>
	/// <returns>该函数返回结果递增的值</returns>
	inline LONG interlocked_increment(LONG volatile& addend)
	{
		return InterlockedIncrement(&addend);
	}

	/// <summary>
	/// 执行递增（增加一）的原子操作给指定的64位变量的值，要对32位值进行操作，请使用InterlockedIncrement函数，addend参数指向的变量必须在64位边界上对齐(使用_aligned_malloc)
	/// </summary>
	/// <param name="addend">[in, out]指向要递增的变量的指针</param>
	/// <returns>该函数返回结果递增的值</returns>
	inline LONG64 interlocked_increment64(LONG64 volatile& addend)
	{
		return InterlockedIncrement64(&addend);
	}

	/// <summary>
	/// 执行将指定32位变量设置为指定值的原子操作
	/// </summary>
	/// <param name="target">[in, out]该函数将此变量设置为value，并返回其先前的值</param>
	/// <param name="value">要设置target的新值</param>
	/// <returns>返回target参数的初始值</returns>
	inline LONG interlocked_exchange(LONG volatile& target, LONG value)
	{
		return InterlockedExchange(&target, value);
	}

	/// <summary>
	/// 执行将指定64位变量设置为指定值的原子操作
	/// </summary>
	/// <param name="target">[in, out]该函数将此变量设置为value，并返回其先前的值</param>
	/// <param name="value">要设置target的新值</param>
	/// <returns>返回target参数的初始值</returns>
	inline LONG64 interlocked_exchange64(LONG64 volatile& target, LONG64 value)
	{
		return InterlockedExchange64(&target, value);
	}

	/// <summary>
	/// 执行将指定指针设置为指定值的原子操作，在64位系统上，参数是64位，并且target参数必须在64位边界上对齐，32位系统同理可得
	/// </summary>
	/// <param name="target">[in, out]该函数将此指针变量设置为value，并返回其先前的值</param>
	/// <param name="value">要设置target的新值</param>
	/// <returns>返回target参数的初始值</returns>
	inline PVOID interlocked_exchange_pointer(PVOID volatile& target, PVOID value)
	{
		return InterlockedExchangePointer(&target, value);
	}

	/// <summary>
	/// 对指定值执行原子比较和赋值操作。该函数比较两个指定的32位值,destination和comparand的值，若相等则destination被另一个32位值exchange赋值。
	/// </summary>
	/// <param name="destination">[in, out]指向目标值的指针，该函数的所有参数必须在32位边界上对齐</param>
	/// <param name="exchange">用于给destination赋值的值</param>
	/// <param name="comparand">要与destination进行比较的值，若与destination相等，则使用exchange赋值destination，否则不做任何事</param>
	/// <returns>该函数返回destination参数的初始值</returns>
	inline LONG interlocked_compare_exchange(LONG volatile& destination, LONG exchange, LONG comparand)
	{
		return InterlockedCompareExchange(&destination, exchange, comparand);
	}

	/// <summary>
	/// 对指定值执行原子比较和赋值操作。该函数比较两个指定的64位值,destination和comparand的值，若相等则destination被另一个64位值exchange赋值。
	/// </summary>
	/// <param name="destination">[in, out]指向目标值的指针，该函数的所有参数必须在64位边界上对齐</param>
	/// <param name="exchange">用于给destination赋值的值</param>
	/// <param name="comparand">要与destination进行比较的值，若与destination相等，则使用exchange赋值destination，否则不做任何事</param>
	/// <returns>该函数返回destination参数的初始值</returns>
	inline LONG64 interlocked_compare_exchange64(LONG64 volatile& destination, LONG64 exchange, LONG64 comparand)
	{
		return InterlockedCompareExchange64(&destination, exchange, comparand);
	}

	/// <summary>
	/// 对指定值执行原子比较和赋值操作。该函数比较两个指定的指针值,destination和comparand，若相等则destination被另一个指针exchange赋值。
	/// </summary>
	/// <param name="destination">[in, out]目标指针，若是64位程序该函数的所有参数必须在64位边界上对齐，32位同理</param>
	/// <param name="exchange">用于给destination赋值的值</param>
	/// <param name="comparand">要与destination进行比较的值，若与destination相等，则使用exchange赋值destination，否则不做任何事</param>
	/// <returns>该函数返回destination参数的初始值</returns>
	inline PVOID interlocked_compare_exchange_pointer(PVOID volatile& destination, PVOID exchange, PVOID comparand)
	{
		return InterlockedCompareExchangePointer(&destination, exchange, comparand);
	}

	/// <summary>
	/// 提供入栈和出栈原子操作的单向链表栈，适用于多线程读写
	/// </summary>
	/// <typeparam name="T">任意用户需要的数据类型</typeparam>
	template<typename T>
	class interlocked_list
	{
	public:
		/// <summary>
		/// 一个用于单向链表栈的链表项
		/// </summary>
		struct interlocked_list_struct
		{
			SLIST_ENTRY item_entry;
			T data;
		};

		interlocked_list(const interlocked_list&) = delete;
		interlocked_list(interlocked_list&&) = delete;
		interlocked_list& operator=(const interlocked_list&) = delete;
		interlocked_list& operator=(interlocked_list&&) = delete;

	public:

		interlocked_list() : list_head(make_aligned_heap<SLIST_HEADER>())
		{
			InitializeSListHead(list_head);
		}

		~interlocked_list()
		{
			PSLIST_ENTRY list_entry = nullptr;
			while (list_entry = InterlockedPopEntrySList(list_head))
			{
				// 为T类型调用析构函数，然后释放内存
				reinterpret_cast<interlocked_list_struct*>(list_entry)->data.~T();
				_aligned_free(list_entry);
			}
			_aligned_free(list_head);
		}

		/// <summary>
		/// 在单向链表的前面压入一个项。对链表的访问在多处理器系统上同步
		/// </summary>
		/// <param name="data">指定数据</param>
		/// <returns>操作是否失败</returns>
		bool push(const T& data)
		{
			interlocked_list_struct* item = make_aligned_heap<interlocked_list_struct>();
			if (item == nullptr)
				return false;
			// 在新申请的内存上构造T类型
			new(&item->data) T(data);
			InterlockedPushEntrySList(list_head, &(item->item_entry));
			return true;
		}

		/// <summary>
		/// 在单向链表的前面压入一个项。对链表的访问在多处理器系统上同步
		/// </summary>
		/// <param name="data">指定数据</param>
		/// <returns>操作是否失败</returns>
		bool push(T&& data)
		{
			interlocked_list_struct* item = make_aligned_heap<interlocked_list_struct>();
			if (item == nullptr)
				return false;
			// 在新申请的内存上构造T类型
			new(&item->data) T(std::move(data));
			InterlockedPushEntrySList(list_head, &(item->item_entry));
			return true;
		}

		/// <summary>
		/// 从单向链表的前面弹出一个项。对链表的访问在多处理器系统上同步
		/// </summary>
		/// <param name="data">[out]用于接收的数据</param>
		/// <returns>若链表为空，返回false，否则返回true</returns>
		bool pop(T& data)
		{
			PSLIST_ENTRY list_entry = InterlockedPopEntrySList(list_head);
			if (list_entry == nullptr)
				return false;
			interlocked_list_struct* item = reinterpret_cast<interlocked_list_struct*>(list_entry);
			data = item->data;
			// 为T类型调用析构函数，然后释放内存
			item->data.~T();
			_aligned_free(list_entry);
			return true;
		}

		/// <summary>
		/// 获取该单向链表的项的数量
		/// </summary>
		/// <returns>该单向链表的项的数量</returns>
		USHORT size()
		{
			return QueryDepthSList(list_head);
		}

	private:
		PSLIST_HEADER list_head;

		template<typename t_type>
		inline static t_type* make_aligned_heap()
		{
			return static_cast<t_type*>(_aligned_malloc(sizeof(t_type), MEMORY_ALLOCATION_ALIGNMENT));
		}
	};

	/// <summary>
	/// 关键段，用于同步访问某个和多个需要同步访问的资源(用于多线程)，它适用于当使用Interlocked系函数无法满足需求的情况。不能跨线程共享
	/// </summary>
	class critical_section
	{
		friend class condition_variable;

	public:
		/// <summary>
		/// 关键段构造函数
		/// </summary>
		/// <param name="is_spin">默认为true，即是否在enter时开启旋转锁轮询，若为false，则在被占用时直接进入等待</param>
		/// <param name="spin_count">旋转锁循环次数，若is_spin为false，该参数无效</param>
		critical_section(bool is_spin = true, DWORD spin_count = 4000)
		{
			if (!is_spin)
				spin_count = 0;
			InitializeCriticalSectionAndSpinCount(&cs, spin_count);
			GET_ERROR_MSG_OUTPUT(std::tcout);
		}
		~critical_section()
		{
			DeleteCriticalSection(&cs);
		}

	public:
		critical_section(const critical_section&) = delete;
		critical_section(critical_section&&) = delete;
		critical_section& operator=(const critical_section&) = delete;
		critical_section& operator=(critical_section&&) = delete;

	public:
		/// <summary>
		/// 尝试进入关键段，若返回值为true，则已进入关键段，你需要在结束访问同步资源之后调用leave函数，退出关键段。若为false则表示该关键段正被占用，你不需要调用leave函数
		/// </summary>
		/// <returns>表示是否进入关键段，若为true，你需要在访问同步资源结束后调用leave函数</returns>
		inline bool try_enter()
		{
			return TryEnterCriticalSection(&cs);
		}
		/// <summary>
		/// 进入关键段，若当前关键段正被其他线程占用，若开启旋转锁功能，则进行旋转锁循环，到达指定次数后，进入等待状态(否则直接进入等待)，直到进入关键段之后返回
		/// </summary>
		inline void enter()
		{
			EnterCriticalSection(&cs);
		}
		/// <summary>
		/// 退出关键段，在调用该函数后，你不能再对关联的同步资源进行访问。注意你调用了几次enter，就要调用几次leave，这样其他线程才能进入关键段
		/// </summary>
		inline void leave()
		{
			LeaveCriticalSection(&cs);
		}

		/// <summary>
		/// 模板方法，自动进入关键段并执行指定可调用对象，在可调用对象返回后自动退出关键段
		/// </summary>
		/// <param name="fun">指定可调用对象，它应该包含访问与该关键段关联的同步资源的代码</param>
		/// <param name="...args">转发给可调用对象fun的参数</param>
		template<typename Func, typename... Args>
		inline void into_section(Func fun, Args&&... args)
		{
			enter();
			fun(std::move(args)...);
			leave();
		}

		/// <summary>
		/// 模板方法，自动进入关键段并执行指定可调用对象，在可调用对象返回后自动退出关键段，该函数会返回可调用对象的返回值
		/// </summary>
		/// <param name="fun">指定可调用对象，它应该包含访问与该关键段关联的同步资源的代码</param>
		/// <param name="...args">转发给可调用对象fun的参数</param>
		/// <returns>指定可调用对象的返回值</returns>
		template<typename Func, typename... Args>
		inline auto into_section_return(Func fun, Args&&... args)
		{
			enter();
			auto return_val = fun(std::move(args)...);
			leave();
			return return_val;
		}

	private:
		CRITICAL_SECTION cs;

	};

	/// <summary>
	/// Slim读写锁，性能比关键段更强，支持多个读取者线程读取，但是相比于关键段，不能递归获取SRW锁，若能接受该限制，建议使用该同步机制。不能跨线程共享
	/// </summary>
	class slimrw_lock
	{
		friend class condition_variable;

	public:
		slimrw_lock()
		{
			InitializeSRWLock(&srw);
		}
		~slimrw_lock() {}

	public:
		slimrw_lock(const slimrw_lock&) = delete;
		slimrw_lock(slimrw_lock&&) = delete;
		slimrw_lock& operator=(const slimrw_lock&) = delete;
		slimrw_lock& operator=(slimrw_lock&&) = delete;

	public:
		/// <summary>
		/// 获取独占模式的slim读写锁，若该锁正在被独占模式或共享模式占用，调用线程进入等待(适用于写入同步资源)
		/// </summary>
		inline void acquire_exclusive()
		{
			AcquireSRWLockExclusive(&srw);
		}

		/// <summary>
		/// 获取共享模式的slim读写锁，若该锁正在被独占模式占用，调用线程进入等待(适用于读取同步资源)
		/// </summary>
		inline void acquire_shared()
		{
			AcquireSRWLockShared(&srw);
		}

		/// <summary>
		/// 释放独占模式获取的slim读写锁，slim读写锁必须由获取它的同一个线程释放
		/// </summary>
		inline void release_exclusive()
		{
			ReleaseSRWLockExclusive(&srw);
		}

		/// <summary>
		/// 释放共享模式获得的slim读写锁，slim读写锁必须由获取它的同一个线程释放
		/// </summary>
		inline void release_shared()
		{
			ReleaseSRWLockShared(&srw);
		}

		/// <summary>
		/// 尝试获取独占模式的slim读写锁，若该锁正在被独占模式或共享模式占用，则返回false，若成功获取，返回true
		/// </summary>
		/// <returns>是否成功获取独占模式的slim读写锁</returns>
		inline bool try_acquire_exclusive()
		{
			return TryAcquireSRWLockExclusive(&srw);
		}

		/// <summary>
		/// 尝试获取共享模式的slim读写锁，若该锁正在被独占模式占用，则返回false，若成功获取，返回true
		/// </summary>
		/// <returns>是否成功获取共享模式的slim读写锁</returns>
		inline bool try_acquire_shared()
		{
			return TryAcquireSRWLockShared(&srw);
		}

		/// <summary>
		/// 模板方法，自动进入独占模式的slim读写锁并执行指定可调用对象，在函数返回后释放独占模式的slim读写锁
		/// </summary>
		/// <param name="fun">指定可调用对象，它应该包含写入和访问同步资源的代码</param>
		/// <param name="...args">转发给可调用对象fun的参数</param>
		template<typename Func, typename... Args>
		inline void into_exclusive(Func fun, Args&&... args)
		{
			acquire_exclusive();
			fun(std::move(args)...);
			release_exclusive();
		}

		/// <summary>
		/// 模板方法，自动进入独占模式的slim读写锁并执行指定可调用对象，在函数返回后释放独占模式的slim读写锁，该函数会返回可调用对象的返回值
		/// </summary>
		/// <param name="fun">指定可调用对象，它应该包含写入和访问同步资源的代码</param>
		/// <param name="...args">转发给可调用对象fun的参数</param>
		/// <returns>指定可调用对象的返回值</returns>
		template<typename Func, typename... Args>
		inline auto into_exclusive_return(Func fun, Args&&... args)
		{
			acquire_exclusive();
			auto return_val = fun(std::move(args)...);
			release_exclusive();
			return return_val;
		}

		/// <summary>
		/// 模板方法，自动进入共享模式的slim读写锁并执行指定可调用对象，在函数返回后释放共享模式的slim读写锁
		/// </summary>
		/// <param name="fun">指定可调用对象，它应该包含访问同步资源的代码</param>
		/// <param name="...args">转发给可调用对象fun的参数</param>
		template<typename Func, typename... Args>
		inline void into_shared(Func fun, Args&&... args)
		{
			acquire_shared();
			fun(std::move(args)...);
			release_shared();
		}

		/// <summary>
		/// 模板方法，自动进入共享模式的slim读写锁并执行指定可调用对象，在函数返回后释放共享模式的slim读写锁，该函数会返回可调用对象的返回值
		/// </summary>
		/// <param name="fun">指定可调用对象，它应该包含访问同步资源的代码</param>
		/// <param name="...args">转发给可调用对象fun的参数</param>
		/// <returns>指定可调用对象的返回值</returns>
		template<typename Func, typename... Args>
		inline auto into_shared_return(Func fun, Args&&... args)
		{
			acquire_shared();
			auto return_val = fun(std::move(args)...);
			release_shared();
			return return_val;
		}

	private:
		SRWLOCK srw;

	};

	/// <summary>
	/// 条件变量是同步原语，使线程能够等待特定条件发生。条件变量是不能跨进程共享的用户模式对象。
	/// </summary>
	class condition_variable
	{
	public:
		condition_variable()
		{
			InitializeConditionVariable(&cv);
		}
		~condition_variable(){}

	public:
		condition_variable(const condition_variable&) = delete;
		condition_variable(condition_variable&&) = delete;
		condition_variable& operator=(const condition_variable&) = delete;
		condition_variable& operator=(condition_variable&&) = delete;

	public:
		/// <summary>
		/// 调用线程在该条件变量下睡眠，并将指定的关键段作为原子操作释放。另一个线程对该条件变量调用wake或wake_all唤醒线程，线程被唤醒后，重新获取线程进入休眠状态时释放的关键段
		/// </summary>
		/// <remarks>
		/// 条件变量会受到虚假唤醒（那些与显式唤醒无关的唤醒）和被盗唤醒（另一个线程设法在被唤醒线程之前运行）的影响。
		/// 因此，您应该在睡眠操作返回后重新检查谓词（通常在while循环中）。
		/// </remarks>
		/// <param name="cs">指定要释放的关键段，在线程唤醒后又会获取这个被释放的关键段</param>
		/// <param name="milliseconds_to_wait">超时值，以毫秒为单位，如果超时间隔已过，该函数将重新获取关键段并返回false。可以为INFINITE和0</param>
		/// <returns>若函数成功，返回true，若函数失败或超时值已过，返回值为false</returns>
		inline bool sleep_cs(critical_section& cs, DWORD milliseconds_to_wait = INFINITE)
		{
			auto val = SleepConditionVariableCS(&cv, &cs.cs, milliseconds_to_wait);
			GET_ERROR_MSG_OUTPUT(std::tcout);
			return val;
		}

		/// <summary>
		/// 调用线程在该条件变量下睡眠，并将指定的slim读写锁作为原子操作释放。另一个线程对该条件变量调用wake或wake_all唤醒线程，线程被唤醒后，重新获取线程进入休眠状态时释放的slim读写锁
		/// </summary>
		/// <remarks>
		/// 条件变量会受到虚假唤醒（那些与显式唤醒无关的唤醒）和被盗唤醒（另一个线程设法在被唤醒线程之前运行）的影响。
		/// 因此，您应该在睡眠操作返回后重新检查谓词（通常在while循环中）。注意，调用函数时，必须确保已经获取slim锁了，否则不可预计的后果
		/// </remarks>
		/// <param name="srwlock">指定要释放的slim读写锁，在线程唤醒后又会获取这个被释放的slim读写锁，此锁必须以flags参数指定的方式持有</param>
		/// <param name="flags">默认为独占模式的slim锁，若为CONDITION_VARIABLE_LOCKMODE_SHARED，则指定为共享模式</param>
		/// <param name="milliseconds_to_wait">超时值，以毫秒为单位，如果超时间隔已过，该函数将重新获取关键段并返回false。可以为INFINITE和0</param>
		/// <returns>若函数成功，返回true，若函数失败或超时值已过，返回值为false</returns>
		inline bool sleep_slimrw(slimrw_lock& srwlock, ULONG flags = 0, DWORD milliseconds_to_wait = INFINITE)
		{
			auto val = SleepConditionVariableSRW(&cv, &srwlock.srw, milliseconds_to_wait, flags);
			GET_ERROR_MSG_OUTPUT(std::tcout);
			return val;
		}

		/// <summary>
		/// 唤醒等待该条件变量的单个线程
		/// </summary>
		inline void wake()
		{
			WakeConditionVariable(&cv);
		}

		/// <summary>
		/// 唤醒等待该条件变量的所有线程
		/// </summary>
		inline void wake_all()
		{
			WakeAllConditionVariable(&cv);
		}

		/// <summary>
		/// 针对sleep_cs的包装，比起sleep_cs，当且仅在调用线程被唤醒时，且predicate返回true时，该函数才返回。这样可以避免虚假唤醒和被盗唤醒
		/// </summary>
		/// <param name="cs">指定要释放的关键段，在线程唤醒后又会获取这个被释放的关键段</param>
		/// <param name="predicate">检查谓词，当调用线程被唤醒时，且predicate返回true时，该函数才返回，否则重新进入睡眠</param>
		/// <param name="...args">转发给检查谓词的参数</param>
		/// <returns>若函数成功，返回true，若函数失败或超时值已过，返回值为false</returns>
		template<typename Pred, typename... Args>
		inline bool sleep_predicate_cs(critical_section& cs, Pred predicate, Args&&... args)
		{
			bool val = true;
			while (!predicate(std::move(args)...))
			{
				 val = sleep_cs(cs);
			}
			return val;
		}

		/// <summary>
		/// 针对sleep_slimrw的包装，比起sleep_slimrw，当且仅在调用线程被唤醒时，且predicate返回true时，该函数才返回。这样可以避免虚假唤醒和被盗唤醒
		/// </summary>
		/// <param name="srwlock">指定要释放的slim读写锁，在线程唤醒后又会获取这个被释放的slim读写锁，此锁必须以flags参数指定的方式持有</param>
		/// <param name="flags">默认为独占模式的slim锁，若为CONDITION_VARIABLE_LOCKMODE_SHARED，则指定为共享模式</param>
		/// <param name="predicate">检查谓词，当调用线程被唤醒时，且predicate返回true时，该函数才返回，否则重新进入睡眠</param>
		/// <param name="...args">转发给检查谓词的参数</param>
		/// <returns>若函数成功，返回true，若函数失败或超时值已过，返回值为false</returns>
		template<typename Pred, typename... Args>
		inline bool sleep_predicate_slimrw(slimrw_lock& srwlock, ULONG flags, Pred predicate, Args&&... args)
		{
			bool val = true;
			while (!predicate(std::move(args)...))
			{
				val = sleep_slimrw(srwlock, flags);
			}
			return val;
		}

	private:
		CONDITION_VARIABLE cv;

	};

	// 如上的线程同步机制都是用户模式下的同步机制，其特点是性能较内核模式同步机制快得多，但是除了Interlocked系列外均只能对一个进程下的所有线程进行同步
	// 而Interlocked系列同步机制只能同步简单数据，无法满足复杂需求。当我们需要同步不同进程的线程时，并且同步复杂数据时，需要使用内核模式下的同步机制
	// 下面是内核模式下的同步机制

	/// <summary>
	/// 等待指定内核对象处于触发状态，一个I/O完成例程或异步过程调用(APC)在线程队列中，或超时值已过，这三种情况时返回(根据参数不同)
	/// </summary>
	/// <remarks>
	/// 注意创建窗口的线程不要调用该函数和multiple版本，可能会导致死锁！因为创建窗口的线程要处理窗口消息。
	/// 使用MsgWaitFor*，而不是WaitFor*系函数。
	/// </remarks>
	/// <param name="object_handle">指定内核对象的句柄(支持类型查看文档)，该句柄必须具有SYNCHRONIZE访问权限，若句柄在等待时关闭，函数行为未定义</param>
	/// <param name="milliseconds_to_wait">超时值，可以为0或INFINITE，若为INFINITE，不会因为超时值而返回</param>
	/// <param name="alertable">若为true，一个I/O完成例程或异步过程调用(APC)在线程队列中时返回并执行他们，否则不返回，并且不会执行完成例程或 APC 函数</param>
	/// <returns>返回以WAIT_开头的宏，用于指示调用线程为什么继续执行</returns>
	inline DWORD wait_for_single_object(HANDLE object_handle, DWORD milliseconds_to_wait = INFINITE, bool alertable = false)
	{
		auto val = WaitForSingleObjectEx(object_handle, milliseconds_to_wait, alertable);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 等待指定一个或所有内核对象处于触发状态，一个I/O完成例程或异步过程调用(APC)在线程队列中，或超时值已过，这三种情况时返回(根据参数不同)
	/// </summary>
	/// <remarks>
	/// 注意创建窗口的线程不要调用该函数和single版本，可能会导致死锁！因为创建窗口的线程要处理窗口消息。
	/// 使用MsgWaitFor*，而不是WaitFor*系函数。
	/// </remarks>
	/// <param name="counts">内核句柄数组的数量，它不应该超过MAXIMUM_WAIT_OBJECTS，不能为0</param>
	/// <param name="object_handles">对象句柄数组,该数组可以是不同类型对象句柄(支持类型查看文档)。该句柄必须有SYNCHRONIZE访问权限，若句柄在等待时关闭，函数行为未定义</param>
	/// <param name="wait_all">若为false，则内核句柄数组其中一个触发时就返回，若为true，则所有内核句柄触发才返回</param>
	/// <param name="milliseconds_to_wait">超时值，可以为0或INFINITE，若为INFINITE，不会因为超时值而返回</param>
	/// <param name="alertable">若为true，一个I/O完成例程或异步过程调用(APC)在线程队列中时返回并执行他们，否则不返回，并且不会执行完成例程或 APC 函数</param>
	/// <returns>返回以WAIT_开头的宏，用于指示调用线程为什么继续执行</returns>
	inline DWORD wait_for_multiple_object(DWORD counts, const HANDLE* object_handles, bool wait_all = true , DWORD milliseconds_to_wait = INFINITE, bool alertable = false)
	{
		auto val = WaitForMultipleObjectsEx(counts, object_handles, wait_all, milliseconds_to_wait, alertable);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 创建或打开命名或未命名的事件内核对象并返回内核对象的句柄
	/// </summary>
	/// <param name="flags">它可以是CREATE_EVENT_INITIAL_SET和CREATE_EVENT_MANUAL_RESET的组合，前者指示初始触发还是不触发，后者指示手动还是自动</param>
	/// <param name="desired_access">返回的句柄具有的访问权限，它是EVENT_开头的宏</param>
	/// <param name="event_name">事件的名字，该名称限制为MAX_PATH个字符，区分大小写，可以为nullptr，即未命名的内核对象</param>
	/// <param name="event_attributes">事件的安全属性</param>
	/// <returns>函数成功返回事件对象的句柄，若失败返回NULL，若命名的事件对象在函数调用之前存在,则函数返回现有对象的句柄</returns>
	inline HANDLE create_event(DWORD flags = 0, DWORD desired_access = EVENT_MODIFY_STATE, const std::tstring& event_name = _T(""), LPSECURITY_ATTRIBUTES event_attributes = nullptr)
	{
		auto val = CreateEventEx(event_attributes, tstring_to_pointer(event_name), flags, desired_access);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 将指定的事件对象设置为触发状态，对于自动事件，被某一等待该事件的线程捕获后自动变为未触发状态，而手动事件除非调用ResetEvent，会保持触发状态
	/// </summary>
	/// <param name="event_handle">指定事件对象的句柄，句柄必须具有 EVENT_MODIFY_STATE 访问权限</param>
	/// <returns>操作是否成功</returns>
	inline bool set_event(HANDLE event_handle)
	{
		auto val = SetEvent(event_handle);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 将指定的事件对象设置为未触发状态，该函数主要用于手动重置事件对象，自动事件被某一等待该事件的线程捕获后自动变为未触发状态
	/// </summary>
	/// <param name="event_handle">指定事件对象的句柄，句柄必须具有 EVENT_MODIFY_STATE 访问权限</param>
	/// <returns>操作是否成功</returns>
	inline bool reset_event(HANDLE event_handle)
	{
		auto val = ResetEvent(event_handle);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 打开现有的命名事件对象
	/// </summary>
	/// <param name="event_name">要打开的事件的名称。名称比较区分大小写，此函数可以打开私有命名空间中的对象</param>
	/// <param name="inherit_handle">如果此值为TRUE，则此进程创建的子进程将继承句柄。否则，子进程不会继承这个句柄</param>
	/// <param name="desired_access">返回的句柄具有的访问权限，它是EVENT_开头的宏</param>
	/// <returns>函数成功，则返回值是事件对象的句柄。如果函数失败，则返回值为NULL</returns>
	inline HANDLE open_event(const std::tstring& event_name, bool inherit_handle = false, DWORD desired_access = EVENT_MODIFY_STATE)
	{
		auto val = OpenEvent(desired_access, inherit_handle, event_name.c_str());
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

};//sync

};//mw