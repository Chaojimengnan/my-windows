#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <shellapi.h>
#include <tchar.h>

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#ifdef MY_WINDOWS_PRINT_ERROR

#    define LOERR_ENABLE_TRANSFER
#    define LOERR_ENABLE_PREDEFINE
#    define LOERR_ENABLE_PREDEFINE_WINDOWS
#    define LOERR_ENABLE_PREDEFINE_WINDOWS_SOCKET

#include <loerr/loerr.h>

#    ifndef GET_ERROR_MSG_OUTPUT()
#        define GET_ERROR_MSG_OUTPUT() LOERR_WINDOWS_OUTPUT_IF_ERROR()
#    endif
#    ifndef GET_ERROR_MSG_OUTPUT_NORMAL(value, error_value)
#        define GET_ERROR_MSG_OUTPUT_NORMAL(value, error_value) LOERR_OUTPUT_IF_VALUE_EQUAL(value, error_value)
#    endif
#    ifndef GET_ERROR_MSG_OUTPUT_SOCKET()
#        define GET_ERROR_MSG_OUTPUT_SOCKET() LOERR_WINDOWS_SOCKET_OUTPUT_IF_ERROR();
#    endif

#endif


#ifdef UNICODE
#    define tstring wstring
#    define tcout wcout
#    define tcin wcin
#    define tostringstream wostringstream
#    define tistringstream wistringstream
#else
#    define tstring string
#    define tcout cout
#    define tcin cin
#    define tostringstream ostringstream
#    define tistringstream istringstream
#endif // UNICODE

/// 最大文本长度宏，用于与WIN32API字符串的交互
#define MW_MAX_TEXT 512

namespace mw {

/// <summary>
/// 将错误代码转换成对应的文本提示
/// </summary>
/// <param name="error_code">错误代码</param>
/// <returns>返回错误代码对应的文本提示信息</returns>
inline std::tstring formate_error_code(DWORD error_code)
{
    PTSTR error_text = nullptr;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        error_code,
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        (LPTSTR)&error_text,
        0, nullptr);

    std::tstring error_str(error_text);
    LocalFree(error_text);
    return error_str;
}

/// <summary>
/// 返回指向该字符串缓冲区的const指针，若为""，则返回NULL
/// </summary>
/// <param name="str">源字符串</param>
/// <returns>指向字符串缓冲区的const指针</returns>
constexpr inline PCTSTR tstring_to_pointer(const std::tstring& str)
{
    return ((str == _T("")) ? nullptr : str.c_str());
}

/// <summary>
/// 从tstring中复制一个使用智能指针包装的原生的缓冲区
/// </summary>
/// <param name="tstr">指定tstring</param>
/// <returns>原生缓冲区</returns>
inline std::shared_ptr<TCHAR[]> get_buffer_from_tstring(const std::tstring& tstr)
{
    std::shared_ptr<TCHAR[]> buffer(new TCHAR[tstr.size() + 1]);
    _tcscpy_s(buffer.get(), tstr.size() + 1, tstr.c_str());
    return buffer;
}

} // namespace mw