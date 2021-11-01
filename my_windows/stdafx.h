#pragma once

#include <Windows.h>
#include <memory>
#include <string>
#include <functional>

#ifdef MY_WINDOWS_BUILD
#   define MW_API __declspec (dllexport)
#else
#   define MW_API __declspec (dllimport)
#endif