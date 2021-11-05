#include "my_windows.h"
#include <iostream>

#ifdef _WIN32
#include <crtdbg.h>
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif



int main(int argc, char *argv[])
{

	/*auto my_job = mw::make_job();
	my_job->create("猛男在此");
	JOBOBJECT_BASIC_LIMIT_INFORMATION job_limit = { 0 };
	job_limit.PriorityClass = IDLE_PRIORITY_CLASS;

	job_limit.PerJobUserTimeLimit.QuadPart = 100;

	job_limit.LimitFlags = JOB_OBJECT_LIMIT_PRIORITY_CLASS | JOB_OBJECT_LIMIT_JOB_TIME;

	my_job->set_information(JobObjectBasicLimitInformation, &job_limit, sizeof(job_limit));

	mw::process_info toto1, toto2;
	mw::create_process(toto1, "cmd", "", 0, CREATE_SUSPENDED|CREATE_NEW_CONSOLE);
	mw::create_process(toto2, "cmd", "", 0, CREATE_SUSPENDED | CREATE_NEW_CONSOLE);
	my_job->assign_process(*toto1.process_handle);
	my_job->assign_process(*toto2.process_handle);
	ResumeThread(*toto1.thread_handle);
	ResumeThread(*toto2.thread_handle);

	JOBOBJECT_BASIC_LIMIT_INFORMATION job_limit2 = { 0 };

	my_job->query_information(JobObjectBasicLimitInformation, &job_limit2, sizeof(job_limit2));*/


	mw::window_class_new::event_function_dict_type my_event;
	my_event[WM_PAINT] = [](HWND hwnd, WPARAM, LPARAM, LRESULT&)->bool {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		MoveToEx(hdc, 30, 10, NULL);
		LineTo(hdc, 20, 50);
		LineTo(hdc, 50, 20);
		LineTo(hdc, 10, 20);
		LineTo(hdc, 40, 50);
		LineTo(hdc, 30, 10);

		EndPaint(hwnd, &ps);
		return true;
	};

	mw::window_class_new toto("laolao", my_event);
	mw::window_instance_new dada(toto.create());

	dada.show_window();

	
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	return msg.wParam;
}