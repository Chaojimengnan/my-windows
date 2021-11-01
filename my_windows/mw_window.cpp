#include "mw_window.h"
#include <stdexcept>
#include <iostream>

namespace mw {

	window_class::window_class(const std::wstring& class_name)
		:window_class(class_name, L"", CS_HREDRAW | CS_VREDRAW,
			0, 0, LoadIcon(NULL,IDI_APPLICATION), LoadCursor(NULL,IDC_ARROW),
			(HBRUSH)GetStockObject(WHITE_BRUSH), (HICON)LoadImage(GetModuleHandle(NULL),
				MAKEINTRESOURCE(5),
				IMAGE_ICON,
				GetSystemMetrics(SM_CXSMICON),
				GetSystemMetrics(SM_CYSMICON),
				LR_DEFAULTCOLOR)) {}

	window_class::window_class(const std::wstring& class_name, UINT style, HICON hIcon, 
		HCURSOR hCursor, HBRUSH hbrBackground, HICON hIconSm) 
		: window_class(class_name, L"", style, 0, 0, hIcon, hCursor, hbrBackground, hIconSm) {}

	window_class::window_class(const std::wstring& class_name, const std::wstring& menu_name
		, UINT style, int cbClsExtra, int cbWndExtra, HICON hIcon, HCURSOR hCursor, 
		HBRUSH hbrBackground, HICON hIconSm): window_instance_count(0), win_class_id(0)
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
		win_class.lpszMenuName = menu_name.c_str();
		win_class.lpszClassName = class_name.c_str();
		win_class.hInstance = hinstance;
		win_class.lpfnWndProc = window_process;

		win_class_id = RegisterClassEx(&win_class);
		if (!win_class_id)
		{
			throw std::runtime_error("Can't register window class!\n");
		}

	}

	window_class::~window_class()
	{
		if (win_class_id)
			UnregisterClass( MAKEINTATOM(win_class_id), GetModuleHandle(NULL));
	}


	window_instance* window_class::create()
	{
		return create(L"my window", CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			NULL, NULL, NULL, WS_OVERLAPPEDWINDOW, 0);
	}

	window_instance* window_class::create(const std::wstring& window_name)
	{
		return create(window_name, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
			NULL, NULL, NULL, WS_OVERLAPPEDWINDOW, 0);
	}

	window_instance* window_class::create(const std::wstring& window_name, int x, int y, int width, int height)
	{
		return create(window_name, x, y, width, height, NULL, NULL, NULL, WS_OVERLAPPEDWINDOW, 0);
	}

	window_instance* window_class::create(const std::wstring& window_name, int x, int y, int width,
		int height, HWND window_parent, HMENU menu, LPVOID lParam, DWORD style, DWORD ex_style)
	{
		auto my_hwnd = CreateWindowEx(ex_style, MAKEINTATOM(win_class_id),
			window_name.c_str(), style, x, y, width, height,
			window_parent, menu, GetModuleHandle(NULL), lParam);
		window_instance* new_instance = new window_instance(my_hwnd);
		add_window_refer_with_handle(my_hwnd, this, new_instance);
		++window_instance_count;
		return new_instance;
	}

	std::unordered_map<HWND, window_refer>& window_class::handle_map()
	{
		static std::unordered_map<HWND, window_refer> in_handle_map;
		return in_handle_map;
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
			--(this_window.second->window_instance_count);
			DestroyWindow(hwnd);
			return 0;
			}
		case WM_DESTROY:
			{
			window_refer this_window = get_window_refer(hwnd);
			remove_window_refer_with_handle(hwnd);
			if (this_window.second->window_instance_count == 0)
				PostQuitMessage(0);
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

	window_instance::~window_instance()
	{
	}

	void window_instance::show_window(int iCmdShow)
	{
		if (!is_vaild())
			throw std::runtime_error("This window has been destroyed!");

		ShowWindow(my_hwnd, iCmdShow);
		UpdateWindow(my_hwnd);
	}

	HWND window_instance::get_handle()
	{
		if (is_vaild())
			return my_hwnd;
		else return NULL;
	}

}//mw