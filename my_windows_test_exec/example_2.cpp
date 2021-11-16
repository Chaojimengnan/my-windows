#include "example_2.h"
#include <sstream>

bool cmd(INT_PTR& return_value, HWND hwnd, UINT msg, WPARAM w, LPARAM l)
{
	auto high_w = HIWORD(w);
	auto low_w = LOWORD(w);

	switch (low_w)
	{
	case IDOK:
		mw::user::set_dialog_item_text(hwnd, IDC_STATIC1, "呵呵,你点击了确定按钮！");
		break;
	case IDCANCEL:
		mw::user::set_dialog_item_text(hwnd, IDC_STATIC1, "妈的，你点击了取消按钮！");
		break;

	default:
		break;
	}

	return false;
}

bool close(INT_PTR& return_value, HWND hwnd, UINT msg, WPARAM w, LPARAM l)
{
	mw::user::dialog_dict::destroy_key_and_remove(hwnd, mw::user::destroy_window, hwnd);
	return return_value = true;
}

bool destroy(INT_PTR& return_value, HWND hwnd, UINT msg, WPARAM w, LPARAM l)
{
	mw::user::post_quit_message(0);
	return return_value = true;
}

bool mouse_move(INT_PTR& return_value, HWND hwnd, UINT msg, WPARAM w, LPARAM l)
{
	std::ostringstream s;
	s << "鼠标的位置为x:" << LOWORD(l) << ", y:" << HIWORD(l);

	mw::user::set_dialog_item_text(hwnd, IDC_STATIC1, s.str());
	return return_value = true;
}


void example_2()
{
	// 设置对话框默认处理函数
	// 由于注册字典在创建窗口之后，所以需要在默认处理函数中处理WM_INITDIALOG消息
	mw::user::dialog_dict::set_default_process_function([](HWND hwnd, UINT msg, WPARAM w, LPARAM l)->INT_PTR {
		if (msg == WM_INITDIALOG)
		{
			return true;
		}

		return false;
	});

	// 创建一个对话框消息字典
	mw::user::dialog_dict::dict_value_type dialog_event_dict;

	// 注册对应的处理函数
	dialog_event_dict[WM_COMMAND] = cmd;
	dialog_event_dict[WM_CLOSE] = close;
	dialog_event_dict[WM_DESTROY] = destroy;
	dialog_event_dict[WM_MOUSEMOVE] = mouse_move;

	// 创建一个非模态对话框并注册进入字典
	auto dialog_handle = mw::user::dialog_dict::create_key_and_add(dialog_event_dict,
		mw::user::create_modeless_dialog, IDD_DIALOG1, mw::user::dialog_dict::callback_function<INT_PTR, HWND, UINT, WPARAM, LPARAM>, nullptr, 0, nullptr);

	mw::user::show_window(dialog_handle);

	auto ok_button = mw::user::get_child_winodw_from_id(dialog_handle, IDOK);
	//mw::user::enable_window(ok_button, false);
	//mw::user::set_window_text(ok_button, "阿瑟东就");
	mw::user::set_dialog_item_text(dialog_handle, IDC_STATIC1, "猛男啊实打实大苏打实打实大苏打");
	mw::user::set_dialog_button_check_state(dialog_handle, IDCANCEL, BST_INDETERMINATE);
	//mw::user::set_foreground_window(dialog_handle);
	//mw::user::bring_window_to_top(dialog_handle);

	MSG msg;
	while (mw::user::get_message(msg))
	{
		if (!mw::user::is_dialog_message(dialog_handle, msg))
		{
			mw::user::translate_key_to_character(msg);
			mw::user::dispatch_message(msg);
		}
	}

}