#include "example_1.h"
#include "example_2.h"
#include "example_3.h"
#include "example_4.h"
#include "example_5.h"


int main(int argc, char* argv[])
{
#ifdef UNICODE
	std::wcout.imbue(std::locale(""));
#endif // UNICODE
	//example_3();
	//example_4();
	example_4_1();
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
	//example_5();
	//example_5_server();
	//example_5_client();
	//example_2();

	//std::cin.get();

#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	return 0;
}
