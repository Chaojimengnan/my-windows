#include "stdafx.h"
#include "example_7.h"
#include "my_windows_dll/output.h"


// DLL初次尝试，显式链接，隐式链接非常简单就不演示了
void example_7()
{
	// 加载DLL
	auto module_handle = mw::load_library(_T("my_windows_dll.dll"));
	HMODULE module_handle2 = nullptr;

	// 获取句柄并增加引用计数
	mw::get_module_handle_ex(module_handle2, (LPCWSTR)module_handle, GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS);

	std::cout << module_handle << ",   " << module_handle2 << "\n";

	std::tstring str1, str2;

	mw::get_module_file_name(str1, module_handle);
	mw::get_module_file_name(str2, module_handle);
	std::tcout << str1 << ",   " << str2 << "\n";

	// 获取DLL的导出函数add的地址，并将它转换成指向add函数的指针
	auto mymy = (decltype(add)*)mw::get_proc_address(module_handle, "add");

	//std::cout << add(1, 2) << "\n";
	std::cout << "add(1,2):" << mymy(1, 2) << "\n";
	std::cout << "Function add address:" << mymy << "\n";
	
	// 撤销DLL映射，因为引用计数为2，所以要释放两次
	mw::free_library(module_handle);
	mw::free_library(module_handle);
	
	if (mw::get_module_handle(_T("my_windows_dll.dll")) == nullptr)
	{
		std::cout << "DLL已经被撤销\n";
	}
	else {
		std::cout << "DLL并没有被撤销\n";
	}

}



DWORD WINAPI new_thread(PVOID param)
{
	std::cout << "新线程已经创建!\n";
	return 0;
}



// 测试DllMain
void example_7_1()
{
	auto module_handle = mw::load_library(_T("my_windows_dll_x64_Debug.dll"));


	auto mymy = (decltype(add)*)mw::get_proc_address(module_handle, "add");

	std::cout << "add(1,2):" << mymy(1, 2) << "\n";
	auto thread_handle = mw::c_create_thread(new_thread);

	mw::sync::wait_for_single_object(thread_handle);

	mw::free_library(module_handle);

	CloseHandle(thread_handle);
}

// 用于延迟链接，可能不需要
//#pragma comment(lib, "Delayimp.lib")


// 隐式链接测试
void example_7_2()
{
	//std::cout << "隐式链接:" << add(1, 2) << "\n";
}

// Toolhelper初步
void example_7_3()
{
	mw::tool_help th;

	th.create_snapshot();

	PROCESSENTRY32 p = {sizeof(p)};

	th.process_first(p);
	do
	{
		std::cout << p.th32ProcessID << "\n\n";
	} while (th.process_next(p));
}

// 测试API拦截程序是否可以拦截动态载入的库API
// 结果：可以，因为内部也拦截了load_library和get_proc_address
void example_7_4()
{
	// 首先要使用一个user32函数，否则挂钩挂不到我们这个进程
	MessageBoxA(nullptr, "qu ni ma de", "fuck you!", MB_OK);
	auto module_handle = mw::load_library(_T("user32.dll"));

	// 第一次
	auto mymy = (decltype(MessageBoxA)*)mw::get_proc_address(module_handle, "MessageBoxA");

	mymy(nullptr, "qu ni ma de1", "fuck you1!", MB_OK);

	mymy(nullptr, "qu ni ma de2", "fuck you2!", MB_OK);
	mymy(nullptr, "qu ni ma de3", "fuck you2!", MB_OK);
	mw::sleep(500);

	// 第二次，如果能拦截到，应该可以显示出来
	mymy(nullptr, "hahhaahahah", "lallaalallalalaalal!", MB_OK);
	mw::sleep(5000);

	mw::free_library(module_handle);
}



// 用于保存MessageBoxExW的真实地址
auto messagebox_raw = (decltype(MessageBoxW)*)(nullptr);

// 我们的拦截函数
int
WINAPI
message_box_hack(
	_In_opt_ HWND hWnd,
	_In_opt_ LPCWSTR lpText,
	_In_opt_ LPCWSTR lpCaption,
	_In_ UINT uType)
{
	// 拦截API，改成我们想要的内容
	return messagebox_raw(nullptr, _T("你说你妈呢"), _T("????"), MB_OK);
}

// API拦截实战
void example_7_5()
{
	// 获取MessageBox的真实地址，一定要保存这个真实地址，因为一会还要复原被修改的导入表
	messagebox_raw = (decltype(MessageBoxW)*)mw::get_proc_address(mw::get_module_handle(_T("USER32.dll")), "MessageBoxExW");

	// 修改我们的EXE模块的导入表，将MessageBox的地址替换为我们的函数的地址，注意，模块的名字要大写
	mw::hook_module_func("USER32.dll", (PROC)messagebox_raw, (PROC)message_box_hack, mw::get_module_handle());

	// 测试，如果拦截成功，它应该说的不是原文
	mw::user::message_box(_T("大家好，我叫F91，今年我9岁了"), _T("老子是猛男"), MB_OK);


	// 再改回去
	mw::hook_module_func("USER32.dll", (PROC)message_box_hack, (PROC)messagebox_raw, mw::get_module_handle());

	// 测试
	mw::user::message_box(_T("大家好，我叫F91，今年我9岁了"), _T("老子是猛男"), MB_OK);

}

// API拦截实战2，使用DLL拦截，对应代码在DLL中
void example_7_6()
{
	// 载入这个DLL，然后它就会拦截除了它以外的其他模块的API
	auto module_handle = mw::load_library(_T("my_windows_dll_x64_Debug.dll"));


	mw::user::message_box(_T("啊，我被拦截了"), _T("RT"));
	//MessageBoxExW(nullptr, _T("啊，我被拦截了"), _T("RT"), MB_OK, 0);


	// 释放它，该DLL会解除拦截
	mw::free_library(module_handle);

	mw::user::message_box(_T("啊，我被拦截了2"), _T("RT2"));

	Sleep(1000);
}

// 使用挂钩注入DLL(FDM就是你了！)
// 注意，这个示例挂钩只是摆设，他不做任何事
// 唯一的目的在于将我们的DLL注入到FDM中，从而使得我们能在FDM的进程空间执行我们的代码
void example_7_7()
{
	auto module_handle = mw::load_library(_T("my_windows_dll_x64_Debug.dll"));

	auto my_hook_procedure = (decltype(hook_procedure)*)mw::get_proc_address(module_handle, "hook_procedure");

	// 最后一个参数应该是FDM的线程ID
	auto hook_handle = mw::user::set_windows_hook(WH_GETMESSAGE, my_hook_procedure, module_handle, 23568);

	mw::free_library(module_handle);

	std::cin.get();

	mw::user::remove_windows_hook(hook_handle);
}


// 使用远程线程来注入DLL(FDM还是你！)
void example_7_8()
{
	// 获取LoadLibraryA的实际地址
	// 每个进程映射的系统DLL地址是相同的，所以这个地址在FDM进程空间也适用
	auto load_library_raw = (decltype(LoadLibraryA)*)mw::get_proc_address(mw::get_module_handle(_T("Kernel32.dll")), "LoadLibraryA");

	// 第一个参数是FDM的进程ID
	// 打开FDM进程
	auto fdm_process_handle = mw::open_process(28868, false, PROCESS_ALL_ACCESS);
	
	SYSTEM_INFO s = {0};
	mw::get_system_info(s);
	auto page_size = s.dwPageSize;

	// 在FDM地址空间分配一块内存，写入我们要注入的DLL路径
	auto base_address = mw::virtual_alloc(fdm_process_handle, page_size);

	// 写入我们要注入的DLL路径
	mw::write_process_memory(fdm_process_handle, base_address, 
		(LPVOID)"D:\\base\\code\\visual_studio\\my-windows\\output\\x64\\Debug\\my_windows_dll_x64_Debug.dll", 84);

	// 在FDM进程空间创建一个远程线程，其起始点是LoadLibraryA，参数是我刚写入的路径的基地址
	mw::create_remote_thread(fdm_process_handle, (LPTHREAD_START_ROUTINE)load_library_raw, base_address);
	
	std::cin.get();

	// 释放我们在FDM进程空间中分配的内存
	mw::virtual_free(fdm_process_handle, base_address);

	// 额外步骤，与上面相同的步骤，创建一个远程线程，但是目标是FreeLibrary
	// 以释放我们注入的DLL
	
}