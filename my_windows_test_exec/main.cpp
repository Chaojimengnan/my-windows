#include "example_1.h"
#include "example_2.h"
#include "example_3.h"
#include "example_4.h"
#include "example_5.h"
#include "example_6.h"


int main(int argc, char* argv[])
{
#ifdef UNICODE
	std::wcout.imbue(std::locale(""));
#endif // UNICODE
	//example_3();
	//example_4();
	//example_4_1();
	//example_4_2();
	/*auto bf = mw::get_system_time();
	example_4_3();
	auto af = mw::get_system_time();

	std::cout << af - bf;*/
	//example_3_1();
	//example_3_2();
	//example_3_3();
	//example_3_4();
	//example_3_5();
	//example_3_6();
	//example_3_7();
	//example_3_8();
	//example_3_9();
	//example_3_10();
	//example_3_11();
	//example_3_12();
	//example_5();
	//example_5_server();
	//example_5_client();
	//example_2();
	//example_3_13();
	//example_3_14();
	//example_3_15();
	//example_3_16();
	//example_3_17();
	//example_6();
	//example_6_1();
	//std::cin.get();

	//std::cout << "Large page minimum size supports : " << mw::get_large_page_minimum() << "\n";

#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	return 0;
}
