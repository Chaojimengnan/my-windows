@echo off
mkdir ..\output\my_windows
mkdir ..\output\my_windows\lib
mkdir ..\output\my_windows\include
copy ..\my_windows\*.h ..\output\my_windows\include
copy ..\my_windows\*.cpp ..\output\my_windows\include

copy ..\output\x64\Debug\my_windows_lib_x64_Debug.lib ..\output\my_windows\lib
copy ..\output\x64\Release\my_windows_lib_x64_Release.lib ..\output\my_windows\lib
copy ..\output\x86\Debug\my_windows_lib_x86_Debug.lib ..\output\my_windows\lib
copy ..\output\x86\Release\my_windows_lib_x86_Release.lib ..\output\my_windows\lib
echo Distribution files has been put into `output\my_windows\' directory.
pause
