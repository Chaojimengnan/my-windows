#pragma once

namespace mw {

	/// <summary>
	/// 载入指定模块到调用进程的地址空间中，该指定模块可能造成其他模块被加载(该模块依赖的模块)。
	/// </summary>
	/// <remarks>
	/// 关于标志的意义请看文档
	/// 
	/// 若指定模块已经在地址空间中，则增加使用计数(若使用默认标志)
	/// </remarks>
	/// <param name="lib_file_name">一个字符串，它指定要加载的模块的文件名，它可以是DLL或EXE.如果函数找不到模块或其依赖项之一，则函数失败</param>
	/// <param name="flags">可以为0，或LOAD_开头的宏之一，或DONT_RESOLVE_DLL_REFERENCES</param>
	/// <returns>若成功，返回加载模块的实例句柄，若失败返回NULL</returns>
	inline HMODULE load_library(const std::tstring& lib_file_name, DWORD flags = 0)
	{
		auto val = LoadLibraryEx(lib_file_name.c_str(), nullptr, flags);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 将目录添加到用于查找应用程序的 DLL 的搜索路径，它会影响随后的LoadLibrary(Ex)调用
	/// </summary>
	/// <remarks>
	/// 调用 SetDllDirectory 后，标准 DLL 搜索路径为：
	///
	/// - 加载应用程序的目录。
	///	- path_name参数指定的目录。
	///	- 系统目录。使用 GetSystemDirectory函数获取该目录的路径。此目录的名称是 System32。
	///	- 16 位系统目录。没有函数获取这个目录的路径，但是被搜索了。此目录的名称是 System.
	///	- Windows 目录。使用 GetWindowsDirectory函数获取该目录的路径。
	///	- PATH 环境变量中列出的目录。
	/// </remarks>
	/// <param name="path_name">要添加到搜索路径的目录。如果此参数为空字符串 ("")，则调用会从默认 DLL 搜索顺序中删除当前目录。如果此参数为 NULL，则函数恢复默认搜索顺序。</param>
	/// <returns>操作是否成功</returns>
	inline BOOL set_dll_directory(const std::tstring& path_name = _T(""))
	{
		auto val = SetDllDirectory(tstring_to_pointer(path_name));
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 获取用于定位应用程序 DLL 的搜索路径中特定于应用程序的部分
	/// </summary>
	/// <param name="dll_directory_str">[out]用于接收应用程序指定的搜索路径的字符串</param>
	/// <returns>操作是否成功</returns>
	inline DWORD get_dll_directory(std::tstring& dll_directory_str)
	{
		auto size = GetDllDirectory(0, nullptr) + 1;
		TCHAR* temp_str = new(std::nothrow) TCHAR[size];
		
		if (!temp_str)
			return 0;

		auto val = GetDllDirectory(size, temp_str);
		temp_str[size - 1] = '\0';
		GET_ERROR_MSG_OUTPUT();

		dll_directory_str = temp_str;
		delete[] temp_str;
		return val;
	}

	/// <summary>
	/// 释放加载的动态链接库 (DLL) 模块，并在必要时减少其引用计数。当引用计数达到零时，模块从调用进程的地址空间中卸载，句柄不再有效
	/// </summary>
	/// <remarks>
	/// 强烈建议看看文档备注
	/// </remarks>
	/// <param name="lib_module">加载的模组实例句柄</param>
	/// <returns>操作是否成功</returns>
	inline BOOL free_library(HMODULE lib_module)
	{
		auto val = FreeLibrary(lib_module);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 将加载的动态链接库 (DLL) 的引用计数减一，然后调用 ExitThread以终止调用线程。该函数不返回
	/// </summary>
	/// <remarks>
	/// 该函数一般用于，当DLL自己创建了一个线程时的情况，该线程执行完代码之后想要释放该DLL并且退出线程
	/// 就必须调用该函数，不要分别调用Free和Exit，因为一旦free了，对DLL的映射就撤销了！此时线程之后再
	/// 执行则会访问违规。
	/// </remarks>
	/// <param name="lib_module">加载的模组实例句柄</param>
	/// <param name="exit_code">调用线程的退出代码</param>
	[[noreturn]] inline void free_library_and_exit_thread(HMODULE lib_module, DWORD exit_code = 0)
	{
		FreeLibraryAndExitThread(lib_module, exit_code);
	}

	/// <summary>
	/// 获取指定模块的模块句柄，该模块必须已经被当前进程加载(该函数在多线程不可靠，用EX版本)
	/// </summary>
	/// <param name="module_name">模块名字(exe或dll)，若为""，则返回主调进程的可执行文件的句柄(基地址)</param>
	/// <returns>返回指定模块的句柄(基地址)</returns>
	inline HMODULE get_module_handle(const std::tstring& module_name = _T(""))
	{
		auto val = GetModuleHandle(tstring_to_pointer(module_name));
		GET_ERROR_MSG_OUTPUT();
		return val;
	}


	/// <summary>
	/// 获取指定模块的模块句柄并增加模块的引用计数，除非指定了 GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT。该模块必须已由调用进程加载
	/// </summary>
	/// <remarks>
	/// 返回的句柄不是全局的或可继承的。它不能被另一个进程复制或使用。
	/// 
	/// 谨慎使用 GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT，该标志行为与非Ex版本相同，多线程下不可靠。
	/// </remarks>
	/// <param name="module_handle">[out]接收指定模块的句柄，若函数失败返回NULL，该函数无法获取带有LOAD_LIBRARY_AS_DATAFILE标志加载的模块</param>
	/// <param name="module_name">加载模块的名称（.dll 或 .exe 文件），或模块中的地址（如果dwFlags是 GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS）若为NULL，返回exe的句柄</param>
	/// <param name="flags">以GET_MODULE_HANDLE_EX_FLAG_开头的宏的一个或多个，注意_PIN和_UNCHANGED_REFCOUNT不能一起使用，后者行为等同于非ex版本</param>
	/// <returns>操作是否成功</returns>
	inline BOOL get_module_handle_ex(HMODULE& module_handle, LPCWSTR module_name = nullptr, DWORD flags = 0)
	{
		auto val = GetModuleHandleEx(flags, module_name, &module_handle);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 获取当前进程的指定模块的文件路径，该函数无法检测LOAD_LIBRARY_AS_DATAFILE标志的模块。若要查找另一个进程加载的模块的文件路径，请用ex版本
	/// </summary>
	/// <param name="module_file_name">[out]接收模块的路径</param>
	/// <param name="module_handle">若为NULL，返回process_handle指定进程的可执行文件路径</param>
	/// <returns>若成功，返回复制到缓冲区的字符串长度，若失败返回0</returns>
	inline DWORD get_module_file_name(std::tstring& module_file_name, HMODULE module_handle = nullptr)
	{
		TCHAR temp_str[MAX_PATH] = { 0 };

		auto val = GetModuleFileName(module_handle, temp_str, MAX_PATH);
		module_file_name = temp_str;
		return val;
	}

	/// <summary>
	/// 获取指定进程的指定模块的文件路径，并不推荐对当前进程调用该函数，应该使用非ex版本，后者更有效也更可靠。该函数无法检测LOAD_LIBRARY_AS_DATAFILE标志的模块
	/// </summary>
	/// <param name="process_handle">包含模块的进程的句柄，句柄必须具有PROCESS_QUERY_INFORMATION或PROCESS_QUERY_LIMITED_INFORMATION访问权限</param>
	/// <param name="module_file_name">[out]接收模块的路径</param>
	/// <param name="module_handle">若为NULL，返回process_handle指定进程的可执行文件路径</param>
	/// <returns>若成功，返回复制到缓冲区的字符串长度，若失败返回0</returns>
	inline DWORD get_module_file_name_ex(HANDLE process_handle, std::tstring& module_file_name, HMODULE module_handle = nullptr)
	{
		TCHAR temp_str[MAX_PATH] = {0};

		auto val = GetModuleFileNameEx(process_handle, module_handle, temp_str, MAX_PATH);
		module_file_name = temp_str;
		return val;
	}

	/// <summary>
	/// 从指定的动态链接库(DLL)获取导出函数或变量的地址
	/// </summary>
	/// <param name="module_handle">包含导出函数或变量的DLL模块句柄，该函数不从LOAD_LIBRARY_AS_DATAFILE标志加载的模块获取地址</param>
	/// <param name="symbol_name">导出函数或变量名，(不要使用序数值，更推荐直接使用名字)</param>
	/// <returns>若成功，返回值是导出的函数或变量的地址，若失败，则返回值为NULL</returns>
	inline FARPROC get_proc_address(HMODULE module_handle, const std::string& symbol_name)
	{
		auto val = GetProcAddress(module_handle, symbol_name.c_str());
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/*template<typename function_t>
	inline auto get_proc_address_t(HMODULE module_handle, const std::string& symbol_name)
	{
		static_assert(std::is_function<function_t>::value, "它必须是一个函数!");
		return reinterpret_cast<function_t*>(get_proc_address(module_handle, symbol_name));
	}*/

};//mw