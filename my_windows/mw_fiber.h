#pragma once

namespace mw {

/// <summary>
/// 将当前线程转换为纤程。您必须先将线程转换为纤程，然后才能调度其他纤程。
/// </summary>
/// <param name="param">一个用户定义的值，纤程可以通过GetFiberData获取该值</param>
/// <param name="flags">可以是0或FIBER_FLAG_FLOAT_SWITCH，若是0，则x86系统上无法在纤程进行浮点运算</param>
/// <returns>若成功，则是纤程执行上下文的地址，若失败返回NULL</returns>
inline LPVOID convert_thread_to_fiber(LPVOID param = nullptr, DWORD flags = FIBER_FLAG_FLOAT_SWITCH)
{
    auto val = ConvertThreadToFiberEx(param, flags);
    GET_ERROR_MSG_OUTPUT();
    return val;
}

/// <summary>
/// 将当前纤程转换为线程，该函数释放由ConvertThreadToFiber函数分配的资源 。调用此函数后，您将无法从线程调用任何纤程函数。
/// </summary>
/// <returns>操作是否成功</returns>
inline BOOL convert_fiber_to_thread()
{
    auto val = ConvertFiberToThread();
    GET_ERROR_MSG_OUTPUT();
    return val;
}

/// <summary>
/// 分配一个纤程对象，为其分配一个堆栈，并设置从指定的起始地址开始执行，通常是纤程函数。该函数不调度纤程(使用SwitchToFiber)。
/// </summary>
/// <remarks>
/// 进程可以创建的纤程数量受可用虚拟内存的限制。默认情况下，每条纤程都有1MB的保留堆栈空间。因此，您最多可以创建 2028 个纤程。
/// 如果减小默认堆栈大小，则可以创建更多纤程。但是，如果您使用替代策略来处理请求，您的应用程序将具有更好的性能。
///
/// 线程使用SwitchToFiber函数调度纤程之前 ，它必须调用 ConvertThreadToFiber函数，以便有一个与线程关联的纤程。
/// </remarks>
/// <param name="start_address">纤程函数指针，表示纤程的起始地址，新创建的纤程不会开始执行，直到另一个纤程使用该执行上下文调用 SwitchToFiber函数</param>
/// <param name="param">一个用户定义的值，纤程可以通过GetFiberData获取该值</param>
/// <param name="flags">可以是0或FIBER_FLAG_FLOAT_SWITCH，若是0，则x86系统上无法在纤程进行浮点运算</param>
/// <param name="stack_commit_size">堆栈的初始提交大小，以字节为单位。如果此参数为零，则新纤程使用可执行文件的默认提交堆栈大小(最开始的栈大小)</param>
/// <param name="stack_reserve_size">堆栈的初始保留大小，以字节为单位。如果此参数为零，则新纤程使用可执行文件的默认保留堆栈大小(栈最大的大小)</param>
/// <returns>若成功，则是纤程执行上下文的地址，若失败返回NULL</returns>
inline LPVOID create_fiber(LPFIBER_START_ROUTINE start_address, LPVOID param = nullptr, DWORD flags = FIBER_FLAG_FLOAT_SWITCH,
    SIZE_T stack_commit_size = 0, SIZE_T stack_reserve_size = 0)
{
    auto val = CreateFiberEx(stack_commit_size, stack_reserve_size, flags, start_address, param);
    GET_ERROR_MSG_OUTPUT();
    return val;
}

/// <summary>
/// 调度纤程，该函数必须在纤程上进行调用，若当前是线程，你需要ConvertThreadToFiber将当前线程转换为纤程
/// </summary>
/// <remarks>
/// 避免写如下的代码，会出现无法预测的问题
/// SwitchToFiber( GetCurrentFiber() );
/// </remarks>
/// <param name="fiber">要调度的纤程执行上下文的指针，它是ConvertThreadToFiber(Ex)或CreateFiber(Ex)的返回值</param>
inline void switch_to_fiber(LPVOID fiber)
{
    SwitchToFiber(fiber);
}

/// <summary>
/// 删除一个已存在的纤程，括堆栈、寄存器的子集和纤程数据。若是当前运行的纤程调用该函数，则内部将调用ExitThread终止当前线程
/// </summary>
/// <remarks>
/// 如果当前正在运行的纤程被另一个纤程删除，则运行已删除纤程的线程很可能会异常终止，因为纤程堆栈已被释放。
/// </remarks>
/// <param name="fiber">要删除的纤程执行上下文的指针，它是ConvertThreadToFiber(Ex)或CreateFiber(Ex)的返回值</param>
inline void delete_fiber(LPVOID fiber)
{
    DeleteFiber(fiber);
}

/// <summary>
/// 获得当前正在执行的纤程的执行上下文地址
/// </summary>
/// <returns>获得当前正在执行的纤程的执行上下文地址</returns>
inline LPVOID get_current_fiber()
{
    return GetCurrentFiber();
}

/// <summary>
/// 获取与当前纤程关联的纤程数据，它是ConvertThreadToFiber(Ex)或CreateFiber(Ex)的param参数指定的用户值
/// </summary>
/// <returns>返回与当前纤程关联的纤程数据</returns>
inline LPVOID get_fiber_data()
{
    return GetFiberData();
}

}; // namespace mw