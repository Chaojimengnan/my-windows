#include "my_windows.h"
#include <iostream>

class my_window : public mw::window_class
{
protected:
	using mw::window_class::window_class;
	void on_paint(HWND hwnd) override
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		MoveToEx(hdc, 30, 10, NULL);
		LineTo(hdc, 20, 50);
		LineTo(hdc, 50, 20);
		LineTo(hdc, 10, 20);
		LineTo(hdc, 40, 50);
		LineTo(hdc, 30, 10);

		EndPaint(hwnd, &ps);
	}
};


int main(int argc, char *argv[])
{


	my_window mymy(L"mymy");
	auto gogo = mymy.create();
	gogo->show_window();
	auto kaka = mymy.create();
	kaka->show_window();

	std::string tata;

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	delete gogo;
	delete kaka;
	return msg.wParam;
}