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
	mw::sync::interlocked_exchange_add(g_x, 1);
	return 0;
}


DWORD WINAPI ThreadFunc2(PVOID param)
{
	mw::sync::interlocked_exchange_add(g_x, 1);
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
mw::sync::critical_section my_cs_input;
mw::sync::critical_section my_cs_output;


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
	auto cs = static_cast<mw::sync::critical_section*>(pvParam);
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
	auto cs = static_cast<mw::sync::critical_section*>(pvParam);
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
	mw::sync::critical_section mymy;
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

mw::sync::slimrw_lock my_slim_input;
mw::sync::slimrw_lock my_slim_output;

DWORD WINAPI input_thread_slim(PVOID param)
{
	for (size_t i = 0; i < 3; i++)
	{
		std::ostringstream s;
		s << (const char*)param << ":" << i;
		// 进入独占锁
		my_slim_input.acquire_exclusive();
		vec_list.push_back(s.str());
		my_slim_input.release_exclusive();
	}

	return 0;
}

DWORD WINAPI consume_thread_slim(PVOID param)
{
	std::string temp;
	mw::sleep(5);

	while (true)
	{
		// 进入共享锁(只读)
		if (my_slim_input.into_shared_return([&temp]() {
			if (vec_list.size() == 0) return 1;
			temp = vec_list.back();
			
			return 0;
			})) break;

		// 进入独占锁(写)
		my_slim_input.into_exclusive([] {
			vec_list.pop_back();
		});
		
		std::ostringstream s;
		s << (const char*)param << "获取到: " << temp << "\n";
		// 进入输出独占锁
		my_slim_output.acquire_exclusive();
		vec_out_list.push_back(s.str());
		my_slim_output.release_exclusive();
	}
	return 0;
}

void example_3_7()
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

	for (size_t i = 0; i < 6; i++)
	{
		mw::resume_thread(my_handles2[i]);
	}

	// 等待线程返回
	WaitForMultipleObjects(6, my_handles2, true, INFINITE);

	std::string temp;

	for (auto&& i : vec_out_list)
	{
		std::cout << i << "\n";
	}
}


// 用于生产者写入和消费者读入的中介数据结构
std::vector<std::string> cv_vec_list;

// 用于输出流的读写锁
mw::sync::slimrw_lock output_stream_lock;

// 用于中介数据结构的读写锁
mw::sync::slimrw_lock cv_vec_list_lock;

// 中介数据结构最大值
constexpr int MAX_LIST_SIZE = 20;

// 用于指示中介数据结构已满的条件变量
mw::sync::condition_variable cv_vec_list_full;

// 用于指示中间数据结构已空的条件变量
mw::sync::condition_variable cv_vec_list_empty;

/// <summary>
/// 生产者线程
/// </summary>
DWORD WINAPI input_thread_slim_cv(PVOID param)
{
	while (true)
	{
		// 进入独占模式的slim锁
		cv_vec_list_lock.into_exclusive([] {

			// 判断是否已满
			while (cv_vec_list.size() == MAX_LIST_SIZE)
			{
				// 唤醒消费者，即：中介数据满啦！！
				cv_vec_list_full.wake_all();

				// 然后睡眠，等待消费者调用cv_vec_list_empty.wake_all
				output_stream_lock.into_exclusive([] {
					std::cout << "\n生产者睡了\n\n";
					});
				cv_vec_list_empty.sleep_slimrw(cv_vec_list_lock);
				output_stream_lock.into_exclusive([] {
					std::cout << "\n生产者醒了\n\n";
					});
			}

			output_stream_lock.into_exclusive([] {
				std::cout << "生产者生产了一个数据\n";
				});
			cv_vec_list.push_back("一份数据"); // 模拟生产一个数据

			});
	}
	return 0;
}

/// <summary>
/// 消费者线程
/// </summary>
DWORD WINAPI consume_thread_slim_cv(PVOID param)
{
	while (true)
	{
		// 进入独占模式的slim锁
		cv_vec_list_lock.into_exclusive([] {

			// 判断是否已空
			while (cv_vec_list.size() == 0)
			{
				// 唤醒生产者，即：中介数据空啦！！
				cv_vec_list_empty.wake_all();

				// 然后睡眠，等待消费者调用cv_vec_list_full.wake_all
				output_stream_lock.into_exclusive([] {
					std::cout << "\n消费者睡了\n\n";
					});
				cv_vec_list_full.sleep_slimrw(cv_vec_list_lock);
				output_stream_lock.into_exclusive([] {
					std::cout << "\n消费者醒了\n\n";
					});
			}

			output_stream_lock.into_exclusive([] {
				std::cout << "消费者消费了一个数据\n";
				});
			cv_vec_list.pop_back();	// 模拟消费一个数据

			});
	}
	return 0;
}

/// <summary>
/// 该例子展示使用条件变量和读写锁，当生产者线程将中介数据结构写入到最大值时，唤醒消费者，然后自己进入睡眠。
/// 当消费者将中介数据结构读完之后，唤醒生产者，然后自己进入睡眠
/// </summary>
void example_3_8()
{
	std::vector<HANDLE> my_thread_handles;
	my_thread_handles.push_back(mw::c_create_thread(input_thread_slim_cv));
	my_thread_handles.push_back(mw::c_create_thread(consume_thread_slim_cv));
	my_thread_handles.push_back(mw::c_create_thread(input_thread_slim_cv));
	my_thread_handles.push_back(mw::c_create_thread(consume_thread_slim_cv));
	my_thread_handles.push_back(mw::c_create_thread(input_thread_slim_cv));
	my_thread_handles.push_back(mw::c_create_thread(consume_thread_slim_cv));


	std::cin.get();


	for (auto& i : my_thread_handles)
	{
		CloseHandle(i);
	}

}