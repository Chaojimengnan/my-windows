#pragma once

namespace mw {

	MW_API std::wstring string_to_wstring(const std::string& str, UINT string_code_page = CP_UTF8);

	MW_API std::string wstring_to_string(const std::wstring& wstr,
		UINT string_code_page = CP_UTF8, char default_char = '?', PBOOL is_used_def_char = NULL);

	MW_API bool is_text_unicode(const void* buffer, int byte_count, int* test = NULL);

}//mw