#pragma once

#ifdef MY_WINDOWS_DLL_BUILD
#    define MY_API __declspec(dllexport)
#else
#    define MY_API __declspec(dllimport)
#endif

#include "my_windows/my_windows.h"

#ifdef __cplusplus
extern "C" {
#endif

MY_API int add(int lhs, int rhs);
MY_API LRESULT CALLBACK hook_procedure(int nCode, WPARAM wParam, LPARAM lParam);

#ifdef __cplusplus
}
#endif