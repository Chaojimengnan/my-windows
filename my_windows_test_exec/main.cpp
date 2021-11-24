#include "example_1.h"
#include "example_2.h"
#include "example_3.h"
#include "example_4.h"


int main(int argc, char* argv[])
{
#ifdef UNICODE
	std::wcout.imbue(std::locale(""));
#endif // UNICODE

	//example_1();
	//example_3();
	example_4();


#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	return 0;
}