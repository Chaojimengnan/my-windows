#pragma once

#include <Windows.h>
#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include <functional>

/// 获取错误代码,如果是非0值则将对应文本输入到output_stream中
#define GET_ERROR_MSG_OUTPUT(output_stream) {auto my_error_code = GetLastError();\
		if (my_error_code != 0)\
		output_stream << mw::formate_error_code(my_error_code)\
					<< "FILE:" << __FILE__ << "\n"\
					<< "LINE:" << __LINE__ << "\n";}

#ifdef MY_WINDOWS_BUILD
#   define MW_API __declspec (dllexport)
#else
#   define MW_API __declspec (dllimport)
#endif