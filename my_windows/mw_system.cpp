#include "mw_system.h"

namespace mw {
	bool set_desktop_wallpaper(const std::string& bmp_file_name)
	{
		return system_parameters(SPI_SETDESKWALLPAPER, 0, const_cast<char*>(bmp_file_name.c_str()));
	}

}//mw