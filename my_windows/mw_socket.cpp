#include "mw_socket.h"

namespace mw {
namespace socket {
	int socket_startup(int major_version, int minor_version)
	{
		WSADATA data;
		auto result = WSAStartup(MAKEWORD(major_version, minor_version), &data);
		return result;
	}
	int socket_cleanup()
	{
		return WSACleanup();
	}
};//window
};//mw