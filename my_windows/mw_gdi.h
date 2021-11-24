#pragma once

namespace mw {
namespace gdi {

	// TODO:
	// CreateDC，DeleteDC，CreateCompatibleDC，DeleteDC，CreateMetaFile，CloseMetaFile
	// GetWindowDC，

	/// <summary>
	/// 准备指定的窗口绘画和填充一个PAINTSTRUCT结构有关绘画的信息。该函数只应该用于响应WM_PAINT消息，并且每调用该函数都要配对一个EndPaint
	/// </summary>
	/// <remarks>
	/// BeginPaint函数自动设置设备上下文的剪切区域，以排除更新区域之外的任何区域。 
	/// 更新区域由InvalidateRect或InvalidateRgn函数设置，并由系统在调整大小、移动、创建、滚动或影响客户区的任何其他操作之后设置。 
	/// 如果更新区域被标记为擦除，BeginPaint将发送一个WM_ERASEBKGND消息到窗口。 其他事项请看文档 
	/// </remarks>
	/// <param name="window_handle">指定要绘制的窗口</param>
	/// <param name="paint_struct">[out]一个获取绘制信息的PAINTSTRUCT结构体</param>
	/// <returns>若成功，返回指定窗口的显示设备上下文句柄(HDC)，失败返回NULL</returns>
	inline HDC begin_paint(HWND window_handle, PAINTSTRUCT& paint_struct)
	{
		auto val = BeginPaint(window_handle, &paint_struct);
		GET_ERROR_MSG_OUTPUT_NORMAL(std::tcout, val, 0);
		return val;
	}

	/// <summary>
	/// EndPaint函数标记指定窗口中绘制的结束。这个函数在每次调用BeginPaint函数时都是必需的，但只在绘制完成之后。该函数释放BeginPaint获取到的DC
	/// </summary>
	/// <param name="window_handle">已重绘的窗口的句柄</param>
	/// <param name="paint_struct">包含由BeginPaint获取的绘画信息的PAINTSTRUCT结构引用</param>
	/// <returns>返回值始终为非零的</returns>
	inline BOOL end_paint(HWND window_handle, const PAINTSTRUCT& paint_struct)
	{
		return EndPaint(window_handle, &paint_struct);
	}

	/// <summary>
	/// 该函数获取指定窗口的客户区或整个屏幕的设备上下文(DC)句柄，它可以是公共DC，类DC或私有DC。若是公共DC，则需要调用ReleaseDC释放它，否则不用释放。
	/// </summary>
	/// <remarks>如果是公共DC, 该函数在每次获取DC时都会为其分配默认属性，而对于类和私有DC,则保持先前分配的属性不变。
	/// 注意，DC的句柄在任何时候只能由单个线程使用。公共DC的数量仅受可用内存的限制。若要获取私有DC，你应该在窗口类指定私有DC样式。其他事项请看文档。
	/// </remarks>
	/// <param name="window_handle">指定要获取DC的窗口，若为NULL，该函数获取整个屏幕的DC</param>
	/// <returns>若成功返回指定窗口的客户区，若失败返回NULL</returns>
	inline HDC get_dc(HWND window_handle)
	{
		auto val = GetDC(window_handle);
		GET_ERROR_MSG_OUTPUT_NORMAL(std::tcout, val, 0);
		return val;
	}

	/// <summary>
	/// 释放一个设备上下文(DC)，释放它供其他应用程序使用。该函数应该只用于释放公共DC和窗口DC，它对类或私有DC没有影响。调用该函数的线程必须是调用GetDC的线程
	/// </summary>
	/// <remarks>对于GetWindowDC函数的每次调用，以及对于获取公共DC的GetDC函数的每次调用，应用程序都必须调用ReleaseDC函数。
	/// 应用程序不能使用ReleaseDC函数来释放通过调用CreateDC函数创建的DC; 而是必须使用DeleteDC函数。 ReleaseDC必须从调用GetDC的同一个线程调用。
	/// 其他事项请看文档
	/// </remarks>
	/// <param name="window_handle">要被释放DC的窗口句柄</param>
	/// <param name="device_context">要被释放的DC句柄</param>
	/// <returns>该返回值指定DC是否被释放，若为1则是释放，若没有释放返回0</returns>
	inline int release_dc(HWND window_handle, HDC device_context)
	{
		auto val = ReleaseDC(window_handle, device_context);
		GET_ERROR_MSG_OUTPUT_NORMAL(std::tcout, val, 0);
		return val;
	}

	/// <summary>
	/// 在指定的矩形中绘制格式化文本
	/// </summary>
	/// <param name="device_context">设备上下文句柄</param>
	/// <param name="text">要输出的文本</param>
	/// <param name="text_rect">包含要在其中格式化文本的矩形（在逻辑坐标中）的RECT结构，可以使用GetClientRect获得</param>
	/// <param name="format">格式化文本的方法，它应该是DT_开头的宏的组合</param>
	/// <returns>若失败返回0，若成功返回以逻辑单位表示的文本高度，如果指定了DT_VCENTER或DT_BOTTOM，则返回值是距text_rect->top绘制文本底部的偏移量</returns>
	inline int draw_text(HDC device_context, const std::tstring& text, RECT& text_rect, UINT format = DT_CENTER)
	{
		auto val = DrawText(device_context, text.c_str(), -1, &text_rect, format);
		GET_ERROR_MSG_OUTPUT_NORMAL(std::tcout, val, 0);
		return val;
	}

	/// <summary>
	/// 在指定位置绘制一个字符串，使用当前选择的字体，背景颜色，和文本颜色
	/// </summary>
	/// <param name="device_context">设备上下文句柄</param>
	/// <param name="x">系统用于对齐字符串的参考点的 x 坐标（以逻辑坐标表示）</param>
	/// <param name="y">系统用于对齐字符串的参考点的 y 坐标（以逻辑坐标表示）</param>
	/// <param name="text">要输出的文本</param>
	/// <returns>操作是否成功</returns>
	inline bool text_out(HDC device_context, int x, int y, const std::tstring& text)
	{
		auto val = TextOut(device_context, x, y, text.c_str(), text.size());
		GET_ERROR_MSG_OUTPUT_NORMAL(std::tcout, val, 0);
		return val;
	}

	/// <summary>
	/// 将当前选择字体的度量填充指定的TEXTMETRIC结构体中，这个被选择的字体有传入设备上下文句柄决定
	/// </summary>
	/// <param name="device_context">设备上下文句柄</param>
	/// <param name="text_metric">[out]一个用于接收文本度量的结构体</param>
	/// <returns>操作是否成功</returns>
	inline bool get_text_metrics(HDC device_context, LPTEXTMETRIC text_metric)
	{
		auto val = GetTextMetrics(device_context, text_metric);
		GET_ERROR_MSG_OUTPUT_NORMAL(std::tcout, val, 0);
		return val;
	}

	/// <summary>
	/// 为指定的设备上下文设置文本对齐标志
	/// </summary>
	/// <param name="device_context">设备上下文句柄</param>
	/// <param name="text_align">文本对齐标志，它是以TA_或VTA_开头的宏的组合，具体看文档</param>
	/// <returns>若成功，返回之前的文本对齐标志，若失败返回GDI_ERROR</returns>
	inline UINT set_text_align(HDC device_context, UINT text_align)
	{
		auto val = SetTextAlign(device_context, text_align);
		GET_ERROR_MSG_OUTPUT_NORMAL(std::tcout, val, GDI_ERROR);
		return val;
	}

	/// <summary>
	/// 获取指定的设备上下文的文本对齐标志
	/// </summary>
	/// <param name="device_context">设备上下文句柄</param>
	/// <returns>若成功，返回文本对齐标志，它是以TA_或VTA_开头的宏的组合，若失败返回GDI_ERROR</returns>
	inline UINT get_text_alian(HDC device_context)
	{
		auto val = GetTextAlign(device_context);
		GET_ERROR_MSG_OUTPUT_NORMAL(std::tcout, val, GDI_ERROR);
		return val;
	}

	/// <summary>
	/// 在指定坐标下设置一个指定颜色的像素点，如果像素坐标位于当前剪切区域之外，该函数将失败。该函数并非所有设备都支持
	/// </summary>
	/// <param name="device_context">设备上下文的句柄</param>
	/// <param name="x">要设置的点的 x 坐标（以逻辑单位表示）</param>
	/// <param name="y">要设置的点的 y 坐标（以逻辑单位表示）</param>
	/// <param name="color">用于绘制点的颜色。要创建COLORREF颜色值，请使用RGB宏</param>
	/// <returns>如果函数成功，则返回值是函数将像素设置为的 RGB 值，若失败返回-1</returns>
	inline UINT set_pixel(HDC device_context, int x, int y, COLORREF color)
	{
		auto val = SetPixel(device_context, x, y, color);
		GET_ERROR_MSG_OUTPUT_NORMAL(std::tcout, val, -1);
		return val;
	}

	/// <summary>
	/// 指定坐标下设置一个指定颜色近似的像素点，该点必须位于剪切区域和设备表面的可见部分，SetPixelV比SetPixel更快，因为它不需要返回实际绘制的点的颜色值
	/// </summary>
	/// <param name="device_context">设备上下文的句柄</param>
	/// <param name="x">要设置的点的 x 坐标（以逻辑单位表示）</param>
	/// <param name="y">要设置的点的 y 坐标（以逻辑单位表示）</param>
	/// <param name="color">用于绘制点的颜色。要创建COLORREF颜色值，请使用RGB宏</param>
	/// <returns>操作是否成功</returns>
	inline bool set_pixel_v(HDC device_context, int x, int y, COLORREF color)
	{
		auto val = SetPixelV(device_context, x, y, color);
		GET_ERROR_MSG_OUTPUT_NORMAL(std::tcout, val, 0);
		return val;
	}

	/// <summary>
	/// 获取指定坐标的像素的颜色值，像素必须在当前剪辑区域的边界内，并非所有设备都支持该函数
	/// </summary>
	/// <param name="device_context">设备上下文的句柄</param>
	/// <param name="x">要设置的点的 x 坐标（以逻辑单位表示）</param>
	/// <param name="y">要设置的点的 y 坐标（以逻辑单位表示）</param>
	/// <returns>返回指定像素的RGB值，如果像素在当前裁剪区域之外，则返回值为 CLR_INVALID</returns>
	inline COLORREF get_pixel(HDC device_context, int x, int y)
	{
		auto val = GetPixel(device_context, x, y);
		GET_ERROR_MSG_OUTPUT_NORMAL(std::tcout, val, CLR_INVALID);
		return val;
	}

};//gdi
};//mw

