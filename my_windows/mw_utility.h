#pragma once

namespace mw {

	/// <summary>
	/// 将多字节字符串转换为Unicode字符串
	/// </summary>
	/// <param name="str">待转换的多字节字符串</param>
	/// <param name="string_code_page">代码页，默认为UTF-8</param>
	/// <returns>返回对应的Unicode字符串</returns>
	MW_API std::wstring string_to_wstring(const std::string& str, UINT string_code_page = CP_UTF8);

	/// <summary>
	/// 将Unicode字符串转换为多字节字符串，如果要转换的多字节没有对应的字符，则用default_char填充
	/// </summary>
	/// <param name="wstr">待转换的Unicode字符串</param>
	/// <param name="string_code_page">多字节字符串代码页，默认为UTF-8</param>
	/// <param name="default_char">当转换的多字节字符串没有对应编码时填充该字符(如Unicode汉字转换成ANSI)</param>
	/// <param name="is_used_def_char">是否使用了default_char填充</param>
	/// <returns>返回对应的多字节字符串</returns>
	MW_API std::string wstring_to_string(const std::wstring& wstr,
		UINT string_code_page = CP_UTF8, char default_char = '?', PBOOL is_used_def_char = nullptr);

	/// <summary>
	/// 当前缓冲区是否是Unicode字符串(不一定准确)
	/// </summary>
	/// <param name="buffer">待检验缓冲区</param>
	/// <param name="byte_count">待检验的大小，必须小于或等于缓冲区大小</param>
	/// <param name="test">指定的测试，若为NULL，则全部测试</param>
	/// <returns>系统判断是否是unicode</returns>
	inline bool is_text_unicode(const void* buffer, int byte_count, int* test = nullptr)
	{
		return IsTextUnicode(buffer, byte_count, test);
	}

	/// <summary>
	/// 安全句柄包装器，使用智能指针包装，能在销毁时自动关闭句柄
	/// </summary>
	/// <param name="kernel_object">内核对象句柄</param>
	/// <returns>返回被包装后的句柄</returns>
	MW_API std::shared_ptr<HANDLE> safe_handle(HANDLE kernel_object);


	/// <summary>
	/// 获取当前线程所在进程的命令行的vector包装
	/// </summary>
	/// <returns>返回所在进程命令行的vector包装</returns>
	MW_API std::vector<std::string> get_cmd_vec();

	/// <summary>
	/// 获取指定的环境变量的值
	/// </summary>
	/// <param name="var_name">环境变量的名字</param>
	/// <param name="var_value">[out]环境变量的值</param>
	/// <returns>操作是否成功(是否被找到)</returns>
	MW_API bool get_envionment_var(const std::string& var_name, std::string& var_value);
	
	/// <summary>
	/// 设置指定的环境变量的值
	/// </summary>
	/// <remarks>
	/// 当变量名已经存在，并且变量值不为NULL，则修改该变量
	/// 当变量名已经存在，并且变量值为NULL，则删除该变量 
	/// 当变量名不存在，并且变量值不为NULL，则创建该变量
	/// </remarks>
	/// <param name="var_name">环境变量的名字</param>
	/// <param name="var_value">环境变量的值</param>
	/// <returns>操作是否成功</returns>
	inline bool set_envionment_var(const std::string& var_name, const std::string& var_value)
	{
		auto val = SetEnvironmentVariableA(var_name.c_str(), var_value.c_str());
		GET_ERROR_MSG_OUTPUT(std::cout);
		return val;
	}

	/// <summary>
	/// 将指定字符串中的变量使用环境变量替换(如变量$windir$)
	/// </summary>
	/// <param name="src">源字符串</param>
	/// <returns>使用环境变量替换后的字符串</returns>
	MW_API std::string expand_envionment_str(const std::string& src);

	/// <summary>
	/// 获得当前线程所在进程的工作目录
	/// </summary>
	/// <returns>返回所在进程的工作目录</returns>
	MW_API std::string get_current_work_dir();

	/// <summary>
	/// 设置当前线程所在进程的工作目录
	/// </summary>
	/// <param name="work_dir">新工作目录</param>
	/// <returns>操作是否成功</returns>
	inline bool set_current_work_dir(const std::string& work_dir)
	{
		auto val = SetCurrentDirectoryA(work_dir.c_str());
		GET_ERROR_MSG_OUTPUT(std::cout);
		return val;
	}

	

}//mw