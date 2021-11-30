#include "stdafx.h"
#include "example_3.h"


DWORD WINAPI ThreadFunc(PVOID param)
{
	
	std::tcout << _T("这是另一个线程，正在执行中~\n");
	return 0;
}


void example_3()
{
	/*auto thread_handle = CreateThread(NULL, 0, ThreadFunc, nullptr, CREATE_SUSPENDED, nullptr);
	SetThreadPriority(thread_handle, THREAD_PRIORITY_IDLE);
	ResumeThread(thread_handle);
	CloseHandle(thread_handle);*/

	std::tcout << _T("这是主线程，正在执行~\n");

	auto thread_handle = mw::safe_handle(mw::c_create_thread(ThreadFunc));

	std::tcout << _T("新线程创建完毕~\n");

	// 等待线程返回
	WaitForSingleObject(*thread_handle, INFINITE);

	std::tcout << _T("新线程返回了~\n");
}



/////////////////////////////////////////////////////////

long g_x = 0;

DWORD WINAPI ThreadFunc1(PVOID param)
{
	mw::interlocked_exchange_add(g_x, 1);
	return 0;
}


DWORD WINAPI ThreadFunc2(PVOID param)
{
	mw::interlocked_exchange_add(g_x, 1);
	return 0;
}

class MyClass
{
public:
	MyClass(int a) {
		this->a = a;
		std::cout << a <<",进入构造函数" << "\n";
	}
	MyClass(MyClass&& t) noexcept {
		 a = t.a;
		std::cout << a << ",进入移动构造函数" << "\n";
	}
	MyClass(const MyClass& t) noexcept {
		a = t.a;
		std::cout << a << ",进入拷贝构造函数" << "\n";
	}

	MyClass& operator=(const MyClass& t)
	{
		a = t.a;
		return *this;
	}

	MyClass& operator=(MyClass&& t) noexcept
	{
		a = t.a;
		return *this;
	}

	~MyClass()
	{
		std::cout << a << ",进入析构函数" << "\n";
	}
	int a;
private:
	
};




void example_3_1()
{
	/*auto thread_handle1 = mw::safe_handle(mw::c_create_thread(ThreadFunc1));
	auto thread_handle2 = mw::safe_handle(mw::c_create_thread(ThreadFunc2));


	WaitForSingleObject(*thread_handle1, INFINITE);
	WaitForSingleObject(*thread_handle2, INFINITE);

	auto* my_ptr1 = (int*)malloc(sizeof(int));
	auto* my_ptr2 = (int*)_aligned_malloc(sizeof(int), 2);
	auto* my_ptr3 = (int*)_aligned_malloc(sizeof(int), 4);
	auto* my_ptr4 = (int*)_aligned_malloc(sizeof(int), 8);


	std::cout << g_x <<  ",完成\n";
	std::cout << &g_x << "\n";
	std::cout << my_ptr1 << "\n";
	std::cout << my_ptr2 << "\n";
	std::cout << my_ptr3 << "\n";
	std::cout << my_ptr4 << "\n";*/

	/*using my_type = MyClass;

	mw::interlocked_list<my_type> mymy;
	mymy.push(1);
	mymy.push(2);
	mymy.push(3);

	my_type temp(0);
	while (mymy.pop(temp))
	{
		std::cout << temp.a << "\n";
	}

	std::cout << mymy.size() << ",aaa\n";*/

	// 申请原始内存
	auto mymy = (MyClass*)_aligned_malloc(sizeof(MyClass), MEMORY_ALLOCATION_ALIGNMENT);
	// 构造MyClass
	new(mymy) MyClass(3);
	std::cout << mymy->a << "\n";

	// 调用析构函数
	mymy->~MyClass();
	// 释放内存
	_aligned_free(mymy);

}


/////////////////////////////////////////////////////////


//mw::interlocked_list<std::string> my_list;
//mw::interlocked_list<std::string> my_out_list;
std::vector<std::string> vec_list;
std::vector<std::string> vec_out_list;
mw::critical_section my_cs_input;
mw::critical_section my_cs_output;


DWORD WINAPI input_thread(PVOID param)
{
	for (size_t i = 0; i < 3; i++)
	{
		std::ostringstream s;
		s << (const char*)param << ":" << i;
		//my_list.push(s.str());
		my_cs_input.enter();
		vec_list.push_back(s.str());
		my_cs_input.leave();
	}

	return 0;
}

DWORD WINAPI consume_thread(PVOID param)
{
	std::string temp;
	mw::sleep(5);
	/*while (my_list.pop(temp))
	{
		std::ostringstream s;
		s << (const char*)param << "获取到: " << temp << "\n";
		my_out_list.push(s.str());
	}*/

	while (true)
	{
		//my_cs_input.enter();
		if (my_cs_input.into_section_return([&temp]() {
			if (vec_list.size() == 0) return 1;
			temp = vec_list.back();
			vec_list.pop_back();
			return 0;
			})) break;
		
		//my_cs_input.leave();
		std::ostringstream s;
		s << (const char*)param << "获取到: " << temp << "\n";
		my_cs_output.enter();
		vec_out_list.push_back(s.str());
		my_cs_output.leave();
	}

	
	return 0;
}

#include <vector>

void example_3_2()
{
	std::tcout << _T("正在执行~\n");

	std::shared_ptr<HANDLE> my_handles[6];

	my_handles[0] = mw::safe_handle(mw::c_create_thread(input_thread, (LPVOID)"input_1", nullptr, nullptr, CREATE_SUSPENDED));
	my_handles[1] = mw::safe_handle(mw::c_create_thread(input_thread, (LPVOID)"input_2", nullptr, nullptr, CREATE_SUSPENDED));
	my_handles[2] = mw::safe_handle(mw::c_create_thread(input_thread, (LPVOID)"input_3", nullptr, nullptr, CREATE_SUSPENDED));

	my_handles[3] = mw::safe_handle(mw::c_create_thread(consume_thread, (LPVOID)"consume_1", nullptr, nullptr, CREATE_SUSPENDED));
	my_handles[4] = mw::safe_handle(mw::c_create_thread(consume_thread, (LPVOID)"consume_2", nullptr, nullptr, CREATE_SUSPENDED));
	my_handles[5] = mw::safe_handle(mw::c_create_thread(consume_thread, (LPVOID)"consume_3", nullptr, nullptr, CREATE_SUSPENDED));

	HANDLE my_handles2[6] = { *my_handles[0].get(), *my_handles[1].get()
		,*my_handles[2].get() ,*my_handles[3].get() ,*my_handles[4].get(),*my_handles[5].get() };

	//HANDLE my_handles2[3] = { *my_handles[0].get(),*my_handles[1].get() ,*my_handles[3].get()};

	for (size_t i = 0; i < 6; i++)
	{
		mw::resume_thread(my_handles2[i]);
	}

	// 等待线程返回
	WaitForMultipleObjects(6, my_handles2, true, INFINITE);
	
	std::string temp;
	/*while (my_out_list.pop(temp))
	{
		std::cout << temp << "\n";
	}*/

	for (auto&& i : vec_out_list)
	{
		std::cout << i << "\n";
	}



}

#define CACHE_ALIGN 64
// 强制每个结构体在不同的高速缓存行
struct __declspec(align(CACHE_ALIGN)) CUSTINFO1 {
	DWORD dwCustomerID;     // 大部分时间只读
	wchar_t szName[100];    // 大部分时间只读


	// 强制下面的成员在不同的高速缓存行
	__declspec(align(CACHE_ALIGN))
		int nBalcanceDuw;         // 读写
	FILETIME ftLastOrderDate; // 读写
};

// 强制每个结构体在不同的高速缓存行
struct alignas(CACHE_ALIGN) CUSTINFO2 {
	DWORD a;     // 大部分时间只读
	wchar_t b[100];    // 大部分时间只读


	// 强制下面的成员在不同的高速缓存行
	alignas(CACHE_ALIGN)
		int c;         // 读写
	FILETIME d; // 读写
};


void example_3_3()
{
	std::cout << alignof(CUSTINFO1) << "\n";
	std::cout << alignof(CUSTINFO2) << "\n";
	std::cout << sizeof(CUSTINFO1) << "\n";
	std::cout << sizeof(CUSTINFO2) << "\n";

	//CUSTINFO1 a = { 0 };
	CUSTINFO2 b = { 0 };
	b.a = 1;
	for (size_t i = 0; i < 100; i++)
	{
		b.b[i] = L'a';
	}
	b.c = 8;
	b.d.dwLowDateTime = 5;
	b.d.dwHighDateTime = 6;

}


void example_3_4()
{
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION mymy = nullptr;
	DWORD nums = 0;
	mw::get_logical_processor_information(mymy, nums);

	for (size_t i = 0; i < nums; i++)
	{
		if (mymy[i].Relationship == RelationCache)
		{
			std::cout << mymy[i].Cache.LineSize << "\n";
			std::cout << "\n";
		}
	}

	free(mymy);

}


const int COUNT = 1000;
int sum = 0;

DWORD WINAPI the_first_thread(PVOID pvParam)
{
	sum = 0;
	for (size_t i = 1; i <= COUNT; ++i)
	{
		sum += i;
	}
	std::cout << "线程1结束\n";
	return sum;
}

DWORD WINAPI the_second_thread(PVOID pvParam)
{
	sum = 0;
	for (size_t i = 1; i <= COUNT; ++i)
	{
		sum += i;
	}
	std::cout << "线程2结束\n";
	return sum;
}

CRITICAL_SECTION cs;

DWORD WINAPI the_first_thread_safe(PVOID pvParam)
{
	EnterCriticalSection(&cs);
	sum = 0;
	for (size_t i = 1; i <= COUNT; ++i)
	{
		sum += i;
	}
	std::cout << "线程1结束\n";
	LeaveCriticalSection(&cs);
	return sum;
}

DWORD WINAPI the_second_thread_safe(PVOID pvParam)
{
	EnterCriticalSection(&cs);
	sum = 0;
	for (size_t i = 1; i <= COUNT; ++i)
	{
		sum += i;
	}
	std::cout << "线程2结束\n";
	LeaveCriticalSection(&cs);
	return sum;
}



void example_3_5()
{
	std::vector<HANDLE> my_thread_handles;
	InitializeCriticalSection(&cs);
	my_thread_handles.push_back(mw::c_create_thread(the_first_thread_safe));
	my_thread_handles.push_back(mw::c_create_thread(the_second_thread_safe));

	WaitForMultipleObjects(my_thread_handles.size(), my_thread_handles.data(), true, INFINITE);

	std::cout << "结束！\n";
	std::cout << "sum = " << sum << "\n";

	for (auto&& i : my_thread_handles)
	{
		CloseHandle(i);
	}
}


DWORD WINAPI first_thread_safe(PVOID pvParam)
{
	auto cs = static_cast<mw::critical_section*>(pvParam);
	cs->enter();
	sum = 0;
	for (size_t i = 1; i <= COUNT; ++i)
	{
		sum += i;
	}
	std::cout << "线程1结束\n";
	cs->leave();
	return sum;
}

DWORD WINAPI second_thread_safe(PVOID pvParam)
{
	auto cs = static_cast<mw::critical_section*>(pvParam);
	cs->into_section([]() {
		sum = 0;
		for (size_t i = 1; i <= COUNT; ++i)
		{
			sum += i;
		}
		std::cout << "线程2结束\n";
		});
	return sum;
}


void example_3_6()
{
	std::vector<HANDLE> my_thread_handles;
	mw::critical_section mymy;
	my_thread_handles.push_back(mw::c_create_thread(first_thread_safe, &mymy));
	my_thread_handles.push_back(mw::c_create_thread(second_thread_safe, &mymy));

	WaitForMultipleObjects(my_thread_handles.size(), my_thread_handles.data(), true, INFINITE);

	std::cout << "结束！\n";
	std::cout << "sum = " << sum << "\n";

	for (auto&& i : my_thread_handles)
	{
		CloseHandle(i);
	}
}