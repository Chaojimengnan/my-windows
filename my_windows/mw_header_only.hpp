#pragma once
#include "mw_window.h"

namespace mw {
namespace user {

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



};//user
};//mw
