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
		win_class.lpszMenuName = (menu_name == L"" ? NULL : menu_name.c_str());
		win_class.lpszClassName = (class_name == L"" ? NULL: class_name.c_str());
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


	/*std::vector<std::weak_ptr<window_instance>>& window_class::instance_vec()
	{
		static std::vector<std::weak_ptr<window_instance>> ins_vec;
		return ins_vec;
	}*/

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

}//mw