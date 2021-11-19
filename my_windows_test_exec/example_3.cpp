#include "stdafx.h"
#include "example_3.h"


DWORD WINAPI ThreadFunc(PVOID param)
{
	
	std::tcout << _T("这是另一个线程，正在执行中~\n");
	return 0;
}


void example_3()
{
	std::tcout << _T("这是主线程，正在执行~\n");

	auto thread_handle = mw::safe_handle(mw::c_create_thread(ThreadFunc));

	std::tcout << _T("新线程创建完毕~\n");

	// 等待线程返回
	WaitForSingleObject(*thread_handle, INFINITE);

	/*auto hdc = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
	TextOutW(hdc, 0, 0, _T("新线程返回了~\n"), 9);
	DeleteDC(hdc);*/
	std::tcout << _T("新线程返回了~\n");
}