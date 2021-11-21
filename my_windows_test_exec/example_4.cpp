#include "example_4.h"


void example_4()
{
	INPUT msg_input = { 0 };
	
	for (size_t i = 0; i < 10; i++)
	{
		mw::user::write_mouse_event(&msg_input, (i + 1) * 100, (i + 1) * 100 , MOUSEEVENTF_MOVE|MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_VIRTUALDESK);
		mw::user::send_input(&msg_input, 1);
		std::tcout << _T("鼠标移动：" << (i + 1) * 100) << _T("\r\n");
		mw::sleep(1000);
	}
	
}