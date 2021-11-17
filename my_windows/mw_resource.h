#pragma once

namespace mw {
namespace user {

	/// <summary>
	/// 从模块内部获取加载一个图像，并返回其资源句柄(该函数有点问题，等封装了绘制函数再测试)
	/// </summary>
	/// <remarks>封装自LoadImageA</remarks>
	/// <param name="handle_modlue">模块的实例句柄</param>
	/// <param name="resource_id">资源ID</param>
	/// <param name="type">资源类型，可以是`IMAGE_BITMAP`，`IMAGE_CURSOR`，`IMAGE_ICON`</param>
	/// <param name="image_x">以像素为单位的宽度，若为0，并且is_default_size为true，则使用系统默认宽度，若为false是资源实际宽度</param>
	/// <param name="image_y">以像素为单位的高度，若为0，并且is_default_size为true，则使用系统默认高度，若为false是资源实际高度</param>
	/// <param name="is_default_size">若宽度或高度设为0，为true使用系统默认大小，否则使用资源实际大小</param>
	/// <param name="is_monochrome">是否是单色，若false则是默认颜色</param>
	/// <param name="is_transparent">是否透明，若图像颜色深度大于8不要使用此选项</param>
	/// <param name="is_shared">是否共享，类似shared_ptr，当没有使用该资源时系统自动回收</param>
	/// <returns>若成功，返回对应图像的句柄，否则返回NULL</returns>
	MW_API HANDLE load_internal_image(HINSTANCE handle_modlue, 
		int resource_id, UINT type, int image_x = 0, int image_y = 0, bool is_default_size = false,
		bool is_monochrome = false, bool is_transparent = false, bool is_shared = true);


	/// <summary>
	/// 从外部独立文件中获取加载一个图像，并返回其资源句柄(该函数有点问题，等封装了绘制函数再测试)
	/// </summary>
	/// <remarks>封装自LoadImageA</remarks>
	/// <param name="file_path">图像文件的路径</param>
	/// <param name="type">资源类型，可以是`IMAGE_BITMAP`，`IMAGE_CURSOR`，`IMAGE_ICON`</param>
	/// <param name="image_x">以像素为单位的宽度，若为0，并且is_default_size为true，则使用系统默认宽度，若为false是资源实际宽度</param>
	/// <param name="image_y">以像素为单位的高度，若为0，并且is_default_size为true，则使用系统默认高度，若为false是资源实际高度</param>
	/// <param name="is_default_size">若宽度或高度设为0，为true使用系统默认大小，否则使用资源实际大小</param>
	/// <param name="is_monochrome">是否是单色，若false则是默认颜色</param>
	/// <param name="is_transparent">是否透明，若图像颜色深度大于8不要使用此选项</param>
	/// <param name="is_shared">是否共享，类似shared_ptr，当没有使用该资源时系统自动回收</param>
	/// <returns>若成功，返回对应图像的句柄，否则返回NULL</returns>
	MW_API HANDLE load_external_image(const std::tstring& file_path, UINT type, 
		int image_x, int image_y, bool is_default_size = false, bool is_monochrome = false,
		bool is_transparent = false, bool is_shared = true);

};//user
};//mw