#pragma once

namespace mw::gdi {

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
		GET_ERROR_MSG_OUTPUT_NORMAL(val, 0);
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
	/// 获取指定窗口的客户区或整个屏幕的设备上下文(DC)句柄，它可以是公共DC，类DC或私有DC。若是公共DC，则需要调用ReleaseDC释放它，否则不用释放。注意，公共DC做的改变会在释放后丢失。
	/// </summary>
	/// <remarks>如果是公共DC, 该函数在每次获取DC时都会为其分配默认属性，而对于类和私有DC,则保持先前分配的属性不变。
	/// 注意，DC的句柄在任何时候只能由单个线程使用。公共DC的数量仅受可用内存的限制。若要获取私有DC，你应该在窗口类指定私有DC样式。其他事项请看文档。
	/// </remarks>
	/// <param name="window_handle">指定要获取DC的窗口，若为NULL，该函数获取整个屏幕的DC</param>
	/// <returns>若成功返回指定窗口的客户区，若失败返回NULL</returns>
	inline HDC get_dc(HWND window_handle)
	{
		auto val = GetDC(window_handle);
		GET_ERROR_MSG_OUTPUT_NORMAL(val, 0);
		return val;
	}

	/// <summary>
	/// 获取用于整个窗口的设备上下文，包括非客户区，其DC原点是窗口的左上角而不是客户区。该函数每次获取DC时都会为DC分配默认属性，以前的属性丢失
	/// </summary>
	/// <param name="window_handle">要获取的设备上下文的窗口句柄，若为NULL，获取整个屏幕的DC(若有多个屏幕，获取主屏幕DC)</param>
	/// <returns>若成功，返回指定窗口的设备上下文的句柄，若失败返回NULL</returns>
	inline HDC get_window_dc(HWND window_handle)
	{
		auto val = GetWindowDC(window_handle);
		GET_ERROR_MSG_OUTPUT_NORMAL(val, 0);
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
		GET_ERROR_MSG_OUTPUT_NORMAL(val, 0);
		return val;
	}

	/// <summary>
	/// 为指定名字的设备创建一个设备上下文，可以使用EnumDisplayDevices获取有效的显示设备名字
	/// </summary>
	/// <remarks>该函数创建的DC，应该使用DeleteDC销毁，而不是ReleaseDC</remarks>
	/// <param name="device_name">它是正在被使用的指定输出设备的名字，若driver_name为DISPLAY或特定显示设备的名称，该参数也必须为""或相同名称</param>
	/// <param name="driver_name">它可以是DISPLAY或指定显示设备的名字，对于打印机，它应该是""</param>
	/// <param name="pdm">包含设备驱动程序的特定于设备的初始化数据的DEVMODE结构的指针，若driver_name为DISPLAY，该参数必须为nullptr</param>
	/// <returns>若函数成功，则返回值是指定设备的DC的句柄</returns>
	inline HDC create_dc(const std::tstring& device_name = _T(""),
		const std::tstring& driver_name = _T("DISPLAY"), const DEVMODE* pdm = nullptr)
	{
		auto val = CreateDC(tstring_to_pointer(driver_name), tstring_to_pointer(device_name), nullptr, pdm);
		GET_ERROR_MSG_OUTPUT_NORMAL(val, 0);
		return val;
	}

	/// <summary>
	/// 删除指定的设备上下文，该函数不能使用删除GetDC获取的DC
	/// </summary>
	/// <param name="device_context">设备上下文的句柄</param>
	/// <returns>操作是否成功</returns>
	inline bool delete_dc(HDC device_context)
	{
		auto val = DeleteDC(device_context);
		GET_ERROR_MSG_OUTPUT_NORMAL(val, 0);
		return val;
	}

	/// <summary>
	/// 创建一个与指定设备兼容的内存设备上下文，若其设备上下文为nullptr，则创建一个与应用程序当前屏幕兼容的内存DC。使用DeleteDC删除内存DC
	/// </summary>
	/// <remarks>内存 DC 只存在于内存中，它将位图数据存入内存中，其他事项请看文档</remarks>
	/// <param name="device_context">现有 DC 的句柄。如果此句柄为nullptr，则该函数会创建一个与应用程序当前屏幕兼容的内存 DC</param>
	/// <returns>若成功，返回内存DC的句柄，否则返回NULL</returns>
	inline HDC create_compatible_dc(HDC device_context = nullptr)
	{
		auto val = CreateCompatibleDC(device_context);
		GET_ERROR_MSG_OUTPUT_NORMAL(val, 0);
		return val;
	}

	/// <summary>
	/// 创建指定设备的信息上下文，该IC提供了最快的方式获取设备消息，不过GDI绘制函数不接受IC句柄，它应该只用于获取指定设备的信息。使用DeleteDC销毁IC
	/// </summary>
	/// <param name="device_name">指定正在使用的特定输出设备的名称</param>
	/// <param name="driver_name">指向指定设备驱动程序名称</param>
	/// <param name="pdm">包含设备驱动程序的特定于设备的初始化数据的DEVMODE结构的指针，若driver_name为DISPLAY，该参数必须为nullptr</param>
	/// <returns>若函数成功，则返回值是指定设备的IC句柄</returns>
	inline HDC create_ic(const std::tstring& device_name = _T(""),
		const std::tstring& driver_name = _T(""), const DEVMODE* pdm = nullptr)
	{
		auto val = CreateIC(tstring_to_pointer(driver_name), tstring_to_pointer(device_name), nullptr, pdm);
		GET_ERROR_MSG_OUTPUT_NORMAL(val, 0);
		return val;
	}

	/// <summary>
	/// 获取指定设备上下文当前选择的指定类型的图像对象句柄，该函数获取指定图形对象句柄，GetObject获取指定图形对象句柄的信息
	/// </summary>
	/// <param name="device_handle">指定设备上下文</param>
	/// <param name="type">要获取的图形对象的类型，它应该是OBJ_开头的宏的其中一个</param>
	/// <returns>若成功，返回指定图形对象句柄，否则返回NULL</returns>
	inline HGDIOBJ get_current_object(HDC device_handle, UINT type)
	{
		auto val = GetCurrentObject(device_handle, type);
		GET_ERROR_MSG_OUTPUT_NORMAL(val, 0);
		return val;
	}

	/// <summary>
	/// 获取指定图形对象的信息，该函数将其数据写入堆中，并由智能指针管理，具体buffer的结构，请看文档，它应该是一个特定的结构体
	/// </summary>
	/// <param name="graphics_object">指定图形对象，它可以是逻辑位图、画刷、字体、调色板、画笔，或使用CreateDIBSection创建的设备无关位图</param>
	/// <param name="buffer">[out]它应该是一个空的智能指针，用于接收指定图形对象的信息，无须手动释放，其buffer的内容依赖于图形对象的类型，具体看文档</param>
	/// <returns>若成功，返回存储到buffer中的字节数，若失败返回0</returns>
	inline int get_object(HANDLE graphics_object, std::shared_ptr<void>& buffer)
	{
		auto size = GetObject(graphics_object, 0, nullptr);
		buffer.reset(_aligned_malloc(size, 4), [](void* p) {
			_aligned_free(p);
			});
		auto val = GetObject(graphics_object, size, buffer.get());
		GET_ERROR_MSG_OUTPUT_NORMAL(val, 0);
		return val;
	}

	/// <summary>
	/// 给指定设备上下文选择一个指定图形对象，新对象替换相同类型的先前对象。应用程序不能一次将一个位图选择到多个DC中
	/// </summary>
	/// <param name="device_handle">指定设备上下文</param>
	/// <param name="graphics_object">要选择的图形对象，它必须是特定函数创建的位图，画刷，字体，画笔，和区域，具体请看文档</param>
	/// <returns>若对象不是区域并且函数成功，返回被替换对象的句柄，否则是NULL，如果选择对象是一个区域并且函数成功返回特定宏，否则是HGDI_ERROR</returns>
	inline HGDIOBJ select_object(HDC device_handle, HGDIOBJ graphics_object)
	{
		auto val = SelectObject(device_handle, graphics_object);
		GET_ERROR_MSG_OUTPUT_NORMAL(val, 0);
		return val;
	}

	/// <summary>
	/// 删除一个逻辑画笔，画刷，字体，位图，区域或调色板，释放所有与该图形对象管理的资源，删除后，该对象句柄不再有效。若指定图形对象仍被选入DC中，请勿删除
	/// </summary>
	/// <param name="graphics_object">画笔，画刷，字体，位图，区域或调色板的图形对象句柄</param>
	/// <returns>操作是否成功</returns>
	inline bool delete_object(HGDIOBJ graphics_object)
	{
		auto val = DeleteObject(graphics_object);
		GET_ERROR_MSG_OUTPUT_NORMAL(val, 0);
		return val;
	}

	/// <summary>
	/// 获取其中一个系统存储的画笔，画刷，字体或调色板的句柄，返回的图形句柄不需要调用DeleteObject删除，但是调用了也没关系
	/// </summary>
	/// <param name="index">存储的图形对象的类型，具体值是一个宏，请看文档</param>
	/// <returns>若函数成功，返回所请求的逻辑对象句柄，若失败返回NULL</returns>
	inline HGDIOBJ get_stock_object(int index)
	{
		auto val = GetStockObject(index);
		GET_ERROR_MSG_OUTPUT_NORMAL(val, 0);
		return val;
	}

	/// <summary>
	/// 获取指定设备上下文的当前画笔颜色
	/// </summary>
	/// <param name="device_handle">指定设备上下文</param>
	/// <returns>若成功，返回当前画笔颜色的COLORREF值，若失败返回CLR_INVALID</returns>
	inline COLORREF get_dc_pen_color(HDC device_handle)
	{
		auto val = GetDCPenColor(device_handle);
		GET_ERROR_MSG_OUTPUT_NORMAL(val, CLR_INVALID);
		return val;
	}

	/// <summary>
	/// 将指定设备上下文的当前画笔颜色设置为指定值，如果设备无法表示指定的颜色值，则将颜色设置为最接近的物理颜色
	/// </summary>
	/// <param name="device_handle">指定设备上下文</param>
	/// <param name="pen_color">新的画笔颜色</param>
	/// <returns>若成功，返回之前的DC画笔颜色，若失败，返回 CLR_INVALID</returns>
	inline COLORREF set_dc_pen_color(HDC device_handle, COLORREF pen_color)
	{
		auto val = SetDCPenColor(device_handle, pen_color);
		GET_ERROR_MSG_OUTPUT_NORMAL(val, CLR_INVALID);
		return val;
	}

	/// <summary>
	/// 获取指定设备上下文的当前画刷颜色
	/// </summary>
	/// <param name="device_handle">指定设备上下文</param>
	/// <returns>若成功，返回当前画刷颜色的COLORREF值，若失败返回CLR_INVALID</returns>
	inline COLORREF get_dc_brush_color(HDC device_handle)
	{
		auto val = GetDCBrushColor(device_handle);
		GET_ERROR_MSG_OUTPUT_NORMAL(val, CLR_INVALID);
		return val;
	}

	/// <summary>
	/// 将指定设备上下文的当前画刷颜色设置为指定值，如果设备无法表示指定的颜色值，则将颜色设置为最接近的物理颜色
	/// </summary>
	/// <param name="device_handle">指定设备上下文</param>
	/// <param name="pen_color">新的画刷颜色</param>
	/// <returns>若成功，返回之前的DC画刷颜色，若失败，返回 CLR_INVALID</returns>
	inline COLORREF set_dc_brush_color(HDC device_handle, COLORREF brush_color)
	{
		auto val = SetDCBrushColor(device_handle, brush_color);
		GET_ERROR_MSG_OUTPUT_NORMAL(val, CLR_INVALID);
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
		GET_ERROR_MSG_OUTPUT_NORMAL(val, 0);
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
		GET_ERROR_MSG_OUTPUT_NORMAL(val, 0);
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
		GET_ERROR_MSG_OUTPUT_NORMAL(val, 0);
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
		GET_ERROR_MSG_OUTPUT_NORMAL(val, GDI_ERROR);
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
		GET_ERROR_MSG_OUTPUT_NORMAL(val, GDI_ERROR);
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
		GET_ERROR_MSG_OUTPUT_NORMAL(val, -1);
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
		GET_ERROR_MSG_OUTPUT_NORMAL(val, 0);
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
		GET_ERROR_MSG_OUTPUT_NORMAL(val, CLR_INVALID);
		return val;
	}

};//mw::gdi

