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

/// <summary>
/// 创建或打开指定的命名或未命名的文件映射内核对象
/// </summary>
/// <remarks>
/// 创建文件映射对象后，文件的大小不能超过文件映射对象的大小；如果是，则并非所有文件内容都可用于共享。
///
/// 若只是想要和其他进程共享数据，则可以将file_handle置为INVALID_HANDLE_VALUE。此时后背物理存储器变为页面交换文件，而不是磁盘某个具体文件
///
/// 对于使用页交换文件作为后备存储器来创建的文件映射对象，可以使用SEC_COMMIT和SEC_RESERVE标志来控制是否从页交换文件中调拨物理存储器。
/// 注意，这两个标志只对于使用页交换文件的文件映射对象有效，默认是SEC_COMMIT，即在创建文件映射对象的时候从页交换文件中调拨物理存储器。
/// 若不想在创建的时候就调拨物理存储器，则使用SEC_RESERVE标志。随后调用的MapViewOfFileEx，会在调用线程预订一块进程地址空间，但是不会
/// 调拨物理存储器，所以你直接访问是会引发访问违规的。你可以使用VirtualAlloc来进行调拨物理存储器。一旦调拨之后，所有映射了同一个文件映射对象的视图的
/// 其他进程都可以访问已调拨的页面了。
/// </remarks>
/// <param name="file_handle">要从中创建文件映射对象的文件句柄，必须与page_protect兼容的标志打开文件。若为INVALID_HANDLE_VALUE，那么必须指定`maximum_size`，此时文件映射对象的物理存储器将变为页面文件</param>
/// <param name="page_protect">指定文件映射对象的页面保护属性，它必须与指定文件句柄访问权限兼容，它是PAGE_宏的其中一个(看文档)，可以外加SEC_的一个或多个组合(看文档)</param>
/// <param name="maximum_size">若该参数为0，则文件映射对象的最大大小等于file_handle表示的当前大小，若尝试映射大小为0的文件将失败，</param>
/// <param name="mapping_name">文件映射对象的名字，该名称限制为MAX_PATH个字符，区分大小写，可以为nullptr，即未命名的内核对象</param>
/// <param name="mapping_attributes">文件映射对象的安全属性</param>
/// <returns>若成功，则返回新创建文件映射对象，若已经存在同样命名的内核对象，则返回它(使用它的大小，而不是指定大小)，若函数失败返回NULL</returns>
inline HANDLE create_file_mapping(HANDLE file_handle, DWORD page_protect = PAGE_READWRITE,
    ULONGLONG maximum_size = 0,
    const std::tstring& mapping_name = _T(""),
    LPSECURITY_ATTRIBUTES mapping_attributes = nullptr)
{
    ULARGE_INTEGER t = { 0 };
    t.QuadPart = maximum_size;
    auto val = CreateFileMapping(file_handle, mapping_attributes, page_protect,
        t.HighPart, t.LowPart, tstring_to_pointer(mapping_name));
    GET_ERROR_MSG_OUTPUT();
    return val;
}

/// <summary>
/// 打开一个现有命名的文件映射对象
/// </summary>
/// <param name="mapping_name">文件映射对象的名字</param>
/// <param name="inherit_handle">是否允许调用进程之后(如果)创建的新进程是否继承该句柄</param>
/// <param name="desired_access">它是FILE_MAP_宏的组合，表示该句柄应该获取的访问权限</param>
/// <returns>若成功，返回指定文件映射对象的打开句柄，若失败返回NULL</returns>
inline HANDLE open_file_mapping(const std::tstring& mapping_name,
    BOOL inherit_handle = false, DWORD desired_access = FILE_MAP_READ | FILE_MAP_WRITE)
{
    auto val = OpenFileMapping(desired_access, inherit_handle, mapping_name.c_str());
    GET_ERROR_MSG_OUTPUT();
    return val;
}

/// <summary>
/// 将文件映射视图映射到调用进程的地址空间中，调用者可以选择为视图指定基地址，但是不推荐(指定基地址一般用于进程间共享数据)
/// </summary>
/// <remarks>
/// 映射视图在内存中:开始地址base_address，长度number_of_bytes_to_map
/// 其中从指定文件file_offset，长度为number_of_bytes_to_map.
///
/// 映射文件使文件的指定部分在调用进程的地址空间中可见，使用VirtualQueryEx可以获取视图的大小。
/// 用于映射的区域不能进行其他内存分配，包括使用 VirtualAlloc或 VirtualAllocEx函数来预留内存。
/// </remarks>
/// <param name="mapping_handle">文件映射对象的句柄，它应该是CreateFileMapping或 OpenFileMapping的返回值</param>
/// <param name="file_offset">指定文件映射内的偏移，它必须是系统分配粒度的倍数，使用GetSystemInfo获取分配粒度数值</param>
/// <param name="number_of_bytes_to_map">映射到视图的文件映射的字节数，必须在CreateFileMapping指定最大范围内。若为0，则映射到文件映射末尾</param>
/// <param name="desired_access">它是FILE_MAP宏的组合,决定对文件映射对象的访问类型</param>
/// <param name="base_address">指向映射开始的虚拟空间地址，它必须是系统内存分配粒度的倍数，否则失败，若为NULL，则由系统选择映射地址</param>
/// <returns>若成功，返回值就是映射视图的起始地址，若失败返回NULL</returns>
inline PVOID map_view_of_file(HANDLE mapping_handle, ULONGLONG file_offset = 0,
    SIZE_T number_of_bytes_to_map = 0, DWORD desired_access = FILE_MAP_ALL_ACCESS,
    LPVOID base_address = nullptr)
{
    ULARGE_INTEGER t = { 0 };
    t.QuadPart = file_offset;
    auto val = MapViewOfFileEx(mapping_handle, desired_access, t.HighPart,
        t.LowPart, number_of_bytes_to_map, base_address);
    GET_ERROR_MSG_OUTPUT();
    return val;
}

/// <summary>
/// 从调用进程的地址空间取消映射文件的映射视图
/// </summary>
/// <remarks>
/// 请看文档
/// </remarks>
/// <param name="base_address">指向要取消映射的文件的映射视图的基地址的指针。此值必须与先前调用MapViewOfFile(Ex)函数所返回的值相同</param>
/// <returns>操作是否成功</returns>
inline BOOL unmap_view_of_file(LPCVOID base_address)
{
    auto val = UnmapViewOfFile(base_address);
    GET_ERROR_MSG_OUTPUT();
    return val;
}

/// <summary>
/// 将文件映射视图范围内的字节写入磁盘
/// </summary>
/// <remarks>该函数只会将范围内的脏页(dirty page)写入磁盘，所谓脏页就是在文件视图映射后内容已经改变的页面</remarks>
/// <param name="base_address">指向要刷新到映射文件的磁盘的基地址指针</param>
/// <param name="number_of_bytes_to_flush">要刷新的字节数，若为0，则从基地址刷新到映射文件的末尾</param>
/// <returns>操作是否成功</returns>
inline BOOL flush_view_of_file(LPCVOID base_address, SIZE_T number_of_bytes_to_flush = 0)
{
    auto val = FlushViewOfFile(base_address, number_of_bytes_to_flush);
    GET_ERROR_MSG_OUTPUT();
    return val;
}

/// <summary>
/// 获取调用进程的默认堆的句柄，然后可以在对堆函数的后续调用中使用此句柄。
/// </summary>
/// <returns>若成功，返回调用进程的默认堆句柄，若失败，返回NULL</returns>
inline HANDLE get_process_heap()
{
    auto val = GetProcessHeap();
    GET_ERROR_MSG_OUTPUT();
    return val;
}

/// <summary>
/// 创建可由调用进程使用的私有堆对象，该函数在进程虚拟地址空间预订空间，并为该块指定的初始部分调拨物理存储器
/// </summary>
/// <remarks>
/// HEAP_NO_SERIALIZE指定堆不进行同步访问，这意味着多线程访问指定堆时，堆将被破坏。序列化访问性能开销很低，一般不要指定这个标志。
/// HEAP_GENERATE_EXCEPTIONS标志告诉系统，当堆中分配或重新分配内存块失败的时候，抛出一个异常。
/// HEAP_CREATE_ENABLE_EXECUTE标志使得程序可以执行来自堆的内存块的代码，若不指定该标志，系统会抛出EXCEPTION_ACCESS_VIOLATION异常
/// </remarks>
/// <param name="options">它可以是0或HEAP_CREATE_ENABLE_EXECUTE和HEAP_GENERATE_EXCEPTIONS和HEAP_NO_SERIALIZE的组合</param>
/// <param name="initial_size">堆的初始大小(字节)，此值决定堆调拨的初始内存量(向上舍入为系统页面大小的倍数)，若为0，则该函数调拨一页</param>
/// <param name="maximum_size">堆的最大大小(字节)，若不为0，则堆大小是固定的，不能超过最大大小，若为0，则堆大小可以增长，直到所有物理存储器耗尽</param>
/// <returns>若成功，返回新创建的堆的句柄，若失败返回NULL</returns>
inline HANDLE heap_create(DWORD options = 0, SIZE_T initial_size = 0, SIZE_T maximum_size = 0)
{
    auto val = HeapCreate(options, initial_size, maximum_size);
    GET_ERROR_MSG_OUTPUT();
    return val;
}

/// <summary>
/// 为指定堆启用功能
/// </summary>
/// <param name="heap_handle">要设置信息的堆的句柄，该句柄由HeapCreate或 GetProcessHeap函数返回</param>
/// <param name="heap_information_class">它是HEAP_INFORMATION_CLASS枚举中之一</param>
/// <param name="heap_information">堆信息缓冲区。此数据的格式取决于HeapInformationClass参数的值</param>
/// <param name="heap_information_length">heap_information缓冲区的大小，以字节为单位</param>
/// <returns>操作是否成功</returns>
inline BOOL heap_set_information(HANDLE heap_handle = nullptr,
    HEAP_INFORMATION_CLASS heap_information_class = HeapEnableTerminationOnCorruption,
    PVOID heap_information = nullptr, SIZE_T heap_information_length = 0)
{
    auto val = HeapSetInformation(heap_handle, heap_information_class,
        heap_information, heap_information_length);
    GET_ERROR_MSG_OUTPUT();
    return val;
}

/// <summary>
/// 从堆中分配一块内存。分配的内存不可移动
/// </summary>
/// <remarks>
/// flags只对本次分配有效，而HeapCreate指定的options对所有分配有效，flags优先级高于options。
/// HEAP_ZERO_MEMORY标志将使得分配的内存被清零
/// </remarks>
/// <param name="heap_handle">将分配内存的堆的句柄。该句柄由HeapCreate或 GetProcessHeap函数返回</param>
/// <param name="bytes">要分配的字节数，若堆是固定大小的，则该值必须小于0x7FFF8</param>
/// <param name="flags">它可以是0或HEAP_GENERATE_EXCEPTIONS和HEAP_NO_SERIALIZE和HEAP_ZERO_MEMORY的组合，该flag优先级本次分配高于HeapCreate指定的option</param>
/// <returns>若指定HEAP_GENERATE_EXCEPTIONS，则在失败时抛出异常，否则返回NULL，成功则返回已分配内存块的指针</returns>
inline LPVOID heap_alloc(HANDLE heap_handle, SIZE_T bytes, DWORD flags = 0)
{
    return HeapAlloc(heap_handle, flags, bytes);
}

/// <summary>
/// 从堆中重新分配一块内存。该函数使您能够调整内存块的大小并更改其他内存块属性。
/// </summary>
/// <remarks>
/// 若你有指针指向堆指定内存块，那么请务必加上HEAP_REALLOC_IN_PLACE_ONLY，否则该函数可能移动原来的内存块到一个新位置
/// 导致指针失效。若指定HEAP_REALLOC_IN_PLACE_ONLY，那么有可能无法再在原来的位置扩大大小，此时函数失败并返回NULL
/// </remarks>
/// <param name="heap_handle">要从中重新分配内存的堆的句柄。该句柄由 HeapCreate或 GetProcessHeap函数返回</param>
/// <param name="bytes">内存块的新大小(字节)，可以增加或减少内存块的大小，若指定堆是固定大小的，则该参数必须小于0x7FFF8</param>
/// <param name="block_alloc_before">要重新分配的内存块地址，它是HeapAlloc或HeapReAlloc的返回值 </param>
/// <param name="flags">0或HEAP_GENERATE_EXCEPTIONS和HEAP_NO_SERIALIZE和HEAP_REALLOC_IN_PLACE_ONLY和HEAP_ZERO_MEMORY</param>
/// <returns>若指定HEAP_GENERATE_EXCEPTIONS，则在失败时抛出异常，否则返回NULL，成功则返回已分配内存块的指针</returns>
inline LPVOID heap_realloc(HANDLE heap_handle, SIZE_T bytes, LPVOID block_alloc_before, DWORD flags = 0)
{
    return HeapReAlloc(heap_handle, flags, block_alloc_before, bytes);
}

/// <summary>
/// 获取由HeapAlloc或 HeapReAlloc函数从堆分配的指定内存块的大小
/// </summary>
/// <remarks>若指定内存块不在指定堆句柄中，则函数行为未定义</remarks>
/// <param name="heap_handle">内存块所在的堆的句柄，该句柄由 HeapCreate或 GetProcessHeap函数返回</param>
/// <param name="block_alloc_before">指向将获得其大小的内存块的指针。这是一个由HeapAlloc或 HeapReAlloc函数返回的指针.内存块必须来自heap_handle指定的堆</param>
/// <param name="flags">可以是0或HEAP_NO_SERIALIZE</param>
/// <returns>若成功，返回值是所分配内存块的请求大小，以字节为单位。若失败返回(SIZE_T)-1</returns>
inline SIZE_T heap_size(HANDLE heap_handle, LPCVOID block_alloc_before, DWORD flags = 0)
{
    return HeapSize(heap_handle, flags, block_alloc_before);
}

/// <summary>
/// 释放由HeapAlloc或 HeapReAlloc函数从堆分配的内存块
/// </summary>
/// <remarks>
/// 当指定内存块被释放后，你不应该再以任何方式使用它。
/// </remarks>
/// <param name="heap_handle">要释放其内存块的堆的句柄。该句柄由 HeapCreate或 GetProcessHeap函数返回</param>
/// <param name="block_alloc_before">指向将要释放的内存块的指针。这是一个由HeapAlloc或 HeapReAlloc函数返回的指针.内存块必须来自heap_handle指定的堆，可以为NULL</param>
/// <param name="flags">可以是0或HEAP_NO_SERIALIZE</param>
/// <returns>操作是否成功</returns>
inline BOOL heap_free(HANDLE heap_handle, LPVOID block_alloc_before, DWORD flags = 0)
{
    auto val = HeapFree(heap_handle, flags, block_alloc_before);
    GET_ERROR_MSG_OUTPUT();
    return val;
}

/// <summary>
/// 销毁指定的堆对象，它取消调拨并释放私有堆的所有页面，并使堆的句柄无效。对默认堆无效
/// </summary>
/// <remarks>
/// 进程可以调用HeapDestroy而无需先调用 HeapFree函数来释放从堆分配的内存。
/// </remarks>
/// <param name="heap_handle">要销毁的堆的句柄。该句柄由HeapCreate函数返回 。不要使用GetProcessHeap函数返回的进程堆的句柄 </param>
/// <returns>操作是否成功</returns>
inline BOOL heap_destroy(HANDLE heap_handle)
{
    auto val = HeapDestroy(heap_handle);
    GET_ERROR_MSG_OUTPUT();
    return val;
}

// HeapLock, HeapUnlock, HeapWalk,GetProcessHeaps,HeapValidate,HeapCompact作用不大

/// <summary>
/// 将指定进程的地址空间中的指定地址范围内的数据复制到当前进程的指定缓冲区中。任何具有 PROCESS_VM_READ 访问句柄的进程都可以调用该函数。要读取的整个区域必须是可访问的，如果不可访问，则函数失败
/// </summary>
/// <param name="process_handle">要被读取内存的进程句柄，该句柄必须具有PROCESS_VM_READ 访问权限</param>
/// <param name="base_address">指向要从中读取的指定进程中的基地址的指针。在发生任何数据传输之前，系统验证指定大小的基地址和内存中的所有数据是否可访问以进行读取访问</param>
/// <param name="buffer">[out]用于接收从指定进程地址空间复制数据的缓冲区</param>
/// <param name="size">要从指定进程读取的字节数</param>
/// <param name="number_of_bytes_read">[out]用于接收传输到指定缓冲区的字节数，若为NULL，则忽略该参数</param>
/// <returns>操作是否成功</returns>
inline BOOL read_process_memory(HANDLE process_handle, LPCVOID base_address,
    LPVOID buffer, SIZE_T size, SIZE_T* number_of_bytes_read = nullptr)
{
    auto val = ReadProcessMemory(process_handle, base_address, buffer, size, number_of_bytes_read);
    GET_ERROR_MSG_OUTPUT();
    return val;
}

/// <summary>
/// 将数据写入指定进程中的内存区域。要写入的整个区域必须可访问，否则操作将失败
/// </summary>
/// <param name="process_handle">要被修改内存的进程句柄，该句柄必须具有 PROCESS_VM_WRITE 和 PROCESS_VM_OPERATION 访问权限</param>
/// <param name="base_address">指向要从中写入的指定进程中的基地址的指针。在发生任何数据传输之前，系统验证指定大小的基地址和内存中的所有数据是否都可以进行写访问</param>
/// <param name="buffer">指向缓冲区的指针，该缓冲区包含要写入指定进程地址空间的数据</param>
/// <param name="size">要写入指定进程的字节数</param>
/// <param name="number_of_bytes_written">[out]用于接收写入指定进程的字节数，若为NULL，则忽略该参数</param>
/// <returns>操作是否成功</returns>
inline BOOL write_process_memory(HANDLE process_handle, LPVOID base_address,
    LPVOID buffer, SIZE_T size, SIZE_T* number_of_bytes_written = nullptr)
{
    auto val = WriteProcessMemory(process_handle, base_address, buffer, size, number_of_bytes_written);
    GET_ERROR_MSG_OUTPUT();
    return val;
}

}; // namespace mw