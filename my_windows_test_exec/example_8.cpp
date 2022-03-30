#include "example_8.h"
#include "stdafx.h"

#ifdef _WIN32
#    define SLASH '\\'
#else
#    define SLASH '/'
#endif

constexpr const char* get_file_name(const char* file_path)
{
    char c = '\0';
    int index = 0;
    int last_slash_index = 0;
    do
    {
        c = file_path[index];
        if (c == SLASH)
            last_slash_index = index;
        ++index;
    } while (c != '\0');

    return file_path + last_slash_index + 1;
}

#define HERE(str) "[" << get_file_name(__FILE__) << ":" << __LINE__ << "," << __FUNCTION__ << "] " << str

int kakafunc_temp()
{
    __try
    {
        int a = 0;
        std::cout << HERE("我会被执行\n");
        // __leave直接将控制流跳转到try块末尾
        __leave;
        std::cout << HERE("我不会被执行\n");
    } __finally
    {
        // 是否提前退出?__leave不算提前退出，局部展开或全局展开才算
        std::cout << AbnormalTermination() << "\n";
        std::cout << HERE("我会被执行\n");
        return 15;
        std::cout << HERE("我不会被执行\n");
    }
}

/// <summary>
/// SEH初步，终止处理程序
/// </summary>
void example_8()
{
    std::cout << kakafunc_temp() << "\n";
    std::cout << HERE("我会被执行\n");
}

// EXCEPTION_EXECUTE_HANDLER
static DWORD test1()
{
    DWORD dwTemp = 0;
    __try
    {
        dwTemp = 5 / dwTemp;
        dwTemp += 10;
    } __except (EXCEPTION_EXECUTE_HANDLER)
    {
        MessageBeep(MB_ICONWARNING);
    }
    // 异常处理完毕后从这里开始执行
    return dwTemp;
}

/// <summary>
/// 异常处理和异常过滤
/// </summary>
void example_8_1()
{
    //std::cout << test1() << "\n";
    int a = 0;
    __try
    {
        *(int*)nullptr = 15;
        //a = 5 / a;
    } __except (EXCEPTION_EXECUTE_HANDLER)
    {
        std::cout << "出现异常!\n";
    }

    std::cout << "hehe\n";
}

// 会产生异常
static DWORD test2()
{
    DWORD dwTemp = 0;
    __try
    {
        dwTemp = 5 / dwTemp;
        dwTemp += 10;
    } __finally
    {
        std::cout << "停止全局展开\n";
        return 233; // 由于这条语句，test3的except块不会被执行，而是直接从test2();下一条语句开始
    }
    // 异常处理完毕后从这里开始执行
    return dwTemp;
}

// 处理异常
static void test3()
{
    __try
    {
        test2();
        // 是否停止全局展开
        std::cout << "还能继续吗?\n";
    } __except (EXCEPTION_EXECUTE_HANDLER)
    {
        MessageBeep(MB_ICONWARNING);
    }
    // 异常处理完毕后从这里开始执行
    std::cout << "OK\n";
    return;
}

/// <summary>
/// 全局展开，与停止全局展开
/// </summary>
void example_8_2()
{
    test3();
}

int lomekragow(int*& a)
{
    a = new int();
    return EXCEPTION_CONTINUE_EXECUTION;
}

/// <summary>
/// EXCEPTION_CONTINUE_EXECUTION
/// </summary>
void example_8_3()
{
    // 这么写直接无限循环
    // 直接看源码看不出来，但是看汇编就懂了
    // 注意EXCEPTION_CONTINUE_EXECUTION是从产生异常的那条指令重新开始执行
    // 所以请谨慎使用这个标志
    int* a = nullptr;
    __try
    {
        *a = 5;
    } __except (lomekragow(a))
    {
    }

    std::cout << *a << "\n";

    delete a;
}

// OVER 2022-01-19 11:36:50

// 测试_chkstk
void example_8_4()
{
    constexpr auto size = 100000;
    int a[size] = { 0 };

    std::cout << a[size - 1] << "\n";
}