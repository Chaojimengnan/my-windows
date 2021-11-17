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
	inline bool system_parameters(UINT action_type, UINT extra_param = 0,
		PVOID extra_pointer = nullptr, UINT update_and_broadcast_option = 0)
	{
		auto val = SystemParametersInfo(action_type,
			extra_param, extra_pointer, update_and_broadcast_option);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 设置桌面窗口的背景
	/// </summary>
	/// <param name="bmp_file_name">图片文件路径</param>
	/// <returns>操作是否成功</returns>
	MW_API bool set_desktop_wallpaper(const std::tstring& wallpaper_file_name);


	/// <summary>
	/// 获取指定的系统度量(注意所有的返回的数字均已像素为单位，SM_CX*表示宽度，SM_CY*表示高度)
	/// </summary>
	/// <param name="index">指定索引</param>
	/// <returns>如果函数成功，返回请求的系统度量，若失败则返回0</returns>
	inline int get_system_metrics(int index)
	{
		auto val = GetSystemMetrics(index);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 获取指定显示元素的当前颜色。显示元素是出现在系统显示屏幕上的窗口和显示的组成部分。
	/// </summary>
	/// <param name="index">指定哪个显示元素的颜色会被获取，它是一个以COLOR_开头的宏，WIN10或以上版本有许多不支持的宏，详见文档</param>
	/// <returns>返回给定元素颜色的RGB值，若index超出范围，则返回0，但是0也是一个有效的RGB，所以无法使用该函数确认是否有效</returns>
	inline DWORD get_system_color(int index)
	{
		auto val = GetSysColor(index);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 设置指定显示元素的颜色，使用该函数做的修改系统不会保存，仅在当前会话有效
	/// </summary>
	/// <param name="index_nums">index_array和color_array大小</param>
	/// <param name="index_array">index数组，用于指定修改哪个显示元素，其是以COLOR_开头的宏</param>
	/// <param name="color_array">与index数组对应的color数组，用于指定对应索引的显示元素的新颜色</param>
	/// <returns>操作是否成功</returns>
	inline bool set_system_color(int index_nums, const INT* index_array, const COLORREF* color_array)
	{
		auto val = SetSysColors(index_nums, index_array, color_array);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}

	/// <summary>
	/// 获取指定显示元素的当前颜色的画刷。显示元素是出现在系统显示屏幕上的窗口和显示的组成部分。
	/// </summary>
	/// <remarks>系统颜色画笔归系统所有，因此您无需销毁它们。虽然您不需要删除GetSysColorBrush返回的逻辑画笔，但调用DeleteObject不会造成任何损害。</remarks>
	/// <param name="index">指定哪个显示元素的颜色会被获取，它是一个以COLOR_开头的宏，WIN10或以上版本有许多不支持的宏，详见文档</param>
	/// <returns>返回给定元素颜色的画刷，否则返回NULL</returns>
	inline HBRUSH get_system_color_brush(int index)
	{
		auto val = GetSysColorBrush(index);
		GET_ERROR_MSG_OUTPUT(std::tcout);
		return val;
	}


}//mw