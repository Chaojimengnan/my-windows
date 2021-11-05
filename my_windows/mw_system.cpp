#include "mw_system.h"
#include "mw_utility.h"

namespace mw {

	bool system_parameters(UINT action_type, UINT extra_param, PVOID extra_pointer, UINT update_and_broadcast_option)
	{
		auto is_ok = SystemParametersInfoA(action_type, extra_param, extra_pointer, update_and_broadcast_option);
		GET_ERROR_MSG_OUTPUT(std::cout)
		return is_ok;
	}

	bool set_desktop_wallpaper(const std::string& bmp_file_name)
	{
		return system_parameters(SPI_SETDESKWALLPAPER, 0, const_cast<char*>(bmp_file_name.c_str()));
	}

}//mw