#include "mw_utility.h"

namespace mw {

	std::wstring string_to_wstring(const std::string& str, UINT string_code_page)
	{
		auto wide_size = MultiByteToWideChar(string_code_page, 0, str.c_str(), -1, nullptr, 0);
		PWSTR temp_wstr = new WCHAR[wide_size];
		MultiByteToWideChar(string_code_page, 0, str.c_str(), -1, temp_wstr, wide_size);
		std::wstring wstr(temp_wstr);

		delete[] temp_wstr;
		return wstr;
	}

	std::string wstring_to_string(const std::wstring& wstr,
		UINT string_code_page, char default_char, PBOOL is_used_def_char)
	{
		auto str_size = WideCharToMultiByte(string_code_page, 0, 
			wstr.c_str(), -1, nullptr, 0, NULL, NULL );
		PSTR temp_str = new CHAR[str_size];
		WideCharToMultiByte(string_code_page, 0, wstr.c_str(), 
			-1, nullptr, 0, &default_char, is_used_def_char);
		std::string str(temp_str);

		delete[] temp_str;
		return str;
	}

	bool is_text_unicode(const void* buffer, int byte_count, int* test)
	{
		return IsTextUnicode(buffer, byte_count, test);
	}


}//mw