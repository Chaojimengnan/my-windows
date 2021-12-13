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
	auto file_handle = mw::create_file(_T("../temp/123.txt"),GENERIC_READ|GENERIC_WRITE, OPEN_EXISTING, 0);

	char* buffer = new char[100]{0};

	OVERLAPPED write_ol = { 0 };
	OVERLAPPED read_ol = { 0 };
	LARGE_INTEGER kaka = {0};
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

	char* buffer = new char[100]{ 0 };

	std::string tata = "Fucking those shit!";
	DWORD li = 0;

	mw::set_file_pointer(file_handle, {0}, FILE_END);

	mw::write_file(file_handle, tata.c_str(), tata.size(), &li);

	std::cout << li << "\n";

	mw::set_file_pointer(file_handle, { 0 }, FILE_BEGIN);

	mw::read_file(file_handle, buffer, 100, &li);

	std::cout << li << "\n";

	std::cout << buffer << "\n";

}