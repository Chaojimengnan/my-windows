#include "my_windows/my_windows.h"
#include "output.h"
#include <iostream>
#include <vector>

//BOOL WINAPI DllMain(
//    HINSTANCE dll_handle,  // handle to DLL module
//    DWORD reason,          // reason for calling function
//    LPVOID reserved)      // reserved
//{
//    // Perform actions based on the reason for calling.
//    switch (reason)
//    {
//    case DLL_PROCESS_ATTACH:
//        // Initialize once for each new process.
//        // Return FALSE to fail DLL load.
//        std::cout << "爷被ID为" << mw::get_current_process_id() << "的弱智进程加载了\n";
//        break;
//
//    case DLL_THREAD_ATTACH:
//        // Do thread-specific initialization.
//        std::cout << "爷被ID为" << mw::get_current_thread_id() << "的弱智线程加载了\n";
//        break;
//
//    case DLL_THREAD_DETACH:
//        // Do thread-specific cleanup.
//        std::cout << "爷被ID为" << mw::get_current_thread_id() << "的弱智线程卸载了\n";
//        break;
//
//    case DLL_PROCESS_DETACH:
//        // Perform any necessary cleanup.
//        std::cout << "爷被ID为" << mw::get_current_process_id() << "的弱智进程卸载了\n";
//        break;
//    }
//    return TRUE;  // Successful DLL_PROCESS_ATTACH.
//}

int
    WINAPI
    message_box_hack(
        _In_opt_ HWND hWnd,
        _In_opt_ LPCWSTR lpText,
        _In_opt_ LPCWSTR lpCaption,
        _In_ UINT uType)
{
    // 拦截API，改成我们想要的内容
    return mw::user::message_box(_T("asdsadsadasdsadsada"), _T("adsadasdsadsada"));
}

// API拦截实战2，使用DLL拦截，对应代码在DLL中
//BOOL WINAPI DllMain(
//    HINSTANCE dll_handle,  // handle to DLL module
//    DWORD reason,          // reason for calling function
//    LPVOID reserved)      // reserved
//{
//    static auto messagebox_raw = (decltype(MessageBoxW)*)mw::get_proc_address(mw::get_module_handle(_T("USER32.dll")), "MessageBoxExW");
//
//    switch (reason)
//    {
//    case DLL_PROCESS_ATTACH:
//        // Initialize once for each new process.
//        // Return FALSE to fail DLL load.
//        mw::hook_all_modules_func("USER32.dll", (PROC)messagebox_raw, (PROC)message_box_hack, dll_handle);
//        break;
//
//    case DLL_THREAD_ATTACH:
//        // Do thread-specific initialization.
//        break;
//
//    case DLL_THREAD_DETACH:
//        // Do thread-specific cleanup.
//        break;
//
//    case DLL_PROCESS_DETACH:
//        // Perform any necessary cleanup.
//        mw::hook_all_modules_func("USER32.dll", (PROC)message_box_hack, (PROC)messagebox_raw, dll_handle);
//        break;
//    }
//    return TRUE;  // Successful DLL_PROCESS_ATTACH.
//}

#include "main.h"
#include <sstream>

LRESULT CALLBACK hook_procedure(int nCode, WPARAM wParam, LPARAM lParam)
{
    return mw::user::call_next_hook(nCode, wParam, lParam);
}

// 使用挂钩注入DLL(FDM就是你了！)
BOOL WINAPI DllMain(
    HINSTANCE dll_handle, // handle to DLL module
    DWORD reason,         // reason for calling function
    LPVOID reserved)      // reserved
{
    // Perform actions based on the reason for calling.
    std::tostringstream o;
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        o << _T("所在进程ID:") << mw::get_current_process_id();
        mw::user::message_box(_T("爷进来了！"), o.str(), MB_OK);
        // Initialize once for each new process.
        // Return FALSE to fail DLL load.
        //code here
        break;

    case DLL_THREAD_ATTACH:
        // Do thread-specific initialization.
        break;

    case DLL_THREAD_DETACH:
        // Do thread-specific cleanup.
        break;

    case DLL_PROCESS_DETACH:
        // Perform any necessary cleanup.
        break;
    }
    return TRUE; // Successful DLL_PROCESS_ATTACH.
}
