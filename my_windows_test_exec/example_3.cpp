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


void example_3_9()
{
	//auto my_handle = mw::safe_handle(mw::c_create_thread(ThreadFunc));
	//
	//DWORD dw = mw::sync::wait_for_single_object(*my_handle, 500);

	//switch (dw)
	//{
	//case WAIT_OBJECT_0:
	//	// 线程已终止
	//	break;
	//case WAIT_TIMEOUT:
	//	// 在500毫秒内，指定线程并未终止
	//	break;
	//case WAIT_FAILED:
	//	// 调用失败(不合法的句柄？)
	//	break;
	//default:
	//	break;
	//}

	std::vector<HANDLE> handles_vec;
	for (size_t i = 0; i < 3; i++)
		handles_vec.push_back(mw::c_create_thread(ThreadFunc));

	DWORD dw = mw::sync::wait_for_multiple_object(handles_vec.size(), handles_vec.data(), false, 500);

	switch (dw)
	{
	case WAIT_FAILED:
		// 调用失败(不合法的句柄？)
		break;
	case WAIT_TIMEOUT:
		// 在500毫秒内，没有一个指定线程并未终止
		break;
	case WAIT_OBJECT_0 + 0:
		// 线程[0]终止
		break;
	case WAIT_OBJECT_0 + 1:
		// 线程[1]终止
		break;
	case WAIT_OBJECT_0 + 2:
		// 线程[2]终止
		break;
	}

	for (auto& i : handles_vec)
		CloseHandle(i);

}


DWORD WINAPI word_count(PVOID pvParam);
DWORD WINAPI spell_check(PVOID pvParam);
DWORD WINAPI grammar_check(PVOID pvParam);



HANDLE event_file_ready = nullptr;

void example_3_10()
{
	// 创建一个手动重置，初始未触发的事件
	event_file_ready = mw::sync::create_event();

	if (!event_file_ready) return;

	// 生成三个新线程
	std::vector<HANDLE> handle_list;
	handle_list.push_back(mw::c_create_thread(word_count));
	handle_list.push_back(mw::c_create_thread(spell_check));
	handle_list.push_back(mw::c_create_thread(grammar_check));

	// 打开文件，并将内容读入内存
	std::cout << "正在读入文件\n";

	// 触发事件，表示文件内容已经读入内存，使得三个线程能够行动
	std::cout << "文件读入完成，触发事件！\n";

	mw::sync::set_event(event_file_ready);

	mw::sync::wait_for_multiple_object(handle_list.size(), handle_list.data());
	std::cout << "所有进程执行完成！\n";

	CloseHandle(event_file_ready);

	for (auto&& i : handle_list)
	{
		CloseHandle(i);
	}

}



DWORD WINAPI word_count(PVOID pvParam)
{
	// 等待文件数据进入内存
	mw::sync::wait_for_single_object(event_file_ready);

	std::cout << "word_count访问内存块\n";

	return 0;
}

DWORD WINAPI spell_check(PVOID pvParam)
{
	// 等待文件数据进入内存
	mw::sync::wait_for_single_object(event_file_ready);

	// 访问内存块
	std::cout << "spell_check访问内存块\n";

	return 0;
}

DWORD WINAPI grammar_check(PVOID pvParam)
{
	// 等待文件数据进入内存
	mw::sync::wait_for_single_object(event_file_ready);

	// 访问内存块
	std::cout << "grammar_check访问内存块\n";

	return 0;
}


HANDLE waitable_timer = nullptr;


DWORD WINAPI another_thread_wait_timer(PVOID pvParam)
{
	while (true)
	{
		// 等待计时器触发
		mw::sync::wait_for_single_object(waitable_timer);

		// 访问内存块
		std::cout << "线程被计时器激活！\n";
	}
	

	return 0;
}

/// <summary>
/// 将秒数转换为对应的百纳秒数，即1秒为10'000'000百纳秒，计时器以百纳秒为基本单位
/// </summary>
/// <param name="val">对应的秒数</param>
/// <returns>返回对应的纳秒数</returns>
constexpr long long operator"" _second_units(unsigned long long val)
{
	return long long(val * 10'000'000);
}


void APIENTRY TimerAPCRoutine(
	LPVOID lpArgToCompletionRoutine,
	DWORD dwTimerLowValue,
	DWORD dwTimerHighValue
)
{
	std::cout << "mother funker~\n";
}


/// <summary>
/// 测试可等待计时器内核对象
/// </summary>
void example_3_11()
{

	SYSTEMTIME st = {0};
	FILETIME local_time = {0}, utc_time = {0};
	LARGE_INTEGER larget_utc_time = { 0 };

	waitable_timer = mw::sync::create_waitable_timer(0, TIMER_ALL_ACCESS);

	//st.wYear = 2021;	// 年份
	//st.wMonth = 12;		// 十二月
	//st.wDayOfWeek = 0;	// 忽略
	//st.wDay = 6;		// 十二月第六天
	//st.wHour = 10;		// 上午10点
	//st.wMinute = 16;
	//st.wSecond = 0;
	//st.wMilliseconds = 0;

	//SystemTimeToFileTime(&st, &local_time);
	//LocalFileTimeToFileTime(&local_time, &utc_time);

	//larget_utc_time.LowPart = utc_time.dwLowDateTime;
	//larget_utc_time.HighPart = utc_time.dwHighDateTime;

	//auto thread_handle = mw::safe_handle(mw::c_create_thread(another_thread_wait_timer));

	larget_utc_time.QuadPart = -1_second_units;

	if (waitable_timer)										// 第三个参数是6小时循环触发一次
		mw::sync::set_waitable_timer(waitable_timer, &larget_utc_time,5000, TimerAPCRoutine);

	mw::sync::sleep_alertable();

	if(waitable_timer)
		CloseHandle(waitable_timer);
	//std::cin.get();
	
}

volatile LONG index = 0;

VOID CALLBACK work_callback(
	PTP_CALLBACK_INSTANCE Instance,
	PVOID                 Context,
	PTP_WORK              Work
)
{
	auto _index = mw::sync::interlocked_increment(index);
	//std::cout << "猛男归来!" << _index <<  "\n";
	printf_s("猛男归来!%d\n", _index);

	mw::sleep(1000 * _index);

	//std::cout << "完成！" << _index << "\n";
	printf_s("完成!%d\n", _index);
}

/// <summary>
/// 测试线程池工作项，即异步调用函数
/// </summary>
void example_3_12()
{
	// 创建工作项
	auto work_item = mw::create_threadpool_work(work_callback);

	// 提交工作项
	mw::submit_threadpool_work(work_item);
	mw::submit_threadpool_work(work_item);
	mw::submit_threadpool_work(work_item);
	mw::submit_threadpool_work(work_item);

	// 等待完成
	mw::wait_for_threadpool_work_callbacks(work_item);


	//std::cout << "所有提交工作项已经完成！\n";
	printf_s("所有提交工作项已经完成！\n");

	// 释放工作项资源
	mw::close_threadpool_work(work_item);
}