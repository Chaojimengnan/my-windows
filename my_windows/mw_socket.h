#pragma once

#pragma comment(lib, "Ws2_32.lib")

namespace mw {
namespace socket {
	/// <summary>
	/// 若你要使用socket库，首先应该运行该函数在当前进程中初始化Winsock 2 DLL
	/// </summary>
	/// <param name="major_version">主要版本号，默认为2</param>
	/// <param name="minor_version">次要版本号，默认为2</param>
	/// <returns>操作成功返回0！否则返回非0值</returns>
	MW_API int socket_startup(int major_version = 2, int minor_version = 2);

	/// <summary>
	/// 调用该函数来终止Winsock 2 DLL的使用(先确认你是否调用socket_startup)
	/// </summary>
	/// <remarks>
	/// 在多线程环境中， WSACleanup终止所有线程的 Windows Socket操作。
	/// </remarks>
	/// <returns>操作成功返回0！否则返回非0值</returns>
	MW_API int socket_cleanup();
};//window
};//mw