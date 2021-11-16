#include "mw_window.h"
#include "mw_process.h"
#include <stdexcept>
#include <iostream>

namespace mw {
namespace user {


	ATOM register_window_class(WNDPROC procedure, const std::string& class_name, HICON hIcon, 
		HCURSOR hCursor, HBRUSH hbrBackground, HICON hIconSm, UINT style, 
		const std::string& menu_name, int cbClsExtra, int cbWndExtra)
	{
		WNDCLASSEXA win_class;

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
		win_class.lpfnWndProc = procedure;

		auto val = RegisterClassExA(&win_class);
		GET_ERROR_MSG_OUTPUT(std::cout);
		return val;
	}

	HWND create_window(const std::string& window_class_name, const std::string& window_name, int x, int y, int width, int height, HWND window_parent, HMENU menu, LPVOID lParam, DWORD style, DWORD ex_style)
	{
		auto my_hwnd = CreateWindowExA(ex_style, window_class_name.c_str(),
			window_name.c_str(), style, x, y, width, height,
			window_parent, menu, get_module_handle(), lParam);
		GET_ERROR_MSG_OUTPUT(std::cout);
		return my_hwnd;
	}


};//window

};//mw