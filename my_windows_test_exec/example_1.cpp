#include "example_1.h"

BOOL CALLBACK GG(HWND hwnd, LPARAM lParam)
{
	std::string temp;
	mw::user::get_window_text(hwnd, temp);
	std::cout << temp << ", ";
	mw::user::get_window_class_name(hwnd, temp);
	std::cout << temp << "\n";
	return true;
}


constexpr size_t mw_paint_something = WM_USER + 0x150;
constexpr size_t mw_timer_main1 = 10086;
int my_value1 = 5;
std::string my_value2 = "asdojisadjSdaodja啊是第几啊是";
bool my_value3 = true;



BOOL CALLBACK HH(HWND hwnd, LPSTR name, HANDLE data, ULONG_PTR)
{
	std::string ta(name);

	if (ta == "猛男1")
	{
		std::cout << *(int*)data << "\n";
		mw::user::remove_prop(hwnd, name);
	}
	else if (ta == "猛男2")
	{
		std::cout << *(std::string*)data << "\n";
		mw::user::remove_prop(hwnd, name);
	}
	else if (ta == "猛男3")
	{
		std::cout << *(bool*)data << "\n";
		mw::user::remove_prop(hwnd, name);
	}
	else {
		ta.~basic_string();
	}
	std::cout << name << "\n";
	return true;
}


LRESULT CALLBACK JJ(int nCode, WPARAM wParam, LPARAM lParam)
{
	/*std::cout << "nCode:" << nCode << ", wParam:" << wParam << ", lParam:" << lParam << "\n";*/
	auto op = (LPMOUSEHOOKSTRUCT)lParam;
	std::cout << op->pt.x << "," << op->pt.y << "\n";

	return mw::user::call_next_hook(nCode, wParam, lParam);
}

INT_PTR CALLBACK KK(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
	{
		std::cout << "进入对话框WM_INITDIALOG" << lParam << "\n";
	}
	break;
	case WM_CLOSE:
		std::cout << "关闭对话框WM_CLOSE\n";
		//mw::user::end_modal_dialog(hwnd, 1);
		mw::user::destroy_window(hwnd);
		return true;
	default:
		break;
	}
	return false;
}


void example_1()
{
		/*std::cout << mw::get_current_process_id() << "\n";
	std::cout << mw::get_current_thread_id() << "\n";
	std::cout << mw::get_current_process() << "\n";
	std::cout << mw::get_current_thread() << "\n";
	GG(mw::user::get_foreground_window(), 0);
	GG(mw::user::get_top_window(nullptr), 0);*/
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

	/*mw::user::window_dict::set_pre_process_function(
		[](HWND hwnd, UINT msg, WPARAM w, LPARAM l)->LRESULT {
			std::cout << "Fuck you\n";
			return 0;
		}
	);*/


	//mw::user::window_dict::set_default_process_function(mw::user::default_window_procedure);




	mw::user::window_dict::set_default_process_function(
		[](HWND hwnd, UINT msg, WPARAM w, LPARAM l)->LRESULT {
			HGDIOBJ hpen1, hpen2;

			if (msg == WM_CREATE)
			{
				//mw::user::set_timer(hwnd, mw_timer_main1, 3000);
				//mw::user::set_prop(hwnd, "猛男1", &my_value1);
				//mw::user::set_prop(hwnd, "猛男2", &my_value2);
				//mw::user::set_prop(hwnd, "猛男3", &my_value3);
				ShowWindow(hwnd, 1);
				auto hdc = GetDC(hwnd);
				TextOutA(hdc, 400, 300, "哈哈老子进来了", 21);
				hpen1 = GetStockObject(DC_PEN);

				hpen2 = SelectObject(hdc, hpen1);
				SetDCPenColor(hdc, RGB(255, 0, 0));
				Rectangle(hdc, 10, 10, 100, 100);

				ReleaseDC(hwnd, hdc);

			}
			return mw::user::default_window_procedure(hwnd, msg, w, l);
		});

	mw::user::register_window_class(mw::user::window_dict::callback_function, "my_class");
	mw::user::window_dict::dict_value_type my_event;
	my_event[WM_PAINT] = [](LRESULT&, HWND hwnd, UINT, WPARAM, LPARAM)->bool {
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

	my_event[WM_DESTROY] = [](LRESULT&, HWND hwnd, UINT, WPARAM, LPARAM)->bool {
		/*mw::user::enum_props(hwnd, HH);

		mw::user::kill_timer(hwnd, mw_timer_main1);*/
		mw::user::post_quit_message(0);
		return true;
	};

	my_event[mw_paint_something] = [](LRESULT&, HWND hwnd, UINT, WPARAM, LPARAM)->bool {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		MoveToEx(hdc, 100, 80, nullptr);
		LineTo(hdc, 90, 120);
		LineTo(hdc, 120, 90);
		LineTo(hdc, 80, 90);
		LineTo(hdc, 110, 120);
		LineTo(hdc, 100, 280);

		EndPaint(hwnd, &ps);
		return true;
	};

	my_event[WM_TIMER] = [](LRESULT&, HWND hwnd, UINT, WPARAM, LPARAM)->bool {
		//std::cout << "草拟哥\n";
		return true;
	};

	//auto hook_handle = mw::user::set_windows_hook(WH_MOUSE, JJ, nullptr, mw::get_current_thread_id());

	mw::user::window_instance dada(mw::user::create_window("my_class"));
	mw::user::window_dict::add_item_to_dict(dada.get_handle(), my_event);


	//mw::user::create_modal_dialog(IDD_DIALOG1, KK, dada.get_handle());
	auto dialog_handle = mw::user::create_modeless_dialog(IDD_DIALOG1, KK, dada.get_handle());
	mw::user::show_window(dialog_handle);


	dada.to(mw::user::show_window, 1);


	//mw::user::send_message(dada.get_handle(), mw_paint_something, 0, 0);

	MSG msg;
	while (mw::user::get_message(msg))
	{
		if (!mw::user::is_dialog_message(dialog_handle, msg))
		{
			mw::user::translate_key_to_character(msg);
			mw::user::dispatch_message(msg);
		}
	}
	//mw::user::remove_windows_hook(hook_handle);
}