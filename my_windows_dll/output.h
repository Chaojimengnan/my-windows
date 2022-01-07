#pragma once

#ifdef MY_WINDOWS_DLL_BUILD
#   define MY_API __declspec (dllexport)
#else
#   define MY_API __declspec (dllimport)
#endif


#ifdef __cplusplus
extern "C"
{
#endif

MY_API int add(int lhs, int rhs);



#ifdef __cplusplus
}
#endif