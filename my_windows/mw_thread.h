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
    GET_ERROR_MSG_OUTPUT();
    return val;
}

/// <summary>
/// 结束该调用线程，C/C++中不要用这个函数，直接线程函数返回，这样才能正确调用析构函数和其他自动清理，就算要用也用_endthreadex。其他事项请看文档
/// </summary>
/// <param name="exit_code">线程的退出代码</param>
[[deprecated(_T("你不应该使用该函数，而是使用c_exit_thread"))]] [[noreturn]] inline void exit_thread(DWORD exit_code)
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
    GET_ERROR_MSG_OUTPUT();
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
    GET_ERROR_MSG_OUTPUT();
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
    GET_ERROR_MSG_OUTPUT();
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
[[deprecated(_T("你不应该使用该函数，而是使用c_create_thread"))]] inline HANDLE create_thread(LPTHREAD_START_ROUTINE thread_function, LPVOID parameter = nullptr, LPDWORD thread_id = nullptr,
    LPSECURITY_ATTRIBUTES thread_attributes = nullptr, DWORD creation_flags = 0, size_t stack_size = 0)
{
    auto val = CreateThread(thread_attributes, stack_size, thread_function, parameter, creation_flags, thread_id);
    GET_ERROR_MSG_OUTPUT();
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
    GET_ERROR_MSG_OUTPUT();
    return val;
}

/// <summary>
/// 创建在另一个进程的虚拟地址空间中运行的线程
/// </summary>
/// <remarks>
/// 请注意，即使lpStartAddress指向数据、代码或不可访问， CreateRemoteThread也可能成功。如果线程运行时起始地址无效，
/// 则发生异常，线程终止。由于起始地址无效而导致的线程终止被视为线程进程的错误退出。
/// </remarks>
/// <param name="process_handle">要在其中创建线程的进程句柄，该句柄必须具有PROCESS_CREATE_THREAD、PROCESS_QUERY_INFORMATION、PROCESS_VM_OPERATION、PROCESS_VM_WRITE和PROCESS_VM_READ访问权限</param>
/// <param name="thread_function">新创建的线程的线程函数的地址，该函数必须存在于远程进程中</param>
/// <param name="parameter">将作为参数传给线程函数，它可以是程序定义的任意值或指针</param>
/// <param name="thread_id">[out]返回新创建线程的ID，可以为NULL，则不返回线程ID</param>
/// <param name="thread_attributes">线程安全属性</param>
/// <param name="creation_flags">控制线程创建的标志，一般为0，可以为CREATE_SUSPENDED，该标志将在创建完新线程后暂停线程</param>
/// <param name="stack_size">堆栈的初始大小，以字节为单位，系统将此值舍入到最近的页面，若为0，则使用可执行程序默认大小</param>
/// <returns>若函数成功，返回新线程的句柄，若失败返回NULL</returns>
inline HANDLE create_remote_thread(HANDLE process_handle, LPTHREAD_START_ROUTINE thread_function, LPVOID parameter = nullptr, LPDWORD thread_id = nullptr,
    LPSECURITY_ATTRIBUTES thread_attributes = nullptr, DWORD creation_flags = 0, size_t stack_size = 0)
{
    auto val = CreateRemoteThread(process_handle, thread_attributes, stack_size, thread_function, parameter, creation_flags, thread_id);
    GET_ERROR_MSG_OUTPUT();
    return val;
}

/// <summary>
/// 结束该调用线程，使用C/C++运行库的程序应该调用该函数而不是ExitThread，但是并不鼓励调用该函数，应该是线程函数自然返回。
/// </summary>
/// <param name="exit_code">线程的退出代码</param>
[[noreturn]] inline void c_exit_thread(DWORD exit_code)
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
    GET_ERROR_MSG_OUTPUT();
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
    GET_ERROR_MSG_OUTPUT();
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
    GET_ERROR_MSG_OUTPUT();
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
    GET_ERROR_MSG_OUTPUT();
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
    GET_ERROR_MSG_OUTPUT();
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
    GET_ERROR_MSG_OUTPUT();
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
    GET_ERROR_MSG_OUTPUT();
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
    GET_ERROR_MSG_OUTPUT();
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
    GET_ERROR_MSG_OUTPUT();
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
    GET_ERROR_MSG_OUTPUT();
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
    GET_ERROR_MSG_OUTPUT();
    return val;
}

/// <summary>
/// 将用户模式异步过程调用(APC) 对象添加到指定线程的 APC 队列，可以将APC加入其他进程的线程，但是并不推荐，建议只用于本进程的线程
/// </summary>
/// <remarks>当指定线程处于可提醒状态时，将执行指定的APC，若线程不处于，则被添加到指定线程的APC队列中</remarks>
/// <param name="thread_handle">线程的句柄。句柄必须具有THREAD_SET_CONTEXT访问权限</param>
/// <param name="func_APC">指向应用程序提供的 APC 函数的指针，当指定的线程处于可提醒状态时将调用该函数</param>
/// <param name="data">该参数将传递给APC函数</param>
/// <returns>操作是否成功</returns>
inline DWORD queue_user_APC(HANDLE thread_handle, PAPCFUNC func_APC, ULONG_PTR data = 0)
{
    auto val = QueueUserAPC(func_APC, thread_handle, data);
    GET_ERROR_MSG_OUTPUT();
    return val;
}

/// <summary>
/// 请求线程池工作线程调用指定的回调函数
/// </summary>
/// <param name="func">指定回调函数，它将被推送到队列中，然后被线程池中的一个线程执行</param>
/// <param name="param">与回调函数配套的参数，它将被传入回调函数的参数中</param>
/// <param name="pcbe">一个指向TP_CALLBACK_ENVIRON结构的指针，该结构定义了执行回调函数的环境，若为NULL，在默认回调环境中执行</param>
/// <returns>操作是否成功</returns>
inline BOOL try_submit_threadpool_callback(PTP_SIMPLE_CALLBACK func, PVOID param = nullptr,
    PTP_CALLBACK_ENVIRON pcbe = nullptr)
{
    auto val = TrySubmitThreadpoolCallback(func, param, pcbe);
    GET_ERROR_MSG_OUTPUT();
    return val;
}

/// <summary>
/// 在用户模式内存中创建一个线程池工作项结构体，并将其结构体指针返回，你可以使用SubmitThreadpoolWork来提交这个工作项来让线程池来处理
/// </summary>
/// <param name="func_work">指定回调函数，每次你调用SubmitThreadpoolWork推送该工作项时，工作线程就会调用此回调</param>
/// <param name="param">与回调函数配套的参数，它将被传入回调函数的参数中</param>
/// <param name="pcbe">一个指向TP_CALLBACK_ENVIRON结构的指针，该结构定义了执行回调函数的环境，若为NULL，在默认回调环境中执行</param>
/// <returns>若成功返回工作项结构的指针，不要修改此结构的成员，若失败返回NULL</returns>
inline PTP_WORK create_threadpool_work(PTP_WORK_CALLBACK func_work, PVOID param = nullptr,
    PTP_CALLBACK_ENVIRON pcbe = nullptr)
{
    auto val = CreateThreadpoolWork(func_work, param, pcbe);
    GET_ERROR_MSG_OUTPUT();
    return val;
}

/// <summary>
/// 释放指定工作项结构体，若没有未完成的回调，则立即释放工作对象；否则，工作对象将在未完成的回调完成后异步释放
/// </summary>
/// <param name="work_item">指向工作项结构的指针，它应该是调用CreateThreadpoolWork的返回值</param>
inline void close_threadpool_work(PTP_WORK work_item)
{
    CloseThreadpoolWork(work_item);
}

/// <summary>
/// 将指定工作项推送到线程池中，工作线程会调用工作项中的回调函数，你可以一次或多次推送同一个工作项，而无须等待先前的回调完成，回调将并行执行
/// </summary>
/// <param name="work_item">指向工作项结构的指针，它应该是调用CreateThreadpoolWork的返回值</param>
inline void submit_threadpool_work(PTP_WORK work_item)
{
    SubmitThreadpoolWork(work_item);
}

/// <summary>
/// 等待未完成的工作回调完成并可选择取消尚未开始执行的挂起回调
/// </summary>
/// <param name="work_item">指向工作项结构的指针，它应该是调用CreateThreadpoolWork的返回值</param>
/// <param name="cancel_pending_callbacks">是否取消尚未开始执行的排队回调，若为TRUE，除了正在执行的回调，其他指定工作项回调将被取消</param>
inline void wait_for_threadpool_work_callbacks(PTP_WORK work_item, BOOL cancel_pending_callbacks = false)
{
    WaitForThreadpoolWorkCallbacks(work_item, cancel_pending_callbacks);
}

/// <summary>
/// 在用户模式内存中创建一个线程池计时器结构体，并将其结构体指针返回，使用SetThreadpoolTimer在线程池中注册该计时器并指定推送时间和周期
/// </summary>
/// <param name="func_timer">指定回调函数，当计时器到期时，工作线程就会调用此回调</param>
/// <param name="param">与回调函数配套的参数，它将被传入回调函数的参数中</param>
/// <param name="pcbe">一个指向TP_CALLBACK_ENVIRON结构的指针，该结构定义了执行回调函数的环境，若为NULL，在默认回调环境中执行</param>
/// <returns>若成功返回计时器结构的指针，不要修改此结构的成员，若失败返回NULL</returns>
inline PTP_TIMER create_threadpool_timer(PTP_TIMER_CALLBACK func_timer, PVOID param = nullptr,
    PTP_CALLBACK_ENVIRON pcbe = nullptr)
{
    auto val = CreateThreadpoolTimer(func_timer, param, pcbe);
    GET_ERROR_MSG_OUTPUT();
    return val;
}

/// <summary>
/// 释放指定计时器结构体，若没有未完成的回调，则立即释放计时器对象；否则，计时器对象将在未完成的回调完成后异步释放
/// </summary>
/// <remarks>
/// 为了避免回调函数在调用Close函数之后运行，先使用SetThreadpoolTimer将DueTime设为NULL，
/// 并将msPeriod和msWindowLength参数设置为 0。然后调用WaitForThreadpoolTimerCallbacks，
/// 最后调用CloseThreadpoolTimer
/// </remarks>
/// <param name="timer">指向计时器结构的指针，它应该是调用CreateThreadpoolTimer的返回值</param>
inline void close_threadpool_timer(PTP_TIMER timer)
{
    CloseThreadpoolTimer(timer);
}

/// <summary>
/// 设置计时器对象，替换之前的计时器（如果有）。工作线程在指定的超时值(timeout)到期后调用计时器对象的回调。
/// </summary>
/// <param name="timer">指向计时器结构的指针，它应该是调用CreateThreadpoolTimer的返回值</param>
/// <param name="due_time">若为正数或0，则表示1601.1.1以来的绝对时间(100纳秒为单位)，若为负数则是相对于当前时间的等待量，若为NULL则计时器停止推送到队列</param>
/// <param name="period">计时器周期，以毫秒为单位，若该参数为0，则只发送一次信号，若此参数大于0，则是周期性的。</param>
/// <param name="window_length">以毫秒为单位，系统在调用计时器回调之前可以延迟的最长时间。如果设置此参数，系统可以批量调用以节省资源</param>
inline void set_threadpool_timer(PTP_TIMER timer, PFILETIME due_time, DWORD period = 0, DWORD window_length = 0)
{
    SetThreadpoolTimer(timer, due_time, period, window_length);
}

/// <summary>
/// 确认指定计时器对象当前是否被设置了(即due_time是否为非NULL)
/// </summary>
/// <param name="timer">指向计时器结构的指针，它应该是调用CreateThreadpoolTimer的返回值</param>
/// <returns>如果设置了返回TRUE，否则返回FALSE，实际上就是due_time参数是否设置为了非NULL</returns>
inline BOOL is_threadpool_timer_set(PTP_TIMER timer)
{
    return IsThreadpoolTimerSet(timer);
}

/// <summary>
/// 等待未完成的计时器回调完成并可选择取消尚未开始执行的挂起回调。
/// </summary>
/// <param name="timer">指向计时器结构的指针，它应该是调用CreateThreadpoolTimer的返回值</param>
/// <param name="cancel_pending_callbacks">是否取消尚未开始执行的排队回调，若为TRUE，除了正在执行的回调，其他指定工作项回调将被取消</param>
inline void wait_for_threadpool_timer_callbacks(PTP_TIMER timer, BOOL cancel_pending_callbacks = false)
{
    WaitForThreadpoolTimerCallbacks(timer, cancel_pending_callbacks);
}

/// <summary>
/// 在用户模式内存中创建一个线程池等待结构体，并将其结构体指针返回
/// </summary>
/// <param name="func_wait">指定回调函数，当等待完成(指定等待内核对象触发)或超时时线程池调用回调函数</param>
/// <param name="param">与回调函数配套的参数，它将被传入回调函数的参数中</param>
/// <param name="pcbe">一个指向TP_CALLBACK_ENVIRON结构的指针，该结构定义了执行回调函数的环境，若为NULL，在默认回调环境中执行</param>
/// <returns>若成功返回等待结构的指针，不要修改此结构的成员，若失败返回NULL</returns>
inline PTP_WAIT create_threadpool_wait(PTP_WAIT_CALLBACK func_wait, PVOID param = nullptr,
    PTP_CALLBACK_ENVIRON pcbe = nullptr)
{
    auto val = CreateThreadpoolWait(func_wait, param, pcbe);
    GET_ERROR_MSG_OUTPUT();
    return val;
}

/// <summary>
/// 释放指定等待结构体，若没有未完成的回调，则立即释放等待对象；否则，等待对象将在未完成的回调完成后异步释放
/// </summary>
/// <remarks>
/// 为了避免回调函数在调用Close函数之后运行，先使用SetThreadpoolWait将h设为NULL，
/// 。然后调用WaitForThreadpoolWaitCallbacks，最后调用CloseThreadpoolWait
/// </remarks>
/// <param name="wait">指向等待结构的指针，它应该是调用CreateThreadpoolWait的返回值</param>
inline void close_threadpool_wait(PTP_WAIT wait)
{
    CloseThreadpoolWait(wait);
}

/// <summary>
/// 设置等待对象，替换之前的等待对象（如果有）。在句柄发出信号后或指定的超时到期后，工作线程调用等待对象的回调函数。
/// </summary>
/// <remarks>一个等待对象只能等待一个句柄。设置等待对象的句柄会替换之前的句柄（如果有）。
/// 您必须在每次发送信号以触发等待回调之前使用等待对象重新注册该事件。
/// (即当指定对象触发后，并且调用了对应的回调之后，你必须再次注册一次，否则对象再次触发也不会调用回调了)</remarks>
/// <param name="wait">指向等待结构的指针，它应该是调用CreateThreadpoolWait的返回值</param>
/// <param name="object_to_wait">它是一个内核对象句柄，若为NULL，则停止推送到队列中，若不为NULL，它应该是一个有效的可等待对象(若句柄在等待时关闭，函数未定义)</param>
/// <param name="timeout">若为正数或0，则表示1601.1.1以来的绝对时间(100纳秒为单位)，若为负数则是相对于当前时间的等待量，若为NULL则等待不会超时</param>
inline void set_threadpool_wait(PTP_WAIT wait, HANDLE object_to_wait, PFILETIME timeout = nullptr)
{
    SetThreadpoolWait(wait, object_to_wait, timeout);
}

/// <summary>
/// 等待未完成的等待回调完成，并可选择取消尚未开始执行的挂起回调。
/// </summary>
/// <param name="wait">指向等待结构的指针，它应该是调用CreateThreadpoolWait的返回值</param>
/// <param name="cancel_pending_callbacks">是否取消尚未开始执行的排队回调，若为TRUE，除了正在执行的回调，其他指定工作项回调将被取消</param>
inline void wait_for_threadpool_wait_callbakcs(PTP_WAIT wait, BOOL cancel_pending_callbacks = false)
{
    WaitForThreadpoolWaitCallbacks(wait, cancel_pending_callbacks);
}

/// <summary>
/// 在用户模式内存中创建一个线程池I/O完成结构体，并将其结构体指针返回
/// </summary>
/// <param name="file_handle">一个文件或设备句柄(它应该是OVERLAPPED打开的)，它将绑定到这个I/O完成结构体中</param>
/// <param name="func_io">每次文件或设备完成重叠I/O操作时线程池调用的回调函数</param>
/// <param name="param">与回调函数配套的参数，它将被传入回调函数的参数中</param>
/// <param name="pcbe">一个指向TP_CALLBACK_ENVIRON结构的指针，该结构定义了执行回调函数的环境，若为NULL，在默认回调环境中执行</param>
/// <returns>若成功返回I/O完成结构的指针，不要修改此结构的成员，若失败返回NULL</returns>
inline PTP_IO create_threadpool_io(HANDLE file_handle, PTP_WIN32_IO_CALLBACK func_io, PVOID param = nullptr,
    PTP_CALLBACK_ENVIRON pcbe = nullptr)
{
    auto val = CreateThreadpoolIo(file_handle, func_io, param, pcbe);
    GET_ERROR_MSG_OUTPUT();
    return val;
}

/// <summary>
/// 释放指定io完成结构体，若没有未完成的回调，则立即释放等待对象；否则，等待对象将在未完成的回调完成后异步释放
/// </summary>
/// <remarks>
/// 在调用此函数之前，您应该关闭关联的文件句柄并等待所有未完成的重叠 I/O 操作完成。调用此函数后，
/// 您不能再导致发生任何重叠的 I/O 操作。
///
/// 可能需要取消线程池 I/O 通知以防止内存泄漏，参阅CancelThreadpoolIo
/// </remarks>
/// <param name="io">指向io完成结构的指针，它应该是调用CreateThreadpoolIo的返回值</param>
inline void close_threadpool_io(PTP_IO io)
{
    CloseThreadpoolIo(io);
}

/// <summary>
/// 通知线程池，指定I/O完成结构体绑定的I/O操作可能已经开始。当I/O操作完成时，工作线程将调用绑定的回调函数。注意，每一次异步IO之前都要调用该函数
/// </summary>
/// <remarks>
/// 您必须在对绑定到 I/O 完成对象的文件句柄启动 *每个* 异步 I/O 操作之前调用此函数。如果不这样做，
/// 将导致线程池在完成时忽略 I/O 操作并导致内存损坏。
///
/// 如果 I/O 操作失败，则调用CancelThreadpoolIo函数取消此通知。
///
/// 如果绑定到 I/O 完成对象的文件句柄具有通知模式 FILE_SKIP_COMPLETION_PORT_ON_SUCCESS
/// 并且异步 I/O 操作成功立即返回，则不会调用该对象的 I/O 完成回调函数，并且必须取消线程池 I/O 通知
/// (使用CancelThreadpoolIo)
/// </remarks>
/// <param name="io">指向io完成结构的指针，它应该是调用CreateThreadpoolIo的返回值</param>
inline void start_threadpool_io(PTP_IO io)
{
    StartThreadpoolIo(io);
}

/// <summary>
/// 取消来自StartThreadpoolIo函数的通知，注意，若异步IO失败(调用write和read失败并不是997)，你需要调用该函数
/// </summary>
/// <remarks>
/// 为了防止内存泄漏，您必须为以下任一场景调用CancelThreadpoolIo函数：
/// - 重叠（异步） I/O 操作失败（即异步 I/O 函数调用返回失败，错误代码不是 ERROR_IO_PENDING）。
/// - 异步 I/O 操作立即成功返回，并且与 I/O 完成对象关联的文件句柄具有通知模式 FILE_SKIP_COMPLETION_PORT_ON_SUCCESS。
/// 文件句柄不会通知 I/O 完成端口，也不会调用相关的 I/O 回调函数。
/// </remarks>
/// <param name="io">指向io完成结构的指针，它应该是调用CreateThreadpoolIo的返回值</param>
inline void cancel_threadpool_io(PTP_IO io)
{
    CancelThreadpoolIo(io);
}

/// <summary>
/// 等待未完成的 I/O 完成回调完成，并可选择取消尚未开始执行的挂起回调。
/// </summary>
/// <remarks>
/// 当fCancelPendingCallbacks设置为 TRUE 时，仅取消排队的回调。未取消的 I/O 请求不会被取消。
/// 因此，调用者应该为OVERLAPPED结构调用GetOverlappedResult以检查 I/O 操作是否已完成，然后再释放该结构。
/// 作为替代方法，将fCancelPendingCallbacks设置为 FALSE，并让关联的 I/O 完成回调释放OVERLAPPED结构。
/// 注意不要在 I/O 请求仍然挂起时释放OVERLAPPED结构；使用GetOverlappedResult确定 I/O 操作的状态并等待操作完成。
/// 该CancelIoEx函数可以选择首先用于取消未完成的 I/O 请求，从而可能缩短等待时间。
///
/// 注意，正如上面所说，该函数若为TRUE，只是取消队列回调，不会影响排队的I/O请求，你需要使用CancelIoEx取消I/O请求
/// </remarks>
/// <param name="io">指向io完成结构的指针，它应该是调用CreateThreadpoolIo的返回值</param>
/// <param name="cancle_pending_callbacks">是否取消尚未开始执行的排队回调，若为TRUE，除了正在执行的回调，其他指定工作项回调将被取消</param>
inline void wait_for_threadpool_io_callbacks(PTP_IO io, BOOL cancle_pending_callbacks = false)
{
    WaitForThreadpoolIoCallbacks(io, cancle_pending_callbacks);
}

/// <summary>
/// 当当前回调函数返回时线程池将释放指定关键段
/// </summary>
/// <param name="callback_instance">它是线程池回调函数的instance参数，用于指定当前回调</param>
/// <param name="cs">要释放的关键段</param>
inline void leave_critical_section_when_callback_returns(PTP_CALLBACK_INSTANCE callback_instance, CRITICAL_SECTION& cs)
{
    LeaveCriticalSectionWhenCallbackReturns(callback_instance, &cs);
}

/// <summary>
/// 当当前回调函数返回时线程池将释放指定互斥量(mutex)
/// </summary>
/// <param name="callback_instance">它是线程池回调函数的instance参数，用于指定当前回调</param>
/// <param name="mutex">要释放的互斥量</param>
inline void release_mutex_when_callback_returns(PTP_CALLBACK_INSTANCE callback_instance, HANDLE mutex)
{
    ReleaseMutexWhenCallbackReturns(callback_instance, mutex);
}

/// <summary>
/// 当当前回调函数返回时线程池将释放指定信号量(semaphore)
/// </summary>
/// <param name="callback_instance">它是线程池回调函数的instance参数，用于指定当前回调</param>
/// <param name="semaphore">要释放的信号量</param>
/// <param name="crel">信号量对象计数的递增量</param>
inline void release_semaphore_when_callback_returns(PTP_CALLBACK_INSTANCE callback_instance, HANDLE semaphore, DWORD crel)
{
    ReleaseSemaphoreWhenCallbackReturns(callback_instance, semaphore, crel);
}

/// <summary>
/// 当当前回调函数返回时线程池将触发指定事件
/// </summary>
/// <param name="callback_instance">它是线程池回调函数的instance参数，用于指定当前回调</param>
/// <param name="event">要出发的事件</param>
inline void set_event_when_callback_returns(PTP_CALLBACK_INSTANCE callback_instance, HANDLE event)
{
    SetEventWhenCallbackReturns(callback_instance, event);
}

/// <summary>
/// 当当前回调函数返回时线程池将卸载指定DLL模块
/// </summary>
/// <param name="callback_instance">它是线程池回调函数的instance参数，用于指定当前回调</param>
/// <param name="module_handle">指定要卸载的DLL实例句柄</param>
inline void free_library_when_callback_returns(PTP_CALLBACK_INSTANCE callback_instance, HMODULE module_handle)
{
    FreeLibraryWhenCallbackReturns(callback_instance, module_handle);
}

/// <summary>
/// 分配一个新的线程池来执行回调，创建之后，你需要调用SetThreadpoolThreadMaximum和Minimum指定池中可用的最大和最小线程数
/// </summary>
/// <returns>若成功返回线程池结构体的指针，不要修改此结构的成员，若失败返回NULL</returns>
inline PTP_POOL create_threadpool()
{
    auto val = CreateThreadpool(nullptr);
    GET_ERROR_MSG_OUTPUT();
    return val;
}

/// <summary>
/// 关闭指定线程池，若没有未完成的work, io, timer, wait回调，则立即释放等待对象；否则，将在未完成的回调完成后异步释放
/// </summary>
/// <param name="threadpool">指向线程池结构体的指针，它应该是调用CreateThreadpool的返回值</param>
inline void close_threadpool(PTP_POOL threadpool)
{
    CloseThreadpool(threadpool);
}

/// <summary>
/// 设置指定线程池可以分配给处理回调的最大线程数
/// </summary>
/// <param name="threadpool">指向线程池结构体的指针，它应该是调用CreateThreadpool的返回值</param>
/// <param name="thread_maximum">线程池的线程最大值</param>
inline void set_threadpool_thread_maximum(PTP_POOL threadpool, DWORD thread_maximum)
{
    SetThreadpoolThreadMaximum(threadpool, thread_maximum);
}

/// <summary>
/// 设置指定线程池可以分配给处理回调的最大线程数
/// </summary>
/// <param name="threadpool">指向线程池结构体的指针，它应该是调用CreateThreadpool的返回值</param>
/// <param name="thread_minimum">线程池的线程最小值</param>
/// <returns>操作是否成功</returns>
inline BOOL set_threadpool_thread_minimum(PTP_POOL threadpool, DWORD thread_minimum)
{
    auto val = SetThreadpoolThreadMinimum(threadpool, thread_minimum);
    GET_ERROR_MSG_OUTPUT();
    return val;
}

/// <summary>
/// 初始化回调环境
/// </summary>
/// <param name="pcbe">用户分配的一个TP_CALLBACK_ENVIRON结构，它定义一个回调环境</param>
inline void initialize_threadpool_environment(PTP_CALLBACK_ENVIRON pcbe)
{
    InitializeThreadpoolEnvironment(pcbe);
}

/// <summary>
/// 删除指定的回调环境。当不再需要回调环境来创建新的线程池对象时调用此函数
/// </summary>
/// <param name="pcbe">用户分配的一个TP_CALLBACK_ENVIRON结构，它定义一个回调环境</param>
inline void destroy_threadpool_environment(PTP_CALLBACK_ENVIRON pcbe)
{
    DestroyThreadpoolEnvironment(pcbe);
}

/// <summary>
/// 设置生成回调时要使用的线程池，若回调环境不指定线程池，则使用默认全局线程池(不需要销毁的线程池，与进程绑定)
/// </summary>
/// <param name="pcbe">用户分配的一个TP_CALLBACK_ENVIRON结构，它定义一个回调环境，它应该使用Initialize初始化过了</param>
/// <param name="threadpool">一个TP_POOL结构体的指针，它定义了线程池，它应该是CreateThreadpool返回的指针</param>
inline void set_threadpool_callback_pool(PTP_CALLBACK_ENVIRON pcbe, PTP_POOL threadpool)
{
    SetThreadpoolCallbackPool(pcbe, threadpool);
}

/// <summary>
/// 表示与此回调环境关联的回调可能不会快速返回，线程池可以使用此信息来更好地确定何时应该创建新线程
/// </summary>
/// <param name="pcbe">用户分配的一个TP_CALLBACK_ENVIRON结构，它定义一个回调环境，它应该使用Initialize初始化过了</param>
inline void set_threadpool_callback_runs_long(PTP_CALLBACK_ENVIRON pcbe)
{
    SetThreadpoolCallbackRunsLong(pcbe);
}

/// <summary>
/// 只要有未完成的回调，就确保指定的 DLL 保持加载状态。
/// </summary>
/// <remarks>注意事项看文档</remarks>
/// <param name="pcbe">用户分配的一个TP_CALLBACK_ENVIRON结构，它定义一个回调环境，它应该使用Initialize初始化过了</param>
/// <param name="module_handle">DLL的实例句柄</param>
inline void set_threadpool_callback_library(PTP_CALLBACK_ENVIRON pcbe, PVOID module_handle)
{
    SetThreadpoolCallbackLibrary(pcbe, module_handle);
}

/// <summary>
/// 创建一个清理组，应用程序可以使用它来跟踪一个或多个线程池回调，使用SetThreadpoolCallbackCleanupGroup将清理组与回调环境关联
/// </summary>
/// <remarks>
/// 使用CreateThreadpool*系函数都会向组添加一个成员
/// 使用CloseThreadpool*系函数向组删除一个成员
///
/// 要关闭所有的回调，调用CloseThreadpoolCleanupGroupMembers
/// </remarks>
/// <returns>若成功返回线程池清理组结构体的指针，不要修改此结构的成员，若失败返回NULL</returns>
inline PTP_CLEANUP_GROUP create_threadpool_cleanup_group()
{
    auto val = CreateThreadpoolCleanupGroup();
    GET_ERROR_MSG_OUTPUT();
    return val;
}

/// <summary>
/// 将指定的清理组与指定的回调环境相关联
/// </summary>
/// <param name="pcbe">用户分配的一个TP_CALLBACK_ENVIRON结构，它定义一个回调环境，它应该使用Initialize初始化过了</param>
/// <param name="cleanup_group">一个TP_CLEANUP_GROUP结构体的指针，它定义了清理组，它应该是CreateThreadpoolCleanupGroup返回的值</param>
/// <param name="cleanup_group_cancel_func">一个回调函数，当CloseThreadpoolCleanupGroupMembers的第二个参数为TRUE时，为所有取消的回调调用</param>
inline void set_threadpool_callback_cleanup_group(PTP_CALLBACK_ENVIRON pcbe, PTP_CLEANUP_GROUP cleanup_group,
    PTP_CLEANUP_GROUP_CANCEL_CALLBACK cleanup_group_cancel_func = nullptr)
{
    SetThreadpoolCallbackCleanupGroup(pcbe, cleanup_group, cleanup_group_cancel_func);
}

/// <summary>
/// 释放指定清理组的成员，等待所有回调函数完成，并可选择取消任何未完成的回调函数。
/// </summary>
/// <remarks>
/// 使用了该函数，就不应该再使用CloseThreadpool*系函数单独关闭某个项，因为它们都被关闭了。若只是想简单地等待或取消排队项而不释放他们，使用
/// WaitForThreadpool*系函数。
///
/// 其他事项请看文档。
/// </remarks>
/// <param name="cleanup_group">一个TP_CLEANUP_GROUP结构体的指针，它定义了清理组，它应该是CreateThreadpoolCleanupGroup返回的值</param>
/// <param name="cancel_pending_callbacks">是否取消尚未开始执行的排队回调，若为TRUE，除了正在执行的回调，其他指定工作项回调将被取消</param>
/// <param name="cleanup_param">要传给清理组回调函数的参数，若你没有指定清理组回调，或cancel_pending_callbacks为false，该参数忽略</param>
inline void close_threadpool_cleanup_group_members(PTP_CLEANUP_GROUP cleanup_group, BOOL cancel_pending_callbacks = false, PVOID cleanup_param = nullptr)
{
    CloseThreadpoolCleanupGroupMembers(cleanup_group, cancel_pending_callbacks, cleanup_param);
}

/// <summary>
/// 关闭指定的清理组，注意，调用该函数时，清理组必须没有成员，使用CloseThreadpoolCleanupGroupMembers关闭所有成员
/// </summary>
/// <param name="cleanup_group">一个TP_CLEANUP_GROUP结构体的指针，它定义了清理组，它应该是CreateThreadpoolCleanupGroup返回的值</param>
inline void close_threadpool_cleanup_group(PTP_CLEANUP_GROUP cleanup_group)
{
    CloseThreadpoolCleanupGroup(cleanup_group);
}

/// <summary>
/// 分配线程本地存储 (TLS) 索引。进程的任何线程随后都可以使用此索引来存储和获取线程本地的值，因为每个线程都接收自己的索引槽。
/// </summary>
/// <returns>若成功，返回值是TLS索引，索引的槽被初始化为0，若失败，返回值是TLS_OUT_OF_INDEXES</returns>
inline DWORD tls_alloc()
{
    auto val = TlsAlloc();
    GET_ERROR_MSG_OUTPUT();
    return val;
}

/// <summary>
/// 在调用线程的线程本地存储 (TLS) 槽中存储指定 TLS 索引的值。进程的每个线程对于每个 TLS 索引都有自己的插槽。
/// </summary>
/// <param name="index">由TlsAlloc 函数分配的 TLS 索引</param>
/// <param name="value">要存储在调用线程的 TLS 槽中的数据</param>
/// <returns>操作是否成功</returns>
inline BOOL tls_set_value(DWORD index, LPVOID value)
{
    auto val = TlsSetValue(index, value);
    GET_ERROR_MSG_OUTPUT();
    return val;
}

/// <summary>
/// 获取指定 TLS 索引的调用线程的线程本地存储 (TLS) 槽中的值。进程的每个线程对于每个 TLS 索引都有自己的插槽。
/// </summary>
/// <param name="index">由TlsAlloc函数分配的 TLS 索引</param>
/// <returns>若函数成功，返回调用线程TLS槽中的值，若失败返回0，由于TLS槽中的值也可能是0，所以需要GetLastError来检查是否出错</returns>
inline LPVOID tls_get_value(DWORD index)
{
    auto val = TlsGetValue(index);
    GET_ERROR_MSG_OUTPUT();
    return val;
}

/// <summary>
/// 释放线程本地存储 (TLS) 索引，使其可供重用(实际就是将对应索引位标志从INUSE改为FREE)
/// </summary>
/// <param name="index">由TlsAlloc函数分配的 TLS 索引</param>
/// <returns>操作是否成功</returns>
inline BOOL tls_free(DWORD index)
{
    auto val = TlsFree(index);
    GET_ERROR_MSG_OUTPUT();
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
    template <typename T>
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
            new (&item->data) T(data);
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
            new (&item->data) T(std::move(data));
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

        template <typename t_type>
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
            GET_ERROR_MSG_OUTPUT();
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
        template <typename Func, typename... Args>
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
        template <typename Func, typename... Args>
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
        ~slimrw_lock() { }

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
        template <typename Func, typename... Args>
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
        template <typename Func, typename... Args>
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
        template <typename Func, typename... Args>
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
        template <typename Func, typename... Args>
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
        ~condition_variable() { }

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
            GET_ERROR_MSG_OUTPUT();
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
            GET_ERROR_MSG_OUTPUT();
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
        template <typename Pred, typename... Args>
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
        template <typename Pred, typename... Args>
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
        GET_ERROR_MSG_OUTPUT();
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
    inline DWORD wait_for_multiple_object(DWORD counts, const HANDLE* object_handles, bool wait_all = true, DWORD milliseconds_to_wait = INFINITE, bool alertable = false)
    {
        auto val = WaitForMultipleObjectsEx(counts, object_handles, wait_all, milliseconds_to_wait, alertable);
        GET_ERROR_MSG_OUTPUT();
        return val;
    }

    /// <summary>
    /// 等待指定一个或所有对象处于触发状态，一个I/O完成例程或异步过程调用(APC)在线程队列中，或超时值已过。使用wake_mask可以包含输入事件。创建窗口的线程应该使用该函数等待
    /// </summary>
    /// <param name="counts">内核句柄数组的数量，它不应该超过MAXIMUM_WAIT_OBJECTS，若为0，则该函数仅等待输入事件</param>
    /// <param name="object_handles">对象句柄数组,该数组可以是不同类型对象句柄(支持类型查看文档)。该句柄必须有SYNCHRONIZE访问权限，若句柄在等待时关闭，函数行为未定义</param>
    /// <param name="flags">指定等待类型，它可以是0，或者MWMO_开头的宏的组合</param>
    /// <param name="milliseconds">超时值，可以为0或INFINITE，若为INFINITE，不会因为超时值而返回</param>
    /// <param name="wake_mask">要包含的输入事件类型，它是QS_开头的宏的组合</param>
    /// <returns>返回以WAIT_开头的宏，用于指示调用线程为什么继续执行</returns>
    inline DWORD msg_wait_for_multiple_objects(DWORD counts, const HANDLE* object_handles,
        DWORD flags = MWMO_ALERTABLE | MWMO_INPUTAVAILABLE | MWMO_WAITALL, DWORD milliseconds = INFINITE, DWORD wake_mask = QS_ALLINPUT)
    {
        auto val = MsgWaitForMultipleObjectsEx(counts, object_handles, milliseconds, wake_mask, flags);
        GET_ERROR_MSG_OUTPUT();
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
    inline HANDLE create_event(DWORD flags = 0, DWORD desired_access = EVENT_ALL_ACCESS, const std::tstring& event_name = _T(""), LPSECURITY_ATTRIBUTES event_attributes = nullptr)
    {
        auto val = CreateEventEx(event_attributes, tstring_to_pointer(event_name), flags, desired_access);
        GET_ERROR_MSG_OUTPUT();
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
        GET_ERROR_MSG_OUTPUT();
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
        GET_ERROR_MSG_OUTPUT();
        return val;
    }

    /// <summary>
    /// 打开现有的命名事件对象
    /// </summary>
    /// <param name="event_name">要打开的事件的名称。名称比较区分大小写，此函数可以打开私有命名空间中的对象</param>
    /// <param name="inherit_handle">如果此值为TRUE，则此进程创建的子进程将继承句柄。否则，子进程不会继承这个句柄</param>
    /// <param name="desired_access">返回的句柄具有的访问权限，它是EVENT_开头的宏</param>
    /// <returns>函数成功，则返回值是事件对象的句柄。如果函数失败，则返回值为NULL</returns>
    inline HANDLE open_event(const std::tstring& event_name, bool inherit_handle = false, DWORD desired_access = EVENT_ALL_ACCESS)
    {
        auto val = OpenEvent(desired_access, inherit_handle, event_name.c_str());
        GET_ERROR_MSG_OUTPUT();
        return val;
    }

    /// <summary>
    /// 创建或打开命名或未命名的可等待计时器(waitable_timer)内核对象并返回内核对象的句柄
    /// </summary>
    /// <param name="flags">它可以是0或者CREATE_WAITABLE_TIMER_MANUAL_RESET，后者标识这个计时器必须手动重置，否则是自动重置</param>
    /// <param name="desired_access">返回的句柄具有的访问权限，它是TIMER_开头的宏</param>
    /// <param name="waitable_timer_name">计时器的名字，该名称限制为MAX_PATH个字符，区分大小写，可以为nullptr，即未命名的内核对象</param>
    /// <param name="waitable_timer_attributes">计时器的安全属性</param>
    /// <returns>函数成功返回计时器对象的句柄，若失败返回NULL，若命名的计时器对象在函数调用之前存在,则函数返回现有对象的句柄</returns>
    inline HANDLE create_waitable_timer(DWORD flags = 0, DWORD desired_access = TIMER_ALL_ACCESS,
        const std::tstring& waitable_timer_name = _T(""), LPSECURITY_ATTRIBUTES waitable_timer_attributes = nullptr)
    {
        auto val = CreateWaitableTimerEx(waitable_timer_attributes, tstring_to_pointer(waitable_timer_name), flags, desired_access);
        GET_ERROR_MSG_OUTPUT();
        return val;
    }

    /// <summary>
    /// 激活或设置指定可等待计时器，当due_time到期时，该计时器将被触发，并且设置计时器的线程调用可选的完成例程(如果有的话)
    /// </summary>
    /// <remarks>使用CancelWaitableTimer函数取消计时器,使用SetWaitableTimer重置计时器</remarks>
    /// <param name="waitable_timer">计时器对象的句柄，该句柄必须具有TIMER_MODIFY_STATE访问权限</param>
    /// <param name="due_time">计时器触发时的时间，以百纳秒为单位，正值表示绝对时间，请务必使用基于UTC的绝对时间。负值表示相对时间</param>
    /// <param name="cycle_time">计时器的周期，以毫秒为单位，若为0，则计时器只发送一次信号，若大于0，则计时器是周期性的</param>
    /// <param name="completion_rountine">[opt]可选的完成例程指针，当计时器触发时执行</param>
    /// <param name="arg_to_completion_rountine">[opt]可选的指针参数，用于传给完成例程</param>
    /// <param name="resume">一般为false，若为true，则当系统在挂起的节能模式时，并且计时器触发，则系统从挂起中恢复</param>
    /// <returns>操作是否成功</returns>
    inline bool set_waitable_timer(HANDLE waitable_timer, const LARGE_INTEGER* due_time, LONG cycle_time,
        PTIMERAPCROUTINE completion_rountine = nullptr, LPVOID arg_to_completion_rountine = nullptr, bool resume = false)
    {
        auto val = SetWaitableTimer(waitable_timer, due_time, cycle_time, completion_rountine, arg_to_completion_rountine, resume);
        GET_ERROR_MSG_OUTPUT();
        return val;
    }

    /// <summary>
    /// 将指定可等待计时器设置为未激活状态
    /// </summary>
    /// <param name="waitable_timer">计时器对象的句柄，该句柄必须具有TIMER_MODIFY_STATE访问权限</param>
    /// <returns>操作是否成功</returns>
    inline bool cancel_waitable_timer(HANDLE waitable_timer)
    {
        auto val = CancelWaitableTimer(waitable_timer);
        GET_ERROR_MSG_OUTPUT();
        return val;
    }

    /// <summary>
    /// 打开现有的命名可等待计时器对象
    /// </summary>
    /// <param name="waitable_timer_name">要打开的计时器的名称。名称比较区分大小写，此函数可以打开私有命名空间中的对象</param>
    /// <param name="inherit_handle">如果此值为TRUE，则此进程创建的子进程将继承句柄。否则，子进程不会继承这个句柄</param>
    /// <param name="desired_access">返回的句柄具有的访问权限，它是TIMER_开头的宏</param>
    /// <returns>函数成功，则返回值是计时器对象的句柄。如果函数失败，则返回值为NULL</returns>
    inline HANDLE open_waitable_timer(const std::tstring& waitable_timer_name, bool inherit_handle = false, DWORD desired_access = TIMER_MODIFY_STATE)
    {
        auto val = OpenWaitableTimer(desired_access, inherit_handle, waitable_timer_name.c_str());
        GET_ERROR_MSG_OUTPUT();
        return val;
    }

    /// <summary>
    /// 挂起当前线程，直到调用I/O完成回调函数，异步过程调用 (APC) 排队等待线程或超时间隔已过
    /// </summary>
    /// <param name="milliseconds">挂起的时间，若为0(调用线程上没有挂起的用户 APC)，则表示调用线程放弃当前时间片(系统直接调度其他线程)，若为INFINITE，则直接挂起</param>
    /// <param name="alertable">若为true，则调用I/O完成回调函数，异步过程调用 (APC) 排队等待线程时自动处理所有在队列的APC然后返回</param>
    /// <returns>若指定时间到期，返回0，若是 I/O 完成回调函数返回，返回WAIT_IO_COMPLETION</returns>
    inline DWORD sleep_alertable(DWORD milliseconds = INFINITE, bool alertable = true)
    {
        return SleepEx(milliseconds, alertable);
    }

    /// <summary>
    /// 创建或打开命名或未命名的信号量(semaphore)内核对象并返回内核对象的句柄，该内核对象在当前资源计数为0时不触发，大于0时触发
    /// </summary>
    /// <remarks>当使用等待函数等待该信号量时，若成功等待信号量触发，则信号量当前资源计数减1，可以使用ReleaseSemaphore增加当前资源计数</remarks>
    /// <param name="maxnum_count">信号量对象的最大资源计数，该值必须大于0</param>
    /// <param name="initial_count">信号量对象的初始资源计数，该值必须大于等于0，并且小于等于maxnum_count</param>
    /// <param name="desired_access">返回的句柄具有的访问权限，它是SEMAPHORE_开头的宏</param>
    /// <param name="semaphore_attributes">信号量的安全属性</param>
    /// <param name="semaphore_name">信号量的名字，该名称限制为MAX_PATH个字符，区分大小写，可以为nullptr，即未命名的内核对象</param>
    /// <returns>函数成功返回信号量对象的句柄，若失败返回NULL，若命名的信号量对象在函数调用之前存在,则函数返回现有对象的句柄</returns>
    inline HANDLE create_semaphore(LONG maxnum_count, LONG initial_count = 0, DWORD desired_access = SEMAPHORE_ALL_ACCESS,
        LPSECURITY_ATTRIBUTES semaphore_attributes = nullptr, const std::tstring& semaphore_name = _T(""))
    {
        auto val = CreateSemaphoreEx(semaphore_attributes, initial_count, maxnum_count,
            tstring_to_pointer(semaphore_name), 0, desired_access);
        GET_ERROR_MSG_OUTPUT();
        return val;
    }

    /// <summary>
    /// 打开现有的命名信号量对象
    /// </summary>
    /// <param name="semaphore_name">要打开的信号量的名称。名称比较区分大小写，此函数可以打开私有命名空间中的对象</param>
    /// <param name="inherit_handle">如果此值为TRUE，则此进程创建的子进程将继承句柄。否则，子进程不会继承这个句柄</param>
    /// <param name="desired_access">返回的句柄具有的访问权限，它是SEMAPHORE_开头的宏</param>
    /// <returns>函数成功，则返回值是信号量对象的句柄。如果函数失败，则返回值为NULL</returns>
    inline HANDLE open_semaphore(const std::tstring& semaphore_name, bool inherit_handle = false,
        DWORD desired_access = SEMAPHORE_ALL_ACCESS)
    {
        auto val = OpenSemaphore(desired_access, inherit_handle, semaphore_name.c_str());
        GET_ERROR_MSG_OUTPUT();
        return val;
    }

    /// <summary>
    /// 为指定信号量对象增加指定数量的当前资源计数
    /// </summary>
    /// <param name="semaphore_handle">指定信号量句柄，该句柄必须具有SEMAPHORE_MODIFY_STATE访问权限</param>
    /// <param name="release_count">要为当前资源计数增加的量，该值必须大于0，若指定的数量会导致信号量的计数超过最大计数，则不会更改并返回FALSE</param>
    /// <param name="previous_count">[out,opt]接收信号量的先前计数的变量，若不需要可以为NULL</param>
    /// <returns>操作是否成功</returns>
    inline bool release_semaphore(HANDLE semaphore_handle, LONG release_count, LPLONG previous_count = nullptr)
    {
        auto val = ReleaseSemaphore(semaphore_handle, release_count, previous_count);
        GET_ERROR_MSG_OUTPUT();
        return val;
    }

    /// <summary>
    /// 创建或打开命名或未命名的互斥量(mutex)内核对象并返回内核对象的句柄，互斥量当不被任何线程占有时触发，当被线程占有时未触发
    /// </summary>
    /// <param name="mutex_name">互斥量的名字，该名称限制为MAX_PATH个字符，区分大小写，可以为nullptr，即未命名的内核对象</param>
    /// <param name="flags">可以为0或CREATE_MUTEX_INITIAL_OWNER，前者表示初始没有任何线程占有该互斥量，后者表示调用线程初始占有该互斥量</param>
    /// <param name="desired_access">返回的句柄具有的访问权限，它是MUTEX_开头的宏</param>
    /// <param name="mutex_attributes">互斥量的安全属性</param>
    /// <returns>函数成功返回互斥量对象的句柄，若失败返回NULL，若命名的信号量对象在函数调用之前存在,则函数返回现有对象的句柄</returns>
    inline HANDLE create_mutex(const std::tstring& mutex_name = _T(""), DWORD flags = 0,
        DWORD desired_access = MUTEX_ALL_ACCESS, LPSECURITY_ATTRIBUTES mutex_attributes = nullptr)
    {
        auto val = CreateMutexEx(mutex_attributes, tstring_to_pointer(mutex_name), flags, desired_access);
        GET_ERROR_MSG_OUTPUT();
        return val;
    }

    /// <summary>
    /// 打开现有的命名互斥量对象
    /// </summary>
    /// <param name="mutex_name">要打开的互斥量的名称。名称比较区分大小写，此函数可以打开私有命名空间中的对象</param>
    /// <param name="inherit_handle">如果此值为TRUE，则此进程创建的子进程将继承句柄。否则，子进程不会继承这个句柄</param>
    /// <param name="desired_access">返回的句柄具有的访问权限，它是MUTEX_开头的宏</param>
    /// <returns>函数成功，则返回值是互斥量对象的句柄。如果函数失败，则返回值为NULL</returns>
    inline HANDLE open_mutex(const std::tstring& mutex_name, bool inherit_handle = false,
        DWORD desired_access = MUTEX_ALL_ACCESS)
    {
        auto val = OpenMutex(desired_access, inherit_handle, mutex_name.c_str());
        GET_ERROR_MSG_OUTPUT();
        return val;
    }

    /// <summary>
    /// 调用线程释放对指定互斥量的占有，若你是递归的占有互斥量(多次调用等待函数获取同一个互斥量)，那么你也要调用该函数相同的次数，使得互斥量解除占有
    /// </summary>
    /// <param name="mutex_handle">互斥量句柄</param>
    /// <returns>操作是否成功</returns>
    inline bool release_mutex(HANDLE mutex_handle)
    {
        auto val = ReleaseMutex(mutex_handle);
        GET_ERROR_MSG_OUTPUT();
        return val;
    }

    /// <summary>
    /// 等待指定进程，直到该进程创建第一个窗口的线程中没有待处理的输入为止，一般用于父进程等待子进程初始化完毕
    /// </summary>
    /// <param name="process_handle">指定进程的句柄，若该进程是控制台程序或没有消息队列，该函数立即返回</param>
    /// <param name="milliseconds">超时值，以毫秒为单位，可以为INFINITE</param>
    /// <returns>若返回0，则函数成功，若超时，返回WAIT_TIMEOUT，若出现错误，返回WAIT_FAILED</returns>
    inline DWORD wait_for_input_idle(HANDLE process_handle, DWORD milliseconds = INFINITE)
    {
        auto val = WaitForInputIdle(process_handle, milliseconds);
        GET_ERROR_MSG_OUTPUT();
        return val;
    }

    /// <summary>
    /// 以原子方式触发指定内核对象，然后等待另一个指定内核对象
    /// </summary>
    /// <param name="object_to_signal">要触发的内核对象，该对象可以是信号量、互斥量或事件，并且要具有对应的权限，请看文档</param>
    /// <param name="object_to_wait_on">要等待的对象的句柄，该句柄需要具有SYNCHRONIZE访问权限</param>
    /// <param name="milliseconds">超时值，以毫秒为单位，可以为INFINITE</param>
    /// <param name="alertable">若为true，一个I/O完成例程或异步过程调用(APC)在线程队列中时返回并执行他们，否则不返回，并且不会执行完成例程或 APC 函数</param>
    /// <returns>返回以WAIT_开头的宏，用于指示调用线程为什么继续执行</returns>
    inline DWORD signal_object_and_wait(HANDLE object_to_signal, HANDLE object_to_wait_on, DWORD milliseconds = INFINITE, bool alertable = true)
    {
        auto val = SignalObjectAndWait(object_to_signal, object_to_wait_on, milliseconds, alertable);
        GET_ERROR_MSG_OUTPUT();
        return val;
    }

}; // namespace sync

}; // namespace mw