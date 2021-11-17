#pragma once

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
		GET_ERROR_MSG_OUTPUT(std::cout);
		return val;
	}

	/// <summary>
	/// 结束该调用线程，C++中不要用这个函数，直接线程函数返回，这样才能正确调用析构函数和其他自动清理。其他事项请看文档
	/// </summary>
	/// <param name="exit_code">线程的退出代码</param>
	inline void exit_thread(DWORD exit_code)
	{
		ExitThread(exit_code);
	}

};//mw