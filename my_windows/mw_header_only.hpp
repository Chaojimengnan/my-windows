#pragma once
#include "mw_window.h"

namespace mw {
namespace user {

	// 针对窗口的实例化，并导出到DLL
	using window_dict = mw::user::procedure_dict_tools<2,
		std::unordered_map<HWND, std::unordered_map<UINT, std::function<bool(LRESULT&, HWND, UINT, WPARAM, LPARAM)>>>,
		decltype(mw::user::default_window_procedure)>;
	template MW_API LRESULT window_dict::callback_function(HWND, UINT, WPARAM, LPARAM);
	template MW_API auto& window_dict::get_dict();
	template MW_API auto& window_dict::get_my_f();
	template MW_API auto& window_dict::get_my_pref();
	template MW_API void window_dict::set_default_process_function(default_process_function_type f);
	template MW_API void window_dict::set_pre_process_function(default_process_function_type f);
	template MW_API bool window_dict::add_item_to_dict(const dict_key_type& key, const dict_value_type& val);
	template MW_API bool window_dict::remove_item_from_dict(const dict_key_type& key);

	// 针对对话框的实例化，并导出到DLL
	using dialog_dict = mw::user::procedure_dict_tools<2,
		std::unordered_map<HWND, std::unordered_map<UINT, std::function<bool(INT_PTR&, HWND, UINT, WPARAM, LPARAM)>>>,
		INT_PTR(HWND, UINT, WPARAM, LPARAM)>;
	template INT_PTR dialog_dict::callback_function(HWND, UINT, WPARAM, LPARAM);
	template MW_API auto& dialog_dict::get_dict();
	template MW_API auto& dialog_dict::get_my_f();
	template MW_API auto& dialog_dict::get_my_pref();
	template MW_API void dialog_dict::set_default_process_function(default_process_function_type f);
	template MW_API void dialog_dict::set_pre_process_function(default_process_function_type f);
	template MW_API bool dialog_dict::add_item_to_dict(const dict_key_type& key, const dict_value_type& val);
	template MW_API bool dialog_dict::remove_item_from_dict(const dict_key_type& key);



};//user
};//mw
