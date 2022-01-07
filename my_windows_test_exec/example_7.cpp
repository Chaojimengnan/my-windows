#include "stdafx.h"
#include "example_7.h"


#include "my_windows_dll/output.h"

#include "example_6.h"

void print()
{
	std::cout << "hehe\n";
}

// DLL初次尝试
void example_7()
{
	print();
	//std::cout << add(1, 2) << "\n";

}