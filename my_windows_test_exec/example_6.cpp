#include "example_6.h"

constexpr size_t operator ""_GB(size_t t)
{
	return t * 1024 * 1024 * 1024;
}

void example_6()
{
	char* mymy = new char[3_GB];

	std::cout << (size_t*)mymy << "\n";

	/*for (size_t i = 0; i < 3_GB; i++)
	{
		mymy[i] = 'a';
	}
	mymy[3_GB - 1] = '\0';
	
	std::cout << mymy << "\n";*/

}