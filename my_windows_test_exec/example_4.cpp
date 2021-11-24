#include "example_4.h"


void example_4()
{
	/*INPUT msg_input = { 0 };
	
	for (size_t i = 0; i < 10; i++)
	{
		mw::user::write_mouse_event(&msg_input, (i + 1) * 100, (i + 1) * 100 , MOUSEEVENTF_MOVE|MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_VIRTUALDESK);
		mw::user::send_input(&msg_input, 1);
		std::tcout << _T("鼠标移动：" << (i + 1) * 100) << _T("\r\n");
		mw::sleep(1000);
	}*/
	
	/*BOOL is_ok = false;
	mw::get_priority_class_boost(mw::get_current_process(), is_ok);
	std::tcout << is_ok << _T("\n");
	mw::set_priority_class_boost(mw::get_current_process(), true);
	mw::get_priority_class_boost(mw::get_current_process(), is_ok);
	std::tcout << is_ok << _T("\n");
	mw::set_priority_class_boost(mw::get_current_process(), false);
	mw::get_priority_class_boost(mw::get_current_process(), is_ok);
	std::tcout << is_ok << _T("\n");*/

	/*mw::process_info mymy;
	mw::create_process(mymy, _T("cmd"));
	mw::set_process_affinity_mask(mw::get_current_process(), 0x1);*/
	
	TEXTMETRIC mymy = {0};
	auto device_handle = mw::gdi::get_dc(NULL);
	mw::gdi::get_text_metrics(device_handle, &mymy);
	mw::gdi::set_text_align(device_handle, TA_RIGHT);
	mw::gdi::text_out(device_handle, 100, 100, _T("猛得要死"));

	mw::gdi::release_dc(NULL, device_handle);
	

	std::cout << "123";


}