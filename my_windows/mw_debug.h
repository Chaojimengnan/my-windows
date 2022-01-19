#pragma once
#include <DbgHelp.h>
#include "mw_utility.h"

// https://docs.microsoft.com/en-us/windows/win32/debug/debug-help-library

#pragma comment(lib, "Dbghelp.lib")

namespace mw {

	/// <summary>
	/// 对指定映像的指定段进行定位，并返回指定段的信息指针，若失败返回NULL。注意该函数是单线程的，不要在多线程调用它，除非做线程同步
	/// </summary>
	/// <param name="base_address">映像的基地址</param>
	/// <param name="size">[out]接收指定段条目数据大小的变量，以字节为单位</param>
	/// <param name="mapped_as_image">若为TRUE，则系统将文件映射为映像(image)，若为FALSE，则文件被MapViewOfFile函数映射为内存映射文件</param>
	/// <param name="directory_entry">指定段的索引号，它可以是IMAGE_DIRECTORY_ENTRY_宏的其中一个</param>
	/// <returns>若成功，返回值就是指向指定段条目数据的指针，若失败返回NULL</returns>
	inline PVOID image_directory_entry_to_data(PVOID base_address, ULONG& size, BOOLEAN mapped_as_image = true,
		USHORT directory_entry = IMAGE_DIRECTORY_ENTRY_IMPORT)
	{
		auto val = ImageDirectoryEntryToData(base_address, mapped_as_image, directory_entry, &size);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 修改指定模块的导入表，使得对应函数的调用被拦截
	/// </summary>
	/// <remarks>
	/// module_to_hook表示要被修改导入表的模块，如exe，
	/// base_func_module_name和base_func指定了对某个模块导出函数的调用，如USER32.dll的MessageBoxW函数，
	/// hook_func表示新的拦截函数，当exe代码调用user32.dll的MessageBoxW函数时，将变成调用hook_func指定的函数。
	/// </remarks>
	/// <param name="base_func_module_name">导出被拦截函数的模块，比如USER32.dll</param>
	/// <param name="base_func">要被拦截的函数地址，如MessageBoxExW，它应该是使用GetProcAddress获取的真实地址，不要直接把函数传进来，它传入的是导入段的地址！</param>
	/// <param name="hook_func">拦截函数地址，注意该函数的函数签名要与被拦截函数相同，如参数，返回值，调用约定等</param>
	/// <param name="module_to_hook">要被修改导入表的模块，该模块对指定被拦截函数的调用将调用到拦截函数中</param>
	/// <returns>操作是否成功</returns>
	inline bool hook_module_func(const std::string& base_func_module_name,
		PROC base_func, PROC hook_func, HMODULE module_to_hook)
	{
		// 如下，假设我们要修改exe的导入表，使得拦截exe到user32.dll的`MessageBoxW`的调用
		ULONG import_section_size = 0;
		PIMAGE_IMPORT_DESCRIPTOR import_section = nullptr;

		__try {
			// 获取指定模块的导入段信息
			import_section = (PIMAGE_IMPORT_DESCRIPTOR)image_directory_entry_to_data(module_to_hook, import_section_size);
		}
		__except(EXCEPTION_EXECUTE_HANDLER) {
		}

		// 该模块没有导入段或还没有载入
		if (import_section == nullptr) return false;

		for (; import_section->Name; import_section++) 
		{
			// 获取模块的名字，是否是导出被拦截函数的模组(如user32.dll)
			PSTR module_name = (PSTR) ((PBYTE) module_to_hook + import_section->Name);
			if (module_name == base_func_module_name) 
			{
				// 获取导入表的thunk
				PIMAGE_THUNK_DATA thunk = (PIMAGE_THUNK_DATA) 
				((PBYTE) module_to_hook + import_section->FirstThunk);

				// 将当前函数地址替换为我们的拦截函数的地址
				for (; thunk->u1.Function; thunk++) 
				{
					// 获取函数的地址
					PROC* current_func = (PROC*) &thunk->u1.Function;

					// 这是我们要找的函数吗，它应该是当前进程空间中的MessageBoxW的地址
					BOOL is_found = (*current_func == base_func);
					if (is_found)
					{
						// 开始替换地址
						if (!WriteProcessMemory(GetCurrentProcess(), current_func, &hook_func, 
							sizeof(hook_func), NULL) && (ERROR_NOACCESS == GetLastError())) 
						{
							// 如果是页面属性的问题，则修改页面属性然后替换地址，然后再将页面属性复原
							DWORD dwOldProtect;
							if (VirtualProtect(current_func, sizeof(hook_func), PAGE_WRITECOPY, 
								&dwOldProtect)) {

								WriteProcessMemory(GetCurrentProcess(), current_func, &hook_func, 
								sizeof(hook_func), NULL);
								VirtualProtect(current_func, sizeof(hook_func), dwOldProtect, 
								&dwOldProtect);
							}
						}
						return true;  // 找到了，返回
					}
				}
			}  // 解析每个导入段，直到找到一个正确的条目，并且修补它
		}	
		return false;
	}

	/// <summary>
	/// 修改所有模块的导入表，使得对应函数的调用被拦截。可选地，排除调用该函数的代码所在的模块的导入表修改
	/// </summary>
	/// <remarks>
	/// base_func_module_name和base_func指定了对某个模块导出函数的调用，如USER32.dll的MessageBoxW函数，
	/// hook_func表示新的拦截函数，当所有代码调用user32.dll的MessageBoxW函数时，将变成调用hook_func指定的函数。
	/// </remarks>
	/// <param name="base_func_module_name">导出被拦截函数的模块，比如USER32.dll</param>
	/// <param name="base_func">要被拦截的函数地址，如MessageBoxExW，它应该是使用GetProcAddress获取的真实地址，不要直接把函数传进来，它传入的是导入段的地址！</param>
	/// <param name="hook_func">拦截函数地址，注意该函数的函数签名要与被拦截函数相同，如参数，返回值，调用约定等</param>
	/// <param name="exclude_module">可选地排除修改导入段的模块实例地址，若为NULL，则修改所有模块导入表</param>
	/// <returns>操作是否成功</returns>
	inline bool hook_all_modules_func(const std::string& base_func_module_name,
		PROC base_func, PROC hook_func, HMODULE exclude_module = nullptr)
	{
		tool_help th(TH32CS_SNAPMODULE, GetCurrentProcessId());

		bool is_okt = true;

		MODULEENTRY32 me = { sizeof(me) };
		for (BOOL is_ok = th.module_first(me); is_ok; is_ok = th.module_next(me)) 
		{
			if (me.hModule != exclude_module) {
				is_okt = is_okt && hook_module_func(base_func_module_name, base_func, hook_func, me.hModule);
			}
		}
		return is_okt;
	}


	// 以下函数需要在__except块中使用
	// GetExceptionCode 获取异常错误码
	// GetExceptionInformation 获取异常错误详细信息


	// RaiseException抛出SEH异常

}//mw