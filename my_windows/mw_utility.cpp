#include "mw_utility.h"

namespace mw {

	std::wstring string_to_wstring(const std::string& str, UINT string_code_page)
	{
		auto wide_size = MultiByteToWideChar(string_code_page, 0, str.c_str(), -1, nullptr, 0);
		PWSTR temp_wstr = new WCHAR[wide_size];
		MultiByteToWideChar(string_code_page, 0, str.c_str(), -1, temp_wstr, wide_size);
		GET_ERROR_MSG_OUTPUT(std::cout)
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
			-1, temp_str, str_size, &default_char, is_used_def_char);
		GET_ERROR_MSG_OUTPUT(std::cout)
		std::string str(temp_str);

		delete[] temp_str;
		return str;
	}

	bool is_text_unicode(const void* buffer, int byte_count, int* test)
	{
		return IsTextUnicode(buffer, byte_count, test);
	}

	std::shared_ptr<HANDLE> safe_handle(HANDLE kernel_object)
	{
		static auto handle_deleter = [](HANDLE* my_handle) { CloseHandle(*my_handle); delete my_handle; };
		std::shared_ptr<HANDLE> safe_handle_object(new HANDLE(kernel_object), handle_deleter);
		return safe_handle_object;
	}

	std::string formate_error_code(DWORD error_code)
	{
		PSTR error_text = NULL;
		FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			error_code,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(PSTR)&error_text,
			0, NULL);
		std::string error_str(error_text);
		LocalFree(error_text);
		return error_str;
	}

	std::vector<std::string> get_cmd_vec()
	{
		int argc = 0;
		auto argv = CommandLineToArgvW(GetCommandLine(), &argc);
		GET_ERROR_MSG_OUTPUT(std::cout)

		std::vector<std::string> cmd_vec;
		for (size_t i = 0; i < argc; i++)
			cmd_vec.push_back(mw::wstring_to_string(argv[i]));
		HeapFree(GetProcessHeap(), 0, argv);
		return cmd_vec;
	}

	bool get_envionment_var(const std::string& var_name, std::string& var_value)
	{
		auto size_of_char = GetEnvironmentVariableA(var_name.c_str(), NULL, 0);
		auto mybuffer = new char[size_of_char];

		if (GetEnvironmentVariableA(var_name.c_str(), mybuffer, size_of_char))
		{
			var_value = mybuffer;
			delete[] mybuffer;
			return true;
		}
		GET_ERROR_MSG_OUTPUT(std::cout);

		delete[] mybuffer;
		return false;
	}

	bool set_envionment_var(const std::string& var_name, const std::string& var_value)
	{
		auto val = SetEnvironmentVariableA(var_name.c_str(), var_value.c_str());
		GET_ERROR_MSG_OUTPUT(std::cout)
		return val;
	}

	std::string expand_envionment_str(const std::string& src)
	{
		auto size_of_char = ExpandEnvironmentStringsA(src.c_str(), NULL, 0);
		auto mybuffer = new char[size_of_char];
		ExpandEnvironmentStringsA(src.c_str(), mybuffer, size_of_char);
		GET_ERROR_MSG_OUTPUT(std::cout)
		std::string des_str = mybuffer;
		delete[] mybuffer;
		return des_str;
	}

	std::string get_current_work_dir()
	{
		auto size = GetCurrentDirectoryA(0, NULL);
		char* temp_str = new char[size];
		GetCurrentDirectoryA(size, temp_str);
		GET_ERROR_MSG_OUTPUT(std::cout)
		std::string dir_str = temp_str;
		delete[] temp_str;
		return dir_str;
	}

	bool set_current_work_dir(const std::string& work_dir)
	{
		auto is_ok = SetCurrentDirectoryA(work_dir.c_str());
		GET_ERROR_MSG_OUTPUT(std::cout);
		return is_ok;
	}


}//mw