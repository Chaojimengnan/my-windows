#include "example_6.h"

constexpr size_t operator ""_GB(size_t t)
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

