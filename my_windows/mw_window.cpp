#include "mw_window.h"
#include "mw_process.h"
#include <stdexcept>
#include <iostream>

namespace mw {
namespace user {


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
		static auto check_item = [](HWND hwnd ,UINT message, 
			std::function<bool(HWND, UINT, WPARAM, LPARAM, LRESULT&)>& fun) -> bool 
		{
			// 检查句柄字典中有没有这个句柄项
			auto event_function_dict_iter = get_handle_dict().find(hwnd);
			if (event_function_dict_iter == get_handle_dict().end()) return false;

			// 检查事件消息函数字典中有没有这个信息项
			auto event_function_iter = event_function_dict_iter->second.find(message);
			if (event_function_iter == event_function_dict_iter->second.end()) return false;

			// 找到了就返回true
			fun = event_function_iter->second;
			return true;
		};

		std::function<bool(HWND, UINT, WPARAM, LPARAM, LRESULT&)> fun;
		LRESULT return_code = 0;

		// 寻找特定消息对应的函数
		if (check_item(hwnd, message, fun))
			if (fun(hwnd, message, wParam, lParam, return_code))
				goto to_re;

		// 寻找是否存在默认处理函数
		if (check_item(hwnd, DEFALT_PROCESS_FUNCTION, fun))
			if (fun(hwnd, message, wParam, lParam, return_code))
				goto to_re;

		// 否则使用默认窗口过程函数
		return_code = DefWindowProcA(hwnd, message, wParam, lParam);

to_re:	if (message == WM_DESTROY) remove_item_handle_dict(hwnd);
		return return_code;
	}

	window_class::window_class(const event_function_dict_type& event_function_dict, 
		const std::string& class_name, HICON hIcon, HCURSOR hCursor,
		HBRUSH hbrBackground, HICON hIconSm, UINT style, const std::string& menu_name,
		int cbClsExtra, int cbWndExtra )
		: win_class_id(0), event_function_dict(event_function_dict)
	{
		auto hinstance = get_module_handle();
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
		GET_ERROR_MSG_OUTPUT(std::cout);
		if (win_class_id == 0) vaild = false;
	}

	HWND window_class::create()
	{
		return create("my window", CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			nullptr, nullptr, nullptr, WS_OVERLAPPEDWINDOW, 0);
	}

	HWND window_class::create(const std::string& window_name)
	{
		return create(window_name, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			nullptr, nullptr, nullptr, WS_OVERLAPPEDWINDOW, 0);
	}

	HWND window_class::create(const std::string& window_name, int width, int height, int x, int y)
	{
		return create(window_name, x, y, width, height, nullptr, nullptr, nullptr, WS_OVERLAPPEDWINDOW, 0);
	}

	HWND window_class::create(const std::string& window_name, int x, int y, int width, int height, HWND window_parent, HMENU menu, LPVOID lParam, DWORD style, DWORD ex_style)
	{
		auto my_hwnd = CreateWindowExA(ex_style, (LPCSTR)((ULONG_PTR)((WORD)(win_class_id))),
			window_name.c_str(), style, x, y, width, height,
			window_parent, menu, get_module_handle(), lParam);
		GET_ERROR_MSG_OUTPUT(std::cout);
		if (!add_item_handle_dict(my_hwnd, event_function_dict))
			throw std::runtime_error("这不可能啊！");
		return my_hwnd;
	}

	bool window_class::unregister_class()
	{
		auto val = UnregisterClassA((LPCSTR)((ULONG_PTR)((WORD)(win_class_id))), get_module_handle());
		GET_ERROR_MSG_OUTPUT(std::cout);
		return val;
	}



	bool window_instance::show_window(int iCmdShow)
	{
		auto is_ok1 = ShowWindow(window_handle, iCmdShow);
		GET_ERROR_MSG_OUTPUT(std::cout);
		auto is_ok2 = UpdateWindow(window_handle);
		GET_ERROR_MSG_OUTPUT(std::cout);
		return (is_ok1 && is_ok2);
	}

};//window

};//mw