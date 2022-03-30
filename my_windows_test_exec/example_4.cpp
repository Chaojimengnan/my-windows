#include "example_4.h"

void my_completion_routine(DWORD error_code, DWORD number_of_byte_transfered, LPOVERLAPPED overlapped)
{
    std::cout << error_code << "\n";
    std::cout << number_of_byte_transfered << "\n\n\n";
}

/// <summary>
/// 异步
/// </summary>
void example_4()
{
    auto file_handle = mw::create_file(_T("../temp/123.txt"), GENERIC_READ | GENERIC_WRITE, OPEN_EXISTING, 0);

    char* buffer = new char[100] { 0 };

    OVERLAPPED write_ol = { 0 };
    OVERLAPPED read_ol = { 0 };
    LARGE_INTEGER kaka = { 0 };
    /*kaka.QuadPart = 5;
	ol.Offset = kaka.LowPart;
	ol.OffsetHigh = kaka.HighPart;*/
    std::string tata = "Fucking those shit!";
    std::string tat1 = "Oh My Lady GAGA!";

    //write_ol.Offset = tata.size();
    write_ol.Offset = -1;
    write_ol.OffsetHigh = -1;
    mw::write_file_async(file_handle, tata.c_str(), tata.size(), &write_ol, my_completion_routine);

    mw::write_file_async(file_handle, tat1.c_str(), tat1.size(), &write_ol, my_completion_routine);

    mw::read_file_async(file_handle, buffer, 100, &read_ol, my_completion_routine);

    mw::sync::sleep_alertable();

    std::cout << buffer << "\n";
}

/// <summary>
/// 同步
/// </summary>
void example_4_1()
{
    auto file_handle = mw::create_file(_T("../temp/123.txt"),
        GENERIC_READ | GENERIC_WRITE, OPEN_EXISTING, 0, FILE_FLAG_SEQUENTIAL_SCAN | FILE_ATTRIBUTE_NORMAL);

    char* buffer = new char[100] { 0 };

    std::string tata = "Fucking those shit!";
    DWORD li = 0;

    mw::set_file_pointer(file_handle, { 0 }, FILE_END);

    mw::write_file(file_handle, tata.c_str(), tata.size(), &li);

    std::cout << li << "\n";

    mw::set_file_pointer(file_handle, { 0 }, FILE_BEGIN);

    mw::read_file(file_handle, buffer, 100, &li);

    std::cout << li << "\n";

    std::cout << buffer << "\n";
}

// APC回调函数
VOID CALLBACK APC_func(ULONG_PTR param)
{
    std::cout << "执行APC！\n";
}

DWORD WINAPI my_thread(PVOID param)
{
    HANDLE my_event = (HANDLE)param;

    auto dw = mw::sync::wait_for_single_object(my_event, INFINITE, true);
    switch (dw)
    {
    case WAIT_OBJECT_0:
        std::cout << "因为事件触发退出等待\n";
        break;
    case WAIT_IO_COMPLETION:
        std::cout << "因为APC退出等待！\n";
        break;
    default:
        std::cout << "程序出错\n";
        break;
    }

    return 0;
}

/// <summary>
/// 为指定线程添加APC来取消等待
/// </summary>
void example_4_2()
{
    auto my_event = mw::sync::create_event();
    auto my_thread = mw::c_create_thread(::my_thread, my_event);

    // ...
    for (size_t i = 0; i < 1000000; i++)
        ;

    mw::queue_user_APC(my_thread, APC_func);
    mw::sync::wait_for_single_object(my_thread);

    CloseHandle(my_event);
    CloseHandle(my_thread);
}

// 缓冲区大小
constexpr auto BUFFERSIZE = 64 * 1024;

// 读取源文件的完成键
constexpr auto ACTION_READ = 0;
// 写入目标文件的完成键
constexpr auto ACTION_WRITE = 1;

// 工作线程的数量
constexpr auto THREAD_NUM = 16;

// 源文件句柄
HANDLE src_file = nullptr;
// 目标文件句柄
HANDLE dest_file = nullptr;
// IO完成端口
HANDLE completion_port = nullptr;
// 源文件大小
LARGE_INTEGER src_file_size = { 0 };
// 当前读取点(相对于源文件)
volatile LARGE_INTEGER current_file_pointer = { 0 };

// 当前正在进行的异步 *写* 操作IO的数量
volatile LONG current_write_num = 0;
// 当前正在进行的异步 *读* 操作IO的数量
volatile LONG current_read_num = 0;

// 继承overlapped，增加一些自己的字段，如buffer指针
struct power_overlapped : public OVERLAPPED
{
    power_overlapped() : OVERLAPPED({ 0 }), io_buffer(new char[BUFFERSIZE]) { }
    ~power_overlapped()
    {
        if (io_buffer)
            delete[] io_buffer;
    }

    void* io_buffer;
};

// 用于每个线程读写的overlapped结构体和对应的buffer
power_overlapped overlapped_array[THREAD_NUM];

// 用于同步访问current_file_pointer的读写锁
mw::sync::slimrw_lock current_file_pointer_sl;

//mw::sync::critical_section out_cs;

// 用于指示是否是第一个退出循环的线程，它必须叫醒其他等待的线程
volatile LONG is_first = true;

DWORD WINAPI copy_thread(PVOID param)
{
    size_t index = (size_t)param;
    DWORD byte_to_transferred = 0;
    ULONG_PTR completion_key = 0;
    LPOVERLAPPED _overlapped = nullptr;
    power_overlapped* overlapped = nullptr;

    /*out_cs.into_section([&] {
		std::cout << "线程" << index << "开始工作了！\n";
		});*/

    while (current_read_num > 0 || current_write_num > 0)
    {
        mw::get_queued_completion_status(completion_port, byte_to_transferred, completion_key, _overlapped);
        overlapped = static_cast<power_overlapped*>(_overlapped);

        switch (completion_key)
        {
        case ACTION_READ:
        {
            // 不用设置偏移，因为复制文件其偏移是相同的
            /*out_cs.into_section([&] {
				std::cout << "线程" << index << "接收到读取io完成！\n";
				});*/
            mw::sync::interlocked_exchange_add(current_read_num, -1);
            mw::write_file(dest_file, overlapped->io_buffer, overlapped->InternalHigh, nullptr, overlapped);
            mw::sync::interlocked_increment(current_write_num);
            break;
        }
        case ACTION_WRITE:
        {
            /*out_cs.into_section([&] {
				std::cout << "线程" << index << "接收到写入io完成！\n";
				});*/
            mw::sync::interlocked_exchange_add(current_write_num, -1);

            // 检查当前偏移量是否超过源文件大小(需要共享同步)
            if (current_file_pointer_sl.into_shared_return([] {
                    return current_file_pointer.QuadPart >= src_file_size.QuadPart;
                }))
                break;

            // 若没有则设置最新的偏移量(需要独占同步)
            current_file_pointer_sl.acquire_exclusive();
            overlapped->Offset = current_file_pointer.LowPart;
            overlapped->OffsetHigh = current_file_pointer.HighPart;
            current_file_pointer.QuadPart += BUFFERSIZE;
            current_file_pointer_sl.release_exclusive();

            // 发送读取异步IO操作，并将当前读取异步IO的数量加1
            mw::read_file(src_file, overlapped->io_buffer, BUFFERSIZE, nullptr, overlapped);
            mw::sync::interlocked_increment(current_read_num);

            break;
        }
        }
    }

    // 第一个退出循环的线程必须要叫醒其他正在等待的线程
    if (is_first)
    {
        mw::sync::interlocked_exchange(is_first, false);
        for (size_t i = 0; i < THREAD_NUM - 1; i++)
            mw::post_queued_completion_status(completion_port, 0, ACTION_WRITE, nullptr);
    }

    /*out_cs.into_section([&] {
		std::cout << "线程" << index << "退出！\n";
		});*/

    return 0;
}

/// <summary>
/// 使用IO完成端口进行多线程复制文件
/// </summary>
void example_4_3()
{
    // 打开源文件
    src_file = mw::create_file(_T("D:\\base\\Captures\\2021-12-11 14-10-31.mp4"),
        GENERIC_READ, OPEN_EXISTING, FILE_SHARE_READ, FILE_FLAG_OVERLAPPED);

    // 获取源文件大小
    mw::get_file_size(src_file, src_file_size);

    // 打开目标文件
    dest_file = mw::create_file(_T("D:\\base\\Captures\\mymy.mp4"),
        GENERIC_ALL, CREATE_NEW, 0, FILE_FLAG_OVERLAPPED, src_file);

    // 创建IO完成端口并关联两个文件
    completion_port = mw::create_io_completion_port(src_file, nullptr, ACTION_READ);
    mw::create_io_completion_port(dest_file, completion_port, ACTION_WRITE);

    // 设置文件尾
    mw::set_file_pointer(dest_file, src_file_size, FILE_BEGIN);
    mw::set_end_of_file(dest_file);

    // 初始化每个overlapped，并指定偏移
    for (size_t i = 0; i < THREAD_NUM; i++)
    {
        overlapped_array[i].Offset = current_file_pointer.LowPart;
        overlapped_array[i].OffsetHigh = current_file_pointer.HighPart;
        current_file_pointer.QuadPart += BUFFERSIZE;
    }

    // 先读入THREAD_NUM次，以此来驱动读写循环
    for (size_t i = 0; i < THREAD_NUM; i++)
        mw::read_file(src_file, overlapped_array[i].io_buffer, BUFFERSIZE, nullptr, &overlapped_array[i]);

    current_read_num = THREAD_NUM;

    std::vector<HANDLE> thread_vec;

    // 创建THREAD_NUM-1个线程，加上主线程即THREAD_NUM个线程
    for (size_t i = 0; i < THREAD_NUM - 1; i++) thread_vec.push_back(mw::c_create_thread(copy_thread, (LPVOID)i));

    // 主线程进入复制工作
    copy_thread(0);

    // 等待其他线程退出
    mw::sync::wait_for_multiple_object(thread_vec.size(), thread_vec.data());

    // 关闭句柄
    for (auto&& item : thread_vec)
        CloseHandle(item);

    CloseHandle(src_file);
    CloseHandle(dest_file);
    CloseHandle(completion_port);
}