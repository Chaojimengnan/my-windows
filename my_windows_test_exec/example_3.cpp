#include "stdafx.h"
#include "example_3.h"


DWORD WINAPI ThreadFunc(PVOID param)
{
	
	std::tcout << _T("这是另一个线程，正在执行中~\n");
	return 0;
}


void example_3()
{
	/*auto thread_handle = CreateThread(NULL, 0, ThreadFunc, nullptr, CREATE_SUSPENDED, nullptr);
	SetThreadPriority(thread_handle, THREAD_PRIORITY_IDLE);
	ResumeThread(thread_handle);
	CloseHandle(thread_handle);*/

	std::tcout << _T("这是主线程，正在执行~\n");

	auto thread_handle = mw::safe_handle(mw::c_create_thread(ThreadFunc));

	std::tcout << _T("新线程创建完毕~\n");

	// 等待线程返回
	WaitForSingleObject(*thread_handle, INFINITE);

	std::tcout << _T("新线程返回了~\n");
}