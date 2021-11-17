#include "mw_resource.h"


namespace mw {
namespace user {
	HANDLE load_internal_image(HINSTANCE handle_modlue, int resource_id, UINT type, int image_x, int image_y, bool is_default_size, bool is_monochrome, bool is_transparent, bool is_shared)
	{
		UINT flag = 0;
		flag |= static_cast<UINT>(is_monochrome);
		if (is_transparent)
			flag |= LR_LOADTRANSPARENT;
		if (is_shared)
			flag |= LR_SHARED;
		if (is_default_size)
			flag |= LR_DEFAULTSIZE;

		auto resource_handle = LoadImage(handle_modlue, MAKEINTRESOURCE(resource_id), type, image_x, image_y, flag);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return resource_handle;
	}
	HANDLE load_external_image(const std::tstring& file_path, UINT type, int image_x, int image_y, bool is_default_size, bool is_monochrome, bool is_transparent, bool is_shared)
	{
		UINT flag = 0;
		flag |= static_cast<UINT>(is_monochrome);
		if (is_transparent)
			flag |= LR_LOADTRANSPARENT;
		if (is_shared)
			flag |= LR_SHARED;
		if (is_default_size)
			flag |= LR_DEFAULTSIZE;
		flag |= LR_LOADFROMFILE;

		auto resource_handle = LoadImage(NULL, file_path.c_str(), type, image_x, image_y, flag);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return resource_handle;
	}
};//user
};//mw