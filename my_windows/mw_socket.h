#pragma once

#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <winsock2.h>

#pragma comment(lib, "Ws2_32.lib")

namespace mw::socket {

/// <summary>
/// 若你要使用socket库，首先应该运行该函数在当前进程中初始化Winsock 2 DLL
/// </summary>
/// <param name="data">[out]用于接收 Windows 套接字实现的详细信息</param>
/// <param name="major_version">主要版本号，默认为2</param>
/// <param name="minor_version">次要版本号，默认为2</param>
/// <returns>操作成功返回0！否则返回非0值</returns>
inline int socket_startup(WSADATA& data, int major_version = 2, int minor_version = 2)
{
    auto result = WSAStartup(MAKEWORD(major_version, minor_version), &data);
    return result;
}

/// <summary>
/// 调用该函数来终止Winsock 2 DLL的使用(先确认你是否调用socket_startup)
/// </summary>
/// <remarks>
/// 在多线程环境中， WSACleanup终止所有线程的 Windows Socket操作。
/// </remarks>
/// <returns>操作成功返回0！否则返回非0值</returns>
inline int socket_cleanup()
{
    return WSACleanup();
}

/// <summary>
/// 该函数提供与协议无关的从指定主机名到地址的转换
/// </summary>
/// <param name="node_name">该字符串是主机(节点)的名称或数字主机地址字符串(如baidu.com,或者IPv4,192.168.0.1，或者IPV6,ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff)</param>
/// <param name="service_name">该字符串是服务名字或端口数字,服务名字是端口数字的别名，比如"http"等于"80"，服务名字可选值在system32\drivers\etc\services中</param>
/// <param name="hints">用于指示调用者支持的套接字类型，比如你可以指示只要IPv4地址(ai_addrlen,ai_canonname,ai_addr,和ai_next成员必须为0,否则出错)</param>
/// <param name="result">[out]一个指向链表的指针,该链表是一个或多个ADDRINFOT结构体,它包含主机回应的信息。若函数成功,该链表请用FreeAddrInfo释放</param>
/// <returns>成功返回0，否则返回非零的Windows Socket错误代码,它是WSA_开头的宏，请看文档</returns>
inline INT get_address_info(const std::tstring& node_name, const std::tstring& service_name, const ADDRINFOT& hints, ADDRINFOT*& result)
{
    auto val = GetAddrInfo(node_name.c_str(), service_name.c_str(), &hints, &result);
    GET_ERROR_MSG_OUTPUT_SOCKET();
    return val;
}

/// <summary>
/// 该函数释放由get_address_info动态分配的包含地址信息的链表或者单个ADDRINFOT结构体
/// </summary>
/// <param name="address_info">指向ADDRINFO结构体的指针，或者一个ADDRINFO的链表，它们都会被释放，包括结构体内部指针指向的动态内存</param>
inline VOID free_address_info(ADDRINFOT* address_info)
{
    FreeAddrInfo(address_info);
}

/// <summary>
/// 该函数创建一个指定类型的套接字，若protocol_info不为nullptr，套接字将绑定到WSAPROTOCOL_INFO结构指示的传输服务提供程序上
/// </summary>
/// <remarks>
/// TODO:
/// 为该函数作文档时，并未完全理解Socket工作方式，所以文档内容不保证完全正确，以后完善
/// </remarks>
/// <param name="address_family">指定的地址族，它是AF_开头的宏的组合，当前支持AF_INET或AF_INET6(ipv4,ipv6)。其他地址族需要安装对应的套接字服务提供程序</param>
/// <param name="socket_type">指定套接字的类型，它是SOCK_开头的宏中的一个(如指定TCP还是UDP，请使用SOCK_STREAM或SOCK_DGRAM)，其他套接字类型请看文档</param>
/// <param name="protocol">使用的协议,该参数可能值依赖于地址族和套接字类型,它是IPPROTO_或BTHPROTO_开头宏中的一个,TCP是IPPROTO_TCP,UDP是IPPROTO_UDP</param>
/// <returns>若成功，返回引用到新套接字的描述符，若失败返回INVALID_SOCKET</returns>
inline SOCKET create_socket(int address_family = AF_INET | AF_INET6, int socket_type = SOCK_STREAM, int protocol = IPPROTO_TCP)
{
    auto val = WSASocket(address_family, socket_type, protocol, nullptr, 0, WSA_FLAG_OVERLAPPED);
    GET_ERROR_MSG_OUTPUT_SOCKET();
    return val;
}

/// <summary>
/// 关闭一个存在的套接字
/// </summary>
/// <param name="socket">标识一个要关闭的套接字的描述符</param>
/// <returns>若没有错误发送，返回0，否则返回SOCKET_ERROR</returns>
inline int close_socket(SOCKET socket)
{
    auto val = closesocket(socket);
    GET_ERROR_MSG_OUTPUT_SOCKET();
    return val;
}

/// <summary>
/// 在指定套接字上建立一个连接
/// </summary>
/// <param name="socket">一个标识一个未连接的套接字的描述符</param>
/// <param name="address">指向一个包含要连接的地址的sockaddr结构的指针，它可以是ADDRINFOT的ai_addr成员，它包含IP地址，端口和其他的东西，请看文档</param>
/// <param name="address_len">address参数指向的sockaddr结构的长度(以字节为单位)它可以是ADDRINFOT的ai_addrlen成员</param>
/// <returns>若没有错误发生，返回0，否则返回SOCKET_ERROR</returns>
inline int socket_connect(SOCKET socket, const sockaddr* address, int address_len)
{
    auto val = connect(socket, address, address_len);
    GET_ERROR_MSG_OUTPUT_SOCKET();
    return val;
}

/// <summary>
/// 该函数在一个已连接的套接字上发送数据，注意，该函数仅仅把buffer指向的用户空间数据复制到内核空间的发送缓冲区，然后由协议发送
/// </summary>
/// <param name="socket">一个标识一个已连接的套接字的描述符</param>
/// <param name="buffers_to_send">指向WSABUF结构数组的指针,每个WSABUF结构都包含一个指向缓冲区的指针和缓冲区的长度(字节),在发送期间必须保证有效</param>
/// <param name="buffers_array_counts">buffers_to_send数组中WSABUF结构的数量</param>
/// <param name="number_of_bytes_sent">[out]指向I/O操作完成时调用所发送的数字(以字节为单位)，对于重叠套接字不起作用，应为NULL</param>
/// <param name="flags">用于修改WSASend函数调用行为的标志，请看文档</param>
/// <param name="overlapped">[opt]指向WSAOVERLAPPED结构的指针，对于非重叠套接字，忽略此参数</param>
/// <param name="completion_routine">[opt]发送操作完成时调用的完成例程的指针,对于非重叠套接字，忽略此参数</param>
/// <returns>若没有发生错误并且发送操作立即完成，返回0。否则，返回SOCKET_ERROR，其他事项请看文档</returns>
inline int socket_send_asyn(SOCKET socket, LPWSABUF buffers_to_send, DWORD buffers_array_counts,
    LPDWORD number_of_bytes_sent, DWORD flags = 0, LPWSAOVERLAPPED overlapped = nullptr,
    LPWSAOVERLAPPED_COMPLETION_ROUTINE completion_routine = nullptr)
{
    auto val = WSASend(socket, buffers_to_send, buffers_array_counts,
        number_of_bytes_sent, flags, overlapped, completion_routine);
    GET_ERROR_MSG_OUTPUT_SOCKET();
    return val;
}

/// <summary>
/// 该函数在一个已连接的套接字上发送数据，注意，该函数仅仅把buffer指向的用户空间数据复制到内核空间的发送缓冲区，然后由协议发送
/// </summary>
/// <param name="socket">一个标识一个已连接的套接字的描述符</param>
/// <param name="buffer">一个指向包含要传输的数据的缓存区的指针</param>
/// <param name="buffer_len">缓冲区的长度(以字节为单位)</param>
/// <param name="flags">一组指定调用方式的标志，请看文档，可选值是MSG_DONTROUTE和MSG_OOB，或者它们的组合</param>
/// <returns>若没有发送错误，返回发送的总字节数(它可以比buffer_len小)，否则，返回SOCKET_ERROR</returns>
inline int socket_send(SOCKET socket, const char* buffer, int buffer_len, int flags = 0)
{
    auto val = send(socket, buffer, buffer_len, flags);
    GET_ERROR_MSG_OUTPUT_SOCKET();
    return val;
}

/// <summary>
/// 该函数从已连接的套接字或绑定(bind)的无连接套接字接收数据。若没有数据发送，则调用线程被阻塞，直到客户端发送信息(请回忆三次握手，四次挥手)
/// </summary>
/// <remarks>
/// 若没复制完，buffer满了，则需要多调用几次该函数来接收数据，注意，接收数据的是协议，该函数仅仅只是从内核空间的接收缓冲区复制接受的数据
/// </remarks>
/// <param name="socket">一个标识一个已连接的套接字的描述符</param>
/// <param name="buffers_to_receive">[in,out]指向WSABUF结构数组的指针,每个WSABUF结构都包含一个指向缓冲区的指针和缓冲区的长度(字节)</param>
/// <param name="buffers_array_counts">buffers_to_receive数组中WSABUF结构的数量</param>
/// <param name="number_of_bytes_received">[out]指向I/O操作完成时调用所接收的数字(以字节为单位)，对于重叠套接字不起作用，应为NULL</param>
/// <param name="flags">[in,out]用于修改WSARecv函数调用行为的标志</param>
/// <param name="overlapped">[opt]指向WSAOVERLAPPED结构的指针，对于非重叠套接字，忽略此参数</param>
/// <param name="completion_routine">[opt]发送操作完成时调用的完成例程的指针,对于非重叠套接字，忽略此参数</param>
/// <returns>若没有发生错误并且接收操作立即完成，返回0。否则，返回SOCKET_ERROR，其他事项请看文档</returns>
inline int socket_recv_asyn(SOCKET socket, LPWSABUF buffers_to_receive, DWORD buffers_array_counts,
    LPDWORD number_of_bytes_received, DWORD& flags, LPWSAOVERLAPPED overlapped = nullptr,
    LPWSAOVERLAPPED_COMPLETION_ROUTINE completion_routine = nullptr)
{
    auto val = WSARecv(socket, buffers_to_receive, buffers_array_counts,
        number_of_bytes_received, &flags, overlapped, completion_routine);
    GET_ERROR_MSG_OUTPUT_SOCKET();
    return val;
}

/// <summary>
/// 该函数从已连接的套接字或绑定(bind)的无连接套接字接收数据。若没有数据发送，则调用线程被阻塞，直到客户端发送信息(请回忆三次握手，四次挥手)
/// </summary>
/// <remarks>
/// 若没复制完，buffer满了，则需要多调用几次该函数来接收数据，注意，接收数据的是协议，该函数仅仅只是从内核空间的接收缓冲区复制接受的数据
/// </remarks>
/// <param name="socket">一个标识一个已连接的套接字的描述符</param>
/// <param name="buffer">[out]一个指向要接收传入数据的缓存区的指针</param>
/// <param name="buffer_len">缓冲区的长度(以字节为单位)</param>
/// <param name="flags">一组影响此函数行为的标志，它的可选值是MSG_PEEK，MSG_OOB，MSG_WAITALL，意义请看文档</param>
/// <returns>若没有错误发送，返回接收到的字节数，若连接已正常关闭，返回0.否则返回SOCKET_ERROR</returns>
inline int socket_recv(SOCKET socket, char* buffer, int buffer_len, int flags = 0)
{
    auto val = recv(socket, buffer, buffer_len, flags);
    GET_ERROR_MSG_OUTPUT_SOCKET();
    return val;
}

/// <summary>
/// 该函数禁用套接字的发送或接收，若禁用对应的操作，将无法对指定套接字使用send或recv函数
/// </summary>
/// <param name="socket">一个标识套接字的描述符</param>
/// <param name="how">描述什么类型的操作不再允许的标志，它可以是SD_开头的宏</param>
/// <returns>若没有错误发生，返回0，否则返回SOCKET_ERROR</returns>
inline int socket_shutdown(SOCKET socket, int how = SD_SEND)
{
    auto val = shutdown(socket, how);
    GET_ERROR_MSG_OUTPUT_SOCKET();
    return val;
}

/// <summary>
/// 该函数为指定套接字关联一个本地地址(local address)
/// </summary>
/// <param name="socket">标识一个未绑定的套接字的描述符</param>
/// <param name="address">要赋给绑定的套接字的本地地址</param>
/// <param name="address_len">address参数的长度(以字节为单位)</param>
/// <returns>若没有错误发生，返回0，否则返回SOCKET_ERROR</returns>
inline int socket_bind(SOCKET socket, const sockaddr* address, int address_len)
{
    auto val = bind(socket, address, address_len);
    GET_ERROR_MSG_OUTPUT_SOCKET();
    return val;
}

/// <summary>
/// 放置一个监听传入连接的套接字
/// </summary>
/// <param name="socket">标识一个绑定的未连接的套接字的描述符</param>
/// <param name="backlog">待处理连接队列的最大长度。如果设置为SOMAXCONN，负责 socket的底层服务提供者会将 backlog 设置为一个最大的合理值</param>
/// <returns>若没有错误发生，返回0，否则返回SOCKET_ERROR</returns>
inline int socket_listen(SOCKET socket, int backlog = SOMAXCONN)
{
    auto val = listen(socket, backlog);
    GET_ERROR_MSG_OUTPUT_SOCKET();
    return val;
}

/// <summary>
/// 该函数在套接字上允许一个传入连接请求，若没有传入连接请求，则调用线程被阻塞(若套接字被标记为不能阻塞，则返回一个错误，具体看文档)
/// </summary>
/// <param name="socket">一个标识已经置于监听状态的套接字的描述符，该函数实际建立连接</param>
/// <param name="address">[opt,out]用于接收连接实体的地址的结构体</param>
/// <param name="address_len">[opt,in,out]一个指向整数的可选指针，该整数包含由address参数指向的结构的长度，返回地址的实际长度(byte)</param>
/// <returns>若没有错误发送，返回新创建的连接的套接字，否则返回INVALID_SOCKET</returns>
inline SOCKET socket_accept(SOCKET socket, sockaddr* address = nullptr, int* address_len = nullptr)
{
    auto val = accept(socket, address, address_len);
    GET_ERROR_MSG_OUTPUT_SOCKET();
    return val;
}

}; // namespace mw::socket