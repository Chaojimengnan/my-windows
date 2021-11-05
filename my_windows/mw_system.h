#pragma once

namespace mw {

	/// <summary>
	/// 获取或设置一个系统参数值，该函数也可以在设置参数时更新用户档案(user profile),注意该函数使用ANSI版本
	/// </summary>
	/// <param name="action_type">执行什么操作，指定是获取或设置，以及哪种参数</param>
	/// <param name="extra_param">该参数的作用依赖于action_type，请看文档</param>
	/// <param name="extra_pointer">该参数的作用依赖于action_type，请看文档</param>
	/// <param name="update_and_broadcast_option">如果是设置一个系统参数，该选项指定是否更新用户档案，以及是否广播WM_SETTINGCHANGE消息</param>
	/// <returns></returns>
	MW_API bool system_parameters(UINT action_type, UINT extra_param = 0, 
		PVOID extra_pointer = NULL, UINT update_and_broadcast_option = 0);

	/// <summary>
	/// 设置桌面窗口的背景
	/// </summary>
	/// <param name="bmp_file_name">图片文件路径</param>
	/// <returns>操作是否成功</returns>
	MW_API bool set_desktop_wallpaper(const std::string& wallpaper_file_name);



}//mw