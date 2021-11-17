#pragma once
#include "mw_window.h"

namespace mw {
namespace user {

	constexpr UINT DICT_INIT = WM_USER + 0x399;

	/// <summary>
	/// 针对窗口过程的实例化
	/// </summary>
	using window_dict = mw::user::procedure_dict_tools<2,
		std::unordered_map<HWND, std::unordered_map<UINT, std::function<bool(LRESULT&, HWND, UINT, WPARAM, LPARAM)>>>,
		decltype(mw::user::default_window_procedure)>;

	/// <summary>
	/// 显式实例化为窗口过程
	/// </summary>
	template LRESULT window_dict::callback_function(HWND, UINT, WPARAM, LPARAM);

	//template window_dict:: window_dict::get_dict;


	/// <summary>
	/// 针对对话框过程的实例化
	/// </summary>
	using dialog_dict = mw::user::procedure_dict_tools<2,
		std::unordered_map<HWND, std::unordered_map<UINT, std::function<bool(INT_PTR&, HWND, UINT, WPARAM, LPARAM)>>>,
		INT_PTR(HWND, UINT, WPARAM, LPARAM)>;

	/// <summary>
	/// 显式实例化对话框过程
	/// </summary>
	template INT_PTR dialog_dict::callback_function(HWND, UINT, WPARAM, LPARAM);



};//user
};//mw
