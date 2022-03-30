#include "example_6.h"

constexpr size_t operator""_GB(size_t t)
{
    return t * 1024 * 1024 * 1024;
}

void example_6()
{
    char* mymy = new char[3_GB];
    std::cout << 16_GB;

    //std::cout << (size_t*)mymy << "\n";

    /*int* tata = new int[10];


	for (size_t i = 0; i < 10; i++)
	{
		tata[i] = i;
	}*/

    for (size_t i = 0; i < 3_GB; i++)
    {
        mymy[i] = 'a';
    }
    mymy[3_GB - 1] = '\0';

    std::cout << mymy << "\n";
}

void example_6_1()
{
    // 获取页面大小
    SYSTEM_INFO si = { 0 };
    mw::get_system_info(si);
    size_t page_size = si.dwPageSize;

    std::cout << "当前系统页面大小为：" << page_size << "\n";

    // 预定两个页面的虚拟内存
    auto address = mw::virtual_alloc(mw::get_current_process(), page_size * 2, nullptr, MEM_RESERVE);

    // 调拨其中一个页面
    auto commited_address = mw::virtual_alloc(mw::get_current_process(), page_size, address, MEM_COMMIT);

    int* int_address = static_cast<int*>(commited_address);

    // 写入一些数据
    for (size_t i = 0; i < 10; i++)
    {
        int_address[i] = i;
    }

    // 读取这些数据
    for (size_t i = 0; i < 10; i++)
    {
        std::cout << int_address[i] << ",";
    }

    // 释放内存
    mw::virtual_free(mw::get_current_process(), address, MEM_RELEASE);
}

// MSVC only
#pragma data_seg("mymy")

// 在mymy段中
int a = 0;

// 不在mymy段中
int b;

#pragma data_seg()

// 在mymy段中
__declspec(allocate("mymy")) int c = 0;

// 在mymy段中
__declspec(allocate("mymy")) int d;

// 不在mymy段中
int e = 0;

// 不在mymy段中
int f;

#pragma data_seg("shared")

volatile LONG instance_counts = 0;

#pragma data_seg()

#pragma comment(linker, "/SECTION:shared,RWS")

// 创建一个共享段，它在所有该进程实例中共享
void example_6_2()
{
    auto timer = mw::sync::create_waitable_timer();
    LARGE_INTEGER dummy = { 0 };

    mw::sync::set_waitable_timer(timer, &dummy, 1000);

    mw::sync::interlocked_increment(instance_counts);

    while (true)
    {
        mw::sync::wait_for_single_object(timer);
        system("cls");
        std::cout << "当前正在运行的实例为：" << instance_counts << "\n";
    }
}

// 初次实战，创建内核映射文件
void example_6_3()
{
    auto file_handle = mw::create_file(_T("D:/1.txt"));
    auto file_map = mw::create_file_mapping(file_handle, PAGE_READWRITE, 100);

    // 将映射文件映射到进程地址空间
    auto file_base = static_cast<LPBYTE>(mw::map_view_of_file(file_map));

    // 将指定buffer写入到映射文件所在的进程空间地址
    memcpy_s(file_base, 100, "呵呵，你好吗，我的朋友\n", 34);

    // 确保将映射页面刷入文件
    mw::flush_view_of_file(file_base);

    // 取消映射
    mw::unmap_view_of_file(file_base);

    CloseHandle(file_map);
    CloseHandle(file_handle);
}

// 基地址
LPVOID based;

struct my_list;

// 两种都行，这个类型将表示指针存储在基地址based上的偏移地址，而不是完整的地址空间地址
// 所以实际上引用该指针值等于based + 该指针值，最后才是真正的地址空间地址
// 这样当两个进程的页面映射文件基地址不同时，这个指针也能用，因为它存储的偏移量不是实际地址空间的地址
using my_list_ptr = my_list(__based(based) *);
//typedef my_list __based(based)*my_list_ptr;

struct my_list
{
    my_list_ptr next;
    int val;
};

// 使用__base在进程间共享一个链表
// 准备数据方
void example_6_4()
{
    // 以页交换文件为后备存储器的内存映射文件对象
    auto file_map = mw::create_file_mapping(INVALID_HANDLE_VALUE, PAGE_READWRITE, 100, _T("shared_mapping"));

    auto finish_event = mw::sync::create_event(0, EVENT_ALL_ACCESS, _T("finish_event"));

    // 将映射文件映射到进程地址空间
    based = static_cast<LPBYTE>(mw::map_view_of_file(file_map));

    my_list_ptr list_ptr = 0;

    // 写入三个my_list结构
    for (size_t i = 0; i < 3; i++)
    {
        list_ptr[i].val = i;
        list_ptr[i].next = list_ptr + i + 1;
    }
    list_ptr[2].next = nullptr;

    mw::sync::set_event(finish_event); // 通知接收数据的进程，数据已经准备完毕

    mw::sleep(10000);

    // 确保将映射页面刷入文件
    mw::flush_view_of_file(based);

    // 取消映射
    mw::unmap_view_of_file(based);

    CloseHandle(finish_event);
    CloseHandle(file_map);
}

// 接收数据方
void example_6_5()
{
    ;
    auto file_map = OpenFileMapping(PAGE_READWRITE, false, _T("shared_mapping"));
    GET_ERROR_MSG_OUTPUT();

    auto finish_event = mw::sync::open_event(_T("finish_event"));

    // 将映射文件映射到进程地址空间
    based = static_cast<LPBYTE>(mw::map_view_of_file(file_map, 0, 0, FILE_MAP_READ));

    // 等待数据准备完毕
    mw::sync::wait_for_single_object(finish_event);

    my_list_ptr list_ptr = 0;

    std::cout << list_ptr[0].val << "\n";

    auto temp_ptr = list_ptr->next;

    // 读取三个my_list结构
    while (temp_ptr)
    {
        std::cout << temp_ptr->val << "\n";
        temp_ptr = temp_ptr->next;
    }

    Sleep(5000);
    // 取消映射
    mw::unmap_view_of_file(based);

    CloseHandle(finish_event);
    CloseHandle(file_map);
}

// 使用堆
void example_6_6()
{
    auto heap_handle = mw::heap_create();
    auto data_address = (int*)mw::heap_alloc(heap_handle, 100 * sizeof(int), HEAP_ZERO_MEMORY);

    std::cout << "size:" << mw::heap_size(heap_handle, data_address) << "\n";

    for (size_t i = 0; i < 100; i++)
    {
        data_address[i] = i;
    }

    auto data_address_new = (int*)mw::heap_realloc(heap_handle, 200 * sizeof(int), data_address, HEAP_ZERO_MEMORY);

    std::cout << "size:" << mw::heap_size(heap_handle, data_address_new) << "\n";

    for (size_t i = 100; i < 200; i++)
    {
        data_address_new[i] = i;
    }

    for (size_t i = 0; i < 200; i++)
    {
        std::cout << data_address_new[i] << " ";
    }

    // 可选的，实际没必要
    //mw::heap_free(heap_handle, data_address_new);
    mw::heap_destroy(heap_handle);

    int* a = new int();
}