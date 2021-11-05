#include "mw_window.h"
#include "mw_utility.h"
#include <stdexcept>
#include <iostream>

namespace mw {

	window_class::window_class(const std::wstring& class_name)
		:window_class(class_name, L"", CS_HREDRAW | CS_VREDRAW,
			0, 0, LoadIcon(NULL,IDI_APPLICATION), LoadCursor(NULL,IDC_ARROW),
			(HBRUSH)GetStockObject(BLACK_BRUSH), NULL) {}

	window_class::window_class(const std::wstring& class_name, UINT style, HICON hIcon, 
		HCURSOR hCursor, HBRUSH hbrBackground, HICON hIconSm) 
		: window_class(class_name, L"", style, 0, 0, hIcon, hCursor, hbrBackground, hIconSm) {}

	window_class::window_class(const std::wstring& class_name, const std::wstring& menu_name
		, UINT style, int cbClsExtra, int cbWndExtra, HICON hIcon, HCURSOR hCursor, 
		HBRUSH hbrBackground, HICON hIconSm):  win_class_id(0)
	{
		auto hinstance = GetModuleHandle(NULL);
		// Default setting
		win_class.cbSize = sizeof(WNDCLASSEX);
		win_class.style = style;
		win_class.cbClsExtra = cbClsExtra;
		win_class.cbWndExtra = cbWndExtra;
		win_class.hIcon = hIcon;
		win_class.hCursor = hCursor;
		win_class.hIconSm = hIconSm;
		win_class.hbrBackground = hbrBackground;
		win_class.lpszMenuName = wstring_to_pointer(menu_name);
		win_class.lpszClassName = wstring_to_pointer(class_name);
		win_class.hInstance = hinstance;
		win_class.lpfnWndProc = window_process;

		win_class_id = RegisterClassEx(&win_class);
		GET_ERROR_MSG_OUTPUT(std::cout)
		if (!win_class_id)
		{
			throw std::runtime_error("Can't register window class!\n");
		}

	}

	window_class::~window_class()
	{
		for (auto& i : instance_vec)
			if (auto p = i.lock())
				p->destroy();
		if (win_class_id)
		{
			UnregisterClass(MAKEINTATOM(win_class_id), GetModuleHandle(NULL));
			GET_ERROR_MSG_OUTPUT(std::cout)
		}
	}


	std::shared_ptr<window_instance> window_class::create()
	{
		return create(L"my window", CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			NULL, NULL, NULL, WS_OVERLAPPEDWINDOW, 0);
	}

	std::shared_ptr<window_instance> window_class::create(const std::wstring& window_name)
	{
		return create(window_name, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
			NULL, NULL, NULL, WS_OVERLAPPEDWINDOW, 0);
	}

	std::shared_ptr<window_instance> window_class::create(const std::wstring& window_name, int x, int y, int width, int height)
	{
		return create(window_name, x, y, width, height, NULL, NULL, NULL, WS_OVERLAPPEDWINDOW, 0);
	}

	std::shared_ptr<window_instance> window_class::create(const std::wstring& window_name, int x, int y, int width,
		int height, HWND window_parent, HMENU menu, LPVOID lParam, DWORD style, DWORD ex_style)
	{
		auto my_hwnd = CreateWindowEx(ex_style, MAKEINTATOM(win_class_id),
			window_name.c_str(), style, x, y, width, height,
			window_parent, menu, GetModuleHandle(NULL), lParam);
		GET_ERROR_MSG_OUTPUT(std::cout)
		std::shared_ptr<window_instance> new_instance(new window_instance(my_hwnd));
		add_window_refer_with_handle(my_hwnd, this, new_instance.get());
		instance_vec.push_back(new_instance);
		++window_instance_count();
		return new_instance;
	}

	std::unordered_map<HWND, window_refer>& window_class::handle_map()
	{
		static std::unordered_map<HWND, window_refer> in_handle_map;
		return in_handle_map;
	}



	size_t& window_class::window_instance_count()
	{
		static size_t ins_count = 0;
		return ins_count;
	}

	LRESULT window_class::window_process(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_PAINT:
			{
			window_refer this_window = get_window_refer(hwnd);
			this_window.second->on_paint(hwnd);
			return 0;
			}

		case WM_CLOSE:
			{
			window_refer this_window = get_window_refer(hwnd);
			this_window.second->on_close(hwnd);
			// Tell the instance that its window has been destroyed
			this_window.first->isvaild = false;
			this_window.first->my_hwnd = NULL;
			--(window_instance_count());
			DestroyWindow(hwnd);
			GET_ERROR_MSG_OUTPUT(std::cout)
			return 0;
			}
		case WM_DESTROY:
			{
			window_refer this_window = get_window_refer(hwnd);
			remove_window_refer_with_handle(hwnd);
			if (window_instance_count() == 0)
			{
				PostQuitMessage(0);
				GET_ERROR_MSG_OUTPUT(std::cout)
			}
			return 0;
			}
		}

		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	window_refer window_class::get_window_refer(HWND hwnd)
	{
		return handle_map().at(hwnd);
	}

	bool window_class::add_window_refer_with_handle(HWND hwnd, window_class* win_class, window_instance* win_ins)
	{
		auto return_value = handle_map().insert(std::pair(hwnd, window_refer(win_ins, win_class)));
		return return_value.second;
	}

	bool window_class::remove_window_refer_with_handle(HWND hwnd)
	{
		return (handle_map().erase(hwnd) == 1);
	}

	window_instance::window_instance(HWND window_handle): my_hwnd(window_handle), isvaild(true)
	{
	}

	LRESULT window_instance::destroy()
	{
		if (is_vaild())
		{
			auto result = SendMessageA(my_hwnd, WM_CLOSE, 0, 0);
			GET_ERROR_MSG_OUTPUT(std::cout)
				return result;
		}
		return 0;
	}

	window_instance::~window_instance()
	{
		if (is_vaild())
			destroy();
	}

	void window_instance::show_window(int iCmdShow)
	{
		if (!is_vaild())
			throw std::runtime_error("This window has been destroyed!");

		ShowWindow(my_hwnd, iCmdShow);
		GET_ERROR_MSG_OUTPUT(std::cout)
		UpdateWindow(my_hwnd);
		GET_ERROR_MSG_OUTPUT(std::cout)
	}

	HWND window_instance::get_handle()
	{
		if (is_vaild())
			return my_hwnd;
		else return NULL;
	}

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

	window_class_new::handle_dict_type& window_class_new::get_handle_dict()
	{
		static handle_dict_type handle_dict;
		return handle_dict;
	}

	bool window_class_new::add_item_handle_dict(HWND window_handle, const event_function_dict_type& event_function_dict)
	{
		auto return_value = get_handle_dict().insert(std::pair(window_handle, event_function_dict));
		return return_value.second;
	}

	bool window_class_new::add_item_handle_dict(HWND window_handle, event_function_dict_type&& event_function_dict)
	{
		auto return_value = get_handle_dict().insert(std::pair(window_handle, std::move(event_function_dict)));
		return return_value.second;
	}

	bool window_class_new::remove_item_handle_dict(HWND window_handle)
	{
		return (get_handle_dict().erase(window_handle) == 1);
	}

	LRESULT window_class_new::window_process(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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

	window_class_new::window_class_new() :window_class_new("my_class", "", CS_HREDRAW | CS_VREDRAW,
		0, 0, LoadIcon(NULL, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)GetStockObject(BLACK_BRUSH), NULL) {}

	window_class_new::window_class_new(const std::string& class_name) : window_class_new(class_name, "", CS_HREDRAW | CS_VREDRAW,
		0, 0, LoadIcon(NULL, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)GetStockObject(BLACK_BRUSH), NULL) {}

	window_class_new::window_class_new(const std::string& class_name, UINT style,
		HICON hIcon, HCURSOR hCursor, HBRUSH hbrBackground, HICON hIconSm) 
		: window_class_new(class_name, "", style, 0, 0, hIcon, hCursor, hbrBackground, hIconSm) {}

	window_class_new::window_class_new(const std::string& class_name, const std::string& menu_name, UINT style, int cbClsExtra, int cbWndExtra, HICON hIcon, HCURSOR hCursor, HBRUSH hbrBackground, HICON hIconSm)
	{
	}





}//mw