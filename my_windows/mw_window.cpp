#include "mw_window.h"
#include "mw_utility.h"
#include <stdexcept>
#include <iostream>

namespace mw {

	HWND find_window(const std::string& class_name, const std::string& window_name)
	{
		auto window_handle = FindWindowA(string_to_pointer(class_name), 
			string_to_pointer(window_name));
		GET_ERROR_MSG_OUTPUT(std::cout)
		return window_handle;
	}

	bool get_window_class_name(HWND window_handle, std::string& class_name)
	{
		CHAR temp_str[MW_MAX_TEXT] = {0};
		auto is_ok = RealGetWindowClassA(window_handle, temp_str, MAX_PATH);
		class_name = temp_str;
		return is_ok;
	}

	bool get_window_text(HWND window_handle, std::string& text)
	{
		CHAR temp_str[MW_MAX_TEXT];
		auto is_ok = GetWindowTextA(window_handle, temp_str, MW_MAX_TEXT);
		text = temp_str;
		return is_ok;
	}


	HWND get_desktop_window()
	{
		return GetDesktopWindow();
	}

	window_class::handle_dict_type& window_class::get_handle_dict()
	{
		static handle_dict_type handle_dict;
		return handle_dict;
	}

	bool window_class::add_item_handle_dict(HWND window_handle, const event_function_dict_type& event_function_dict)
	{
		auto return_value = get_handle_dict().insert(std::pair(window_handle, event_function_dict));
		return return_value.second;
	}

	bool window_class::add_item_handle_dict(HWND window_handle, event_function_dict_type&& event_function_dict)
	{
		auto return_value = get_handle_dict().insert(std::pair(window_handle, std::move(event_function_dict)));
		return return_value.second;
	}

	bool window_class::remove_item_handle_dict(HWND window_handle)
	{
		return (get_handle_dict().erase(window_handle) == 1);
	}

	LRESULT window_class::window_process(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		while (true)
		{
			// 检查句柄字典中有没有这个句柄项
			auto event_function_dict_iter = get_handle_dict().find(hwnd);
			if (event_function_dict_iter == get_handle_dict().end()) break;

			// 检查事件消息函数字典中有没有这个信息项
			auto event_function_iter = event_function_dict_iter->second.find(message);
			if (event_function_iter == event_function_dict_iter->second.end()) break;

			// 调用对应的事件函数，若返回值true，使用return_code返回。
			LRESULT return_code = 0;
			if (event_function_iter->second(hwnd, wParam, lParam, return_code))
			{
				if (message == WM_DESTROY) remove_item_handle_dict(hwnd);
				return return_code;
			}
			break;
		}
		// 否则都将调用系统默认处理函数
		if (message == WM_DESTROY) remove_item_handle_dict(hwnd);
		return DefWindowProcA(hwnd, message, wParam, lParam);
	}

	window_class::window_class(const event_function_dict_type& event_function_dict) 
		:window_class("my_class",event_function_dict, "", CS_HREDRAW | CS_VREDRAW,
		0, 0, LoadIcon(NULL, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)GetStockObject(WHITE_BRUSH), NULL) {}

	window_class::window_class(const std::string& class_name, const event_function_dict_type& event_function_dict) 
		: window_class(class_name,  event_function_dict, "", CS_HREDRAW | CS_VREDRAW,
		0, 0, LoadIcon(NULL, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)GetStockObject(WHITE_BRUSH), NULL) {}

	window_class::window_class(const std::string& class_name, const event_function_dict_type& event_function_dict ,UINT style,
		HICON hIcon, HCURSOR hCursor, HBRUSH hbrBackground, HICON hIconSm) 
		: window_class(class_name, event_function_dict, "", style, 0, 0, hIcon, hCursor, hbrBackground, hIconSm) {}

	window_class::window_class(const std::string& class_name, 
		const event_function_dict_type& event_function_dict , const std::string& menu_name, 
		UINT style, int cbClsExtra, int cbWndExtra, HICON hIcon, HCURSOR hCursor, HBRUSH hbrBackground, HICON hIconSm) 
		: win_class_id(0), event_function_dict(event_function_dict)
	{
		auto hinstance = GetModuleHandle(NULL);
		win_class.cbSize = sizeof(WNDCLASSEXA);
		win_class.style = style;
		win_class.cbClsExtra = cbClsExtra;
		win_class.cbWndExtra = cbWndExtra;
		win_class.hIcon = hIcon;
		win_class.hCursor = hCursor;
		win_class.hIconSm = hIconSm;
		win_class.hbrBackground = hbrBackground;
		win_class.lpszMenuName = string_to_pointer(menu_name);
		win_class.lpszClassName = string_to_pointer(class_name);
		win_class.hInstance = hinstance;
		win_class.lpfnWndProc = window_process;

		win_class_id = RegisterClassExA(&win_class);
		GET_ERROR_MSG_OUTPUT(std::cout)
		if (win_class_id == 0) vaild = false;
	}

	HWND window_class::create()
	{
		return create("my window", CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			NULL, NULL, NULL, WS_OVERLAPPEDWINDOW, 0);
	}

	HWND window_class::create(const std::string& window_name)
	{
		return create(window_name, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			NULL, NULL, NULL, WS_OVERLAPPEDWINDOW, 0);
	}

	HWND window_class::create(const std::string& window_name, int x, int y, int width, int height)
	{
		return create(window_name, x, y, width, height, NULL, NULL, NULL, WS_OVERLAPPEDWINDOW, 0);
	}

	HWND window_class::create(const std::string& window_name, int x, int y, int width, int height, HWND window_parent, HMENU menu, LPVOID lParam, DWORD style, DWORD ex_style)
	{
		auto my_hwnd = CreateWindowExA(ex_style, (LPCSTR)((ULONG_PTR)((WORD)(win_class_id))),
		window_name.c_str(), style, x, y, width, height,
		window_parent, menu, GetModuleHandle(NULL), lParam);
		GET_ERROR_MSG_OUTPUT(std::cout)
		if (!add_item_handle_dict(my_hwnd, event_function_dict))
			throw std::runtime_error("这不可能啊！");
		return my_hwnd;
	}

	bool window_class::unregister_class()
	{
		auto is_ok = UnregisterClassA((LPCSTR)((ULONG_PTR)((WORD)(win_class_id))), GetModuleHandle(NULL));
		GET_ERROR_MSG_OUTPUT(std::cout);
		return is_ok;
	}



	bool window_instance::show_window(int iCmdShow)
	{
		auto is_ok1 = ShowWindow(window_handle, iCmdShow);
		GET_ERROR_MSG_OUTPUT(std::cout)
		auto is_ok2 = UpdateWindow(window_handle);
		GET_ERROR_MSG_OUTPUT(std::cout)

		return (is_ok1 && is_ok2);
	}

}//mw