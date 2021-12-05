#include "stdafx.h"
#include "example_5.h"

/// <summary>
/// 该例子展示使用socket，获取指定服务主机的地址
/// </summary>
void example_5()
{
	WSADATA wsa = {0};
	mw::socket::socket_startup(wsa);

	ADDRINFOT hints = { 0 };
	hints.ai_family = AF_UNSPEC;		// 不指定地址族(你可以指定为IPv4或IPv6，或其他等等)
	hints.ai_socktype = SOCK_STREAM;	// 指定套接字类型为流套接字
	hints.ai_protocol = IPPROTO_TCP;	// 指定为TCP协议
	
	ADDRINFOT* address_info = nullptr;
	// 获取github.com:443的地址
	mw::socket::get_address_info(_T("baidu.com"), _T("443"), hints, address_info);

	auto iter = address_info;

	// 遍历链表
	while (iter)
	{
		std::cout << iter->ai_addr << ", " << iter->ai_addrlen << "\n";
		iter = iter->ai_next;
	}
	// 释放链表
	FreeAddrInfo(address_info);
	
	mw::socket::socket_cleanup();
}


constexpr auto default_port = _T("10086");

void example_5_server()
{
	WSADATA wsa = { 0 };
	mw::socket::socket_startup(wsa);

	ADDRINFOT hints = { 0 };
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	ADDRINFOT* address_info = nullptr;

	// 获取本地地址
	mw::socket::get_address_info(_T(""), default_port, hints, address_info);
	// 创建监听套接字
	auto listen_socket = mw::socket::create_socket(address_info->ai_family, address_info->ai_socktype, address_info->ai_protocol);

	// 给监听套接字绑定地址和端口
	mw::socket::socket_bind(listen_socket, address_info->ai_addr, address_info->ai_addrlen);

	mw::socket::free_address_info(address_info);	// 使用完地址链表后释放资源

	mw::socket::socket_listen(listen_socket);	// 让监听关键字进入监听状态，监听绑定的地址和端口

	auto client_socket = mw::socket::socket_accept(listen_socket);	// 等待客户端发送连接请求，然后建立连接
	
	mw::socket::close_socket(listen_socket); // 不再需要监听关键字

	char recv_buf[512] = {0};
	int result = 0;

	// 接收数据和发送数据
	do {
		result = mw::socket::socket_recv(client_socket, recv_buf, 512);
		if (!result) break;
		std::tcout << (TCHAR*)recv_buf << _T("\n");

		mw::socket::socket_send(client_socket, recv_buf, 512);

	} while (result > 0);

	std::tcout << _T("结束\n");

	mw::socket::socket_shutdown(client_socket);	// 连接结束,因为客户端在这之前肯定已经关闭了发送
	mw::socket::close_socket(client_socket);

	mw::socket::socket_cleanup();
}

void example_5_client()
{
	WSADATA wsa = { 0 };
	mw::socket::socket_startup(wsa);

	ADDRINFOT hints = { 0 };
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	ADDRINFOT* address_info = nullptr;

	// 获取本地地址
	mw::socket::get_address_info(_T(""), default_port, hints, address_info);

	// 连接到服务器的套接字
	auto server_socket = mw::socket::create_socket(address_info->ai_family, address_info->ai_socktype, address_info->ai_protocol);

	mw::socket::socket_connect(server_socket, address_info->ai_addr, address_info->ai_addrlen);	// 连接服务器

	char recv_buf[512] = { 0 };

	mw::socket::socket_send(server_socket, (const char*)_T("你好吗？"), 9);

	mw::socket::socket_shutdown(server_socket);	// 关闭套接字发送功能

	int result = 0;

	do
	{
		result = mw::socket::socket_recv(server_socket, recv_buf, 512);
		if (!result) break;
		std::tcout << (TCHAR*)recv_buf << _T("\n");
		
	} while (result > 0);

	mw::socket::close_socket(server_socket);
	mw::socket::socket_cleanup();
}