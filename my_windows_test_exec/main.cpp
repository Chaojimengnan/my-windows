#include "my_windows/my_windows.h"
#include <iostream>
#include <sstream>

#ifdef _WIN32
#include <crtdbg.h>
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif

#include "resource.h"


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
	


	mw::user::window_class::event_function_dict_type my_event;
	my_event[WM_PAINT] = [](HWND hwnd, UINT, WPARAM, LPARAM, LRESULT&)->bool {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		MoveToEx(hdc, 30, 10, nullptr);
		LineTo(hdc, 20, 50);
		LineTo(hdc, 50, 20);
		LineTo(hdc, 10, 20);
		LineTo(hdc, 40, 50);
		LineTo(hdc, 30, 10);

		EndPaint(hwnd, &ps);
		return true;
	};

	my_event[mw::user::window_class::DEFALT_PROCESS_FUNCTION] = 
		[](HWND hwnd, UINT msg, WPARAM w, LPARAM l, LRESULT& errorcode)->bool 
	{
		errorcode = DefWindowProcW(hwnd, msg, w, l);
		return true;
	};


	mw::user::window_class toto(my_event, "my_class", nullptr, 
		(HCURSOR)mw::user::load_internal_image(mw::get_module_handle(), IDC_CURSOR1, IMAGE_CURSOR));
	mw::user::window_instance dada(toto.create());

	dada.show_window();

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	

#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	return msg.wParam;
}