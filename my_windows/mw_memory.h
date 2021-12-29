#pragma once

namespace mw {

	/// <summary>
	/// 为指定进程的虚拟地址空间 *预定* ，*调拨* 或 *更改* 内存区域的状态，该函数将它分配的内存初始化为0
	/// </summary>
	/// <remarks>
	/// 该函数能调拨一个已预定的页面，能预定一个区域的闲置页面，或同时预定和调拨一个区域的页面
	/// 该函数不能预定一个已预定的页面，但是它可以调拨已调拨的页面，这意味着你可以调拨一定范围内的页面，不用去考虑是否已经被调拨，并且函数不会失败
	/// 
	/// 你可以使用该函数预定一个页面块，然后再使用该函数调拨一个个独立的页面，这使得进程能够预定一片虚拟空间并且直到它需要时才调拨物理存储器。
	/// 
	/// 若start_address不为NULL，则该函数使用start_address和size来计算要分配的页面区域，整个页面范围的当前状态必须与allocation_type指定的类型兼容
	/// 否则函数失败并且不会分配任何页面。
	/// 
	/// 如果要执行动态生成的代码，请使用该函数分配内存并使用VirtualProtectEx授予PAGE_EXECUTE访问权限
	/// 
	/// 使用MEM_RESET来告诉系统指定页面不需要写入页交换文件(如果要将该页换出的话)，而直接抛弃。下一次程序访问这段区域，应该将其视为已重置
	/// (其实你只需要理解成，之前修改的内容都被重置了，页面数据跟页交换文件是一样的)
	/// 。注意，当使用MEM_RESET时，会反向取整，及向小取整，该函数本来是想大取整。注意，MEM_RESET只能单独使用，不能与其他标志按位或在一起。
	/// 
	/// 其他事项请看文档
	/// </remarks>
	/// <param name="process_handle">进程的句柄。该函数在该进程的虚拟地址空间内分配内存，该句柄必须具有PROCESS_VM_OPERATION访问权限</param>
	/// <param name="size">要分配的内存区域的大小(字节)，若start_address为NULL，则向上取整到下一页边界，若不为NULL，则为address到address + size(注意如果跨越页面边界可能分配多余页)</param>
	/// <param name="start_address">若是预定内存，则向下舍入到最接近的分配粒度倍数，若是调拨已预定的内存，则向下舍入到最近页面边界，若为NULL，由函数决定分配哪个区域</param>
	/// <param name="allocation_type">内存分配类型，必须为MEM_的其中一个，不过可以另外搭配MEM_LARGE_PAGES和MEM_PHYSICAL和MEM_TOP_DOWN(MEM_RESERVE | MEM_COMMIT实现预定+调拨)</param>
	/// <param name="page_protect">PAGE_的其中一个，不过可以另外搭配PAGE_GUARD和PAGE_NOCACHE和PAGE_WRITECOMBINE，不过只能用于调拨而不是预定，请看文档</param>
	/// <returns>若成功，返回页面分配区域的基地址，失败返回NULL</returns>
	inline LPVOID virtual_alloc(HANDLE process_handle, size_t size, LPVOID start_address = nullptr, 
		DWORD allocation_type = MEM_RESERVE | MEM_COMMIT | MEM_TOP_DOWN, DWORD page_protect = PAGE_READWRITE)
	{
		auto val = VirtualAllocEx(process_handle, start_address, size, allocation_type, page_protect);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 释放，或取消调拨在指定进程的虚拟空间区域的内存
	/// </summary>
	/// <remarks>
	/// 进程虚拟地址空间的每一页内存都有一个page state(页面状态)，该函数可以取消调拨一系列处于不同状态，一些预定未调拨或已调拨的页面。
	/// 这意味着你可以不用考虑页面当前是否已调拨，直接取消调拨就完事了，取消调拨的页面会释放其物理存储，无论是内存中还是磁盘上的页面交换文件
	/// 
	/// 页面如果取消调拨但未释放，则状态变为预定状态，你可以使用VirtualAllocEx进行调拨，或使用该函数释放它。尝试读取或写入预定页面会导致访问冲突异常
	/// 
	/// 如果一个页面被释放，它的状态变为闲置，当内存被释放或取消调拨，你将永远无法再次引用该内存，任何可能存在于内存中的信息都将永远消失，尝试读取或写入
	/// 闲置页面将导致访问冲突异常。
	/// 
	/// MEM_RELEASE会释放使用VirtualAllocEx预定和调拨的整个区域的页面，不管其中的页面是不是处于相同状态，若是已调拨的页面，该函数首先取消调拨，然后释放
	/// 若是预定页面，则直接释放
	/// </remarks>
	/// <param name="process_handle">进程的句柄。该函数在进程的虚拟地址空间内释放内存，该句柄必须具有PROCESS_VM_OPERATION访问权限</param>
	/// <param name="start_address">指向要释放的内存区域的基地址指针，若free_type为MEM_RELEASE，则该参数必须是使用VirtualAllocEx预定区域时返回的基地址</param>
	/// <param name="free_type">它可以是MEM_DECOMMIT或MEM_RELEASE，前者取消调拨，但预定空间没有释放，后者取消调拨并且收回预定空间</param>
	/// <param name="size">要释放的内存区域大小，若free_type为MEM_RELEASE，则该参数必须为0。若为MEM_DECOMMIT，则取消调拨指定大小的内存页，若为0，则整个区域都被取消调拨</param>
	/// <returns>操作是否成功</returns>
	inline BOOL virtual_free(HANDLE process_handle, LPVOID start_address, DWORD free_type = MEM_RELEASE, size_t size = 0)
	{
		auto val = VirtualFreeEx(process_handle, start_address, size, free_type);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 原型为GlobalMemoryStatusEx，获取系统当前使用物理和虚拟内存的信息，注意该信息是实时的，即第一次调用和第二次调用返回的信息可能不同
	/// </summary>
	/// <param name="memory_status">[out]一个用户分配的MEMORYSTATUSEX结构体，用于接收当前内存相关信息</param>
	/// <returns>操作是否成功</returns>
	inline BOOL virtual_memory_status(MEMORYSTATUSEX& memory_status)
	{
		memory_status.dwLength = sizeof(MEMORYSTATUSEX);
		auto val = GlobalMemoryStatusEx(&memory_status);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 获取指定进程的虚拟地址空间内的一系列页面信息
	/// </summary>
	/// <remarks>
	/// 注意，若共享的write-copy页面被修改，它对于修改该页面的进程是私有的，但是该函数依然将其报告为MEM_MAPPED或MEM_IMAGE而不是MEM_PRIVATE
	/// </remarks>
	/// <param name="process_handle">指定要查询其内存信息的进程的句柄，该句柄必须具有PROCESS_QUERY_INFORMATION访问权限</param>
	/// <param name="start_address">指向要查询的页面区域基地址，该值向下舍入到下一页边界，若地址高于进程可访问的最高内存地址，函数失败</param>
	/// <param name="buffer">[out]用户分配的MEMORY_BASIC_INFORMATION结构，用于接收指定页面范围的信息</param>
	/// <returns>返回buffer实际接收的字节数，若函数失败则为0</returns>
	inline SIZE_T virtual_query(HANDLE process_handle, LPCVOID start_address, MEMORY_BASIC_INFORMATION& buffer)
	{
		auto val = VirtualQueryEx(process_handle, start_address, &buffer, sizeof(MEMORY_BASIC_INFORMATION));
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 改变指定进程虚拟地址空间中已调拨页面区域的保护属性
	/// </summary>
	/// <param name="process_handle">指定进程，该句柄必须具有PROCESS_VM_OPERATION访问权限</param>
	/// <param name="start_address">指向要改变保护属性的页面区域的基地址，不能跨区域更改，所更改的范围必须在同一个预订区域之中</param>
	/// <param name="size">要改变保护属性的区域大小(字节)，若跨页面边界，则被跨页面也会被修改</param>
	/// <param name="new_protect">新的内存保护属性，它可以是非WRITECOPY和非EXECUTE_WRITECOPY的PAGE_*保护属性</param>
	/// <param name="old_protect">[out]用于接收先前的保护属性</param>
	/// <returns>操作是否成功</returns>
	inline BOOL virtual_protect(HANDLE process_handle, LPVOID start_address, 
		SIZE_T size, DWORD new_protect, DWORD& old_protect)
	{
		auto val = VirtualProtectEx(process_handle, start_address, size, new_protect, &old_protect);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 获取大页面(large page)的最小尺寸(通常为2MB或更大)
	/// </summary>
	/// <returns>若CPU支持大页面，返回值是大页面的最小尺寸，若不支持大页面，则返回值为0</returns>
	inline SIZE_T get_large_page_minimum()
	{
		return GetLargePageMinimum();
	}

};