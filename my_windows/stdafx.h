#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <winsock2.h>
#include <shellapi.h>
#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include <functional>


/// 获取错误代码,如果是非0值则将对应文本输入到output_stream中
#define GET_ERROR_MSG_OUTPUT(output_stream) {auto my_error_code = GetLastError();\
		if (my_error_code != 0)\
		output_stream << __FUNCTION__ << "  " << mw::formate_error_code(my_error_code)\
					<< __FILE__ << ":" << __LINE__  << "\n\n"\
					;}

/// 最大文本长度宏，用于与WIN32API字符串的交互
#define MW_MAX_TEXT 512

#ifdef MY_WINDOWS_BUILD
#   define MW_API __declspec (dllexport)
#else
#   define MW_API __declspec (dllimport)
#endif

namespace mw {

	/// <summary>
	/// 将错误代码转换成对应的文本提示		
	/// </summary>
	/// <param name="error_code">错误代码</param>
	/// <returns>返回错误代码对应的文本提示信息</returns>
	inline std::string formate_error_code(DWORD error_code)
	{
		PSTR error_text = nullptr;
		FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			error_code,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(PSTR)&error_text,
			0, nullptr);
		std::string error_str(error_text);
		LocalFree(error_text);
		return error_str;
	}


	/// <summary>
	/// 返回指向该字符串缓冲区的const指针，若为""，则返回NULL
	/// </summary>
	/// <param name="str">源字符串</param>
	/// <returns>指向字符串缓冲区的const指针</returns>
	constexpr inline PCSTR string_to_pointer(const std::string& str)
	{
		return ((str == "") ? nullptr : str.c_str());
	}

	/// <summary>
	/// 返回指向该Unicode字符串缓冲区的const指针，若为""，则返回NULL
	/// </summary>
	/// <param name="str">源字符串</param>
	/// <returns>指向Unicode字符串缓冲区的const指针</returns>
	constexpr inline PCWSTR wstring_to_pointer(const std::wstring& str)
	{
		return ((str == L"") ? nullptr : str.c_str());
	}

}//mw