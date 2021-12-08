#pragma once

namespace mw {

	/// <summary>
	/// 将多字节字符串转换为Unicode字符串
	/// </summary>
	/// <param name="str">待转换的多字节字符串</param>
	/// <param name="string_code_page">代码页，默认为UTF-8</param>
	/// <returns>返回对应的Unicode字符串</returns>
	inline std::wstring string_to_wstring(const std::string& str, UINT string_code_page = CP_UTF8)
	{
		auto wide_size = MultiByteToWideChar(string_code_page, 0, str.c_str(), -1, nullptr, 0);
		PWSTR temp_wstr = new WCHAR[wide_size];
		MultiByteToWideChar(string_code_page, 0, str.c_str(), -1, temp_wstr, wide_size);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		std::wstring wstr(temp_wstr);

		delete[] temp_wstr;
		return wstr;
	}

	/// <summary>
	/// 将Unicode字符串转换为多字节字符串，如果要转换的多字节没有对应的字符，则用default_char填充
	/// </summary>
	/// <param name="wstr">待转换的Unicode字符串</param>
	/// <param name="string_code_page">多字节字符串代码页，默认为UTF-8</param>
	/// <param name="default_char">当转换的多字节字符串没有对应编码时填充该字符(如Unicode汉字转换成ANSI)</param>
	/// <param name="is_used_def_char">是否使用了default_char填充</param>
	/// <returns>返回对应的多字节字符串</returns>
	inline std::string wstring_to_string(const std::wstring& wstr,
		UINT string_code_page = CP_UTF8, char default_char = '?', PBOOL is_used_def_char = nullptr)
	{
		auto str_size = WideCharToMultiByte(string_code_page, 0,
			wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
		PSTR temp_str = new CHAR[str_size];
		WideCharToMultiByte(string_code_page, 0, wstr.c_str(),
			-1, temp_str, str_size, &default_char, is_used_def_char);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		std::string str(temp_str);

		delete[] temp_str;
		return str;
	}

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
	inline std::shared_ptr<HANDLE> safe_handle(HANDLE kernel_object)
	{
		static auto handle_deleter = [](HANDLE* my_handle) { CloseHandle(*my_handle); delete my_handle; };
		std::shared_ptr<HANDLE> safe_handle_object(new HANDLE(kernel_object), handle_deleter);
		return safe_handle_object;
	}


	/// <summary>
	/// 获取当前线程所在进程的命令行的vector包装
	/// </summary>
	/// <returns>返回所在进程命令行的vector包装</returns>
	inline std::vector<std::tstring> get_cmd_vec()
	{
		int argc = 0;
		auto argv = CommandLineToArgvW(GetCommandLineW(), &argc);
		GET_ERROR_MSG_OUTPUT(std::tcout);

		std::vector<std::tstring> cmd_vec;
		for (size_t i = 0; i < argc; i++)
#ifdef UNICODE
			cmd_vec.push_back(argv[i]);
#else
			cmd_vec.push_back(mw::wstring_to_string(argv[i]));
#endif // UNICODE
		HeapFree(GetProcessHeap(), 0, argv);
		return cmd_vec;
	}

	/// <summary>
	/// 获取指定的环境变量的值
	/// </summary>
	/// <param name="var_name">环境变量的名字</param>
	/// <param name="var_value">[out]环境变量的值</param>
	/// <returns>操作是否成功(是否被找到)</returns>
	inline bool get_envionment_var(const std::tstring& var_name, std::tstring& var_value)
	{
		auto size_of_char = GetEnvironmentVariable(var_name.c_str(), nullptr, 0);
		auto mybuffer = new TCHAR[size_of_char];

		if (GetEnvironmentVariable(var_name.c_str(), mybuffer, size_of_char))
		{
			var_value = mybuffer;
			delete[] mybuffer;
			return true;
		}
		GET_ERROR_MSG_OUTPUT(std::tcout);
		delete[] mybuffer;
		return false;
	}
	
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
	inline bool set_envionment_var(const std::tstring& var_name, const std::tstring& var_value)
	{
		auto val = SetEnvironmentVariable(var_name.c_str(), var_value.c_str());
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 将指定字符串中的变量使用环境变量替换(如变量$windir$)
	/// </summary>
	/// <param name="src">源字符串</param>
	/// <returns>使用环境变量替换后的字符串</returns>
	inline std::tstring expand_envionment_str(const std::tstring& src)
	{
		auto size_of_char = ExpandEnvironmentStrings(src.c_str(), nullptr, 0);
		auto mybuffer = new TCHAR[size_of_char];
		ExpandEnvironmentStrings(src.c_str(), mybuffer, size_of_char);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		std::tstring des_str = mybuffer;
		delete[] mybuffer;
		return des_str;
	}

	/// <summary>
	/// 获得当前线程所在进程的工作目录
	/// </summary>
	/// <returns>返回所在进程的工作目录</returns>
	inline std::tstring get_current_work_dir()
	{
		auto size = GetCurrentDirectory(0, nullptr);
		TCHAR* temp_str = new TCHAR[size];
		GetCurrentDirectory(size, temp_str);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		std::tstring dir_str = temp_str;
		delete[] temp_str;
		return dir_str;
	}

	/// <summary>
	/// 设置当前线程所在进程的工作目录
	/// </summary>
	/// <param name="work_dir">新工作目录</param>
	/// <returns>操作是否成功</returns>
	inline bool set_current_work_dir(const std::tstring& work_dir)
	{
		auto val = SetCurrentDirectory(work_dir.c_str());
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 将FILETIME转换为64位无符号数字(包括32位程序)
	/// </summary>
	/// <param name="ft">FILETIME结构体</param>
	/// <returns>对应的数字</returns>
	inline ULONGLONG file_time_to_qword(FILETIME& ft)
	{
		return (Int64ShllMod32(ft.dwHighDateTime, 32) | ft.dwLowDateTime);
	}

namespace user {

	/// <summary>
	/// 将屏幕坐标(像素)转换为鼠标绝对坐标(0-65535)
	/// </summary>
	/// <param name="screen_x">屏幕坐标X</param>
	/// <param name="screen_y">屏幕坐标Y</param>
	/// <returns>对应的转换pair</returns>
	inline std::pair<LONG, LONG> trans_screen_to_absolute(LONG screen_x, LONG screen_y)
	{
		auto screen_width = GetSystemMetrics(SM_CXSCREEN);
		auto screen_height = GetSystemMetrics(SM_CYSCREEN);

		return std::pair<LONG, LONG>(((float)screen_x / screen_width) * 65535, ((float)screen_y / screen_height) * 65535);
	}

	/// <summary>
	/// 该函数将INPUT结构中的事件依次插入到键盘或鼠标输入流中，该函数受UIPI约束
	/// </summary>
	/// <param name="input_array">一列INPUT结构体的数组</param>
	/// <param name="input_struct_nums">数组的大小(INPUT的数量)</param>
	/// <returns>该返回成功插入键盘或鼠标输入流的事件数，若为0，则输入已经被其他线程阻塞了(若是UIPI阻塞，GetLastError不会显示是UIPI阻塞导致的失败)</returns>
	inline UINT send_input(LPINPUT input_array, UINT input_struct_nums = 1)
	{
		auto val = SendInput(input_struct_nums, input_array, sizeof(INPUT));
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 为指定INPUT结构体写入一个鼠标事件，它应该是一个未初始化的INPUT结构体
	/// </summary>
	/// <param name="input_pointer">指定INPUT指针，它应该是一个未初始化的INPUT结构体</param>
	/// <param name="dx">鼠标的绝对位置，或自上次生成鼠标事件以来的运动量，取决于flags成员的值。绝对位置指定为鼠标的x坐标(0到65535)；相对位置被指定为移动的像素数。</param>
	/// <param name="dy">鼠标的绝对位置，或自上次生成鼠标事件以来的运动量，取决于flags成员的值。绝对位置指定为鼠标的y坐标；相对位置被指定为移动的像素数。</param>
	/// <param name="flags">一个位标志，它是以MOUSEEVENTF_开头的宏的组合，用于指定鼠标事件的具体类型</param>
	/// <param name="mouse_data">与flags有关，若没有在flag中指定滚轮或X按钮，它应该为0</param>
	/// <param name="time">事件的时间戳，以毫秒为单位。如果此参数为 0，系统将提供自己的时间戳</param>
	/// <param name="extra_info">与鼠标事件关联的附加值，可以为0</param>
	/// <returns>操作是否成功</returns>
	inline bool write_mouse_event(LPINPUT input_pointer, LONG dx, LONG dy, DWORD flags, DWORD mouse_data = 0, DWORD time = 0, ULONG_PTR extra_info = 0)
	{
		ZeroMemory(input_pointer, sizeof(INPUT));
		input_pointer->type = INPUT_MOUSE;
		input_pointer->mi.dx = dx;
		input_pointer->mi.dy = dy;
		input_pointer->mi.dwFlags = flags;
		input_pointer->mi.mouseData = mouse_data;
		input_pointer->mi.time = time;
		input_pointer->mi.dwExtraInfo = extra_info;
		return true;
	}

	/// <summary>
	/// 为指定INPUT结构体写入一个键盘事件，它应该是一个未初始化的INPUT结构体
	/// </summary>
	/// <param name="input_pointer">指定INPUT指针，它应该是一个未初始化的INPUT结构体</param>
	/// <param name="virtual_key">一个虚拟键码。代码必须是1到254范围内的值，它可以是VK_开头的宏。如果flags成员指定KEYEVENTF_UNICODE，则virtual_key必须为0</param>
	/// <param name="flags">一个位标志，它是以KEYEVENTF_开头的宏的组合，用于指定键盘事件的具体类型</param>
	/// <param name="scan_code">按键的硬件扫描码。如果flags指定KEYEVENTF_UNICODE，则scan_code指定要发送到前台应用程序的Unicode字符</param>
	/// <param name="time">事件的时间戳，以毫秒为单位。如果此参数为 0，系统将提供自己的时间戳</param>
	/// <param name="extra_info">与键盘事件关联的附加值，可以为0</param>
	/// <returns>操作是否成功</returns>
	inline bool write_keyboard_event(LPINPUT input_pointer, WORD virtual_key, DWORD flags, WORD scan_code, DWORD time = 0, ULONG_PTR extra_info = 0)
	{
		ZeroMemory(input_pointer, sizeof(INPUT));
		input_pointer->type = INPUT_KEYBOARD;
		input_pointer->ki.wVk = virtual_key;
		input_pointer->ki.wScan = scan_code;
		input_pointer->ki.dwFlags = flags;
		input_pointer->ki.time = time;
		input_pointer->ki.dwExtraInfo = extra_info;
		return true;
	}

	/// <summary>
	/// 为指定INPUT结构体写入一个模拟鼠标或键盘事件，它应该是一个未初始化的INPUT结构体
	/// </summary>
	/// <param name="input_pointer">指定INPUT指针，它应该是一个未初始化的INPUT结构体</param>
	/// <param name="uMsg">生成的消息</param>
	/// <param name="wParamL">wParam参数的低位</param>
	/// <param name="wParamH">wParam参数的高位</param>
	/// <returns>操作是否成功</returns>
	inline bool write_hardware_event(LPINPUT input_pointer, DWORD uMsg, WORD wParamL, WORD wParamH)
	{
		ZeroMemory(input_pointer, sizeof(INPUT));
		input_pointer->type = INPUT_HARDWARE;
		input_pointer->hi.uMsg = uMsg;
		input_pointer->hi.wParamL = wParamL;
		input_pointer->hi.wParamH = wParamH;
		return true;
	}

	/// <summary>
	/// 定义系统范围的热键，如果为热键指定的击键已被另一个热键注册将失败
	/// </summary>
	/// <param name="window_handle">指定窗口，它将接收热键生成的WM_HOTKEY消息，若为NULL，则投递到调用线程的消息队列中，并在消息循环中处理</param>
	/// <param name="hotkey_id">热键的标识符，若已经存在相同window_handle和id，则它将与新热键一起维护</param>
	/// <param name="virtual_key">热键的虚拟键码，请看文档</param>
	/// <param name="combination_keys">与热键一起的组合键，它是以MOD_开头的宏的组合，比如shift+A，shift就是组合键</param>
	/// <returns>操作是否成功</returns>
	inline bool register_hotkey(HWND window_handle, int hotkey_id, UINT virtual_key, UINT combination_keys = 0)
	{
		auto val = RegisterHotKey(window_handle, hotkey_id, combination_keys, virtual_key);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 释放先前由调用线程注册的热键
	/// </summary>
	/// <param name="window_handle">与要释放的热键关联的窗口的句柄。如果热键与窗口无关，则此参数应为NULL</param>
	/// <param name="hotkey_id">要释放的热键的标识符</param>
	/// <returns>操作是否成功</returns>
	inline bool unregister_hotkey(HWND window_handle, int hotkey_id)
	{
		auto val = UnregisterHotKey(window_handle, hotkey_id);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

};//user

}//mw