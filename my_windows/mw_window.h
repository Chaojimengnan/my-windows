#pragma once
#include "mw_process.h"
#include <unordered_map>

namespace mw {
namespace user {

	// TODO: 
	//	- 还没有包装分层窗口相关的函数

	/// <summary>
	/// 将RECT转换为对应的std::pair{width, height}
	/// </summary>
	/// <param name="rect">要转换的RECT</param>
	/// <returns>返回对应的宽度和高度pair</returns>
	constexpr inline std::pair<int, int> rect_to_width_height(const RECT& rect)
	{
		return std::pair<int, int>(rect.right - rect.left, rect.bottom - rect.top);
	}

	/// <summary>
	/// 获取指定窗口的边界矩形的尺寸,其坐标是相对于屏幕左上角的，就算指定窗口是子窗口也是如此。
	/// </summary>
	/// <param name="window_handle">指定窗口句柄</param>
	/// <param name="window_rect">[out]指定窗口的rect</param>
	/// <returns>操作是否成功</returns>
	inline bool get_window_rect(HWND window_handle, RECT& window_rect)
	{
		auto val = GetWindowRect(window_handle, &window_rect);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 获取指定窗口的客户区的宽度和高低
	/// </summary>
	/// <param name="window_handle">指定窗口句柄</param>
	/// <param name="width">[out]窗口的宽度</param>
	/// <param name="height">[out]窗口的高度</param>
	/// <returns>操作是否成功</returns>
	inline bool get_client_rect(HWND window_handle, LONG& width, LONG& height)
	{
		RECT tmp{ 0 };
		auto val = GetClientRect(window_handle, &tmp);
		GET_ERROR_MSG_OUTPUT();
		width = tmp.right, height = tmp.bottom;
		return val;
	}

	/// <summary>
	/// 将指定屏幕坐标转换成指定窗口客户区坐标
	/// </summary>
	/// <param name="window_handle">指定窗口句柄</param>
	/// <param name="point_x">[in,out]传入屏幕坐标，返回指定窗口客户区坐标</param>
	/// <param name="point_y">[in,out]传入屏幕坐标，返回指定窗口客户区坐标</param>
	/// <returns>操作是否成功</returns>
	inline bool screen_to_client(HWND window_handle, LONG& point_x, LONG& point_y)
	{
		POINT tmp{ point_x, point_y };
		auto val = ScreenToClient(window_handle, &tmp);
		GET_ERROR_MSG_OUTPUT();
		point_x = tmp.x, point_y = tmp.y;
		return val;
	}

	/// <summary>
	/// 将特定的客户区大小转换为所需要的窗口大小(指定不同样式所需的窗口大小可能不同)
	/// </summary>
	/// <remarks>该函数是`AdjustWindowRectEx`的封装，这里是官方文档的节选
	/// 
	/// 当菜单栏有两个或多行时，该函数不会添加额外空间。
	/// 该函数不会考虑`WS_VSCROLL`和`WS_HSCROLL`，想要计算滚动条使用
	/// mw::get_system_metrics并传入SM_CXVSCROLL 或 SM_CYHSCROLL。注意该函数也不会意识到DPI(Dots Per Inch，每英寸点数)。
	/// 我并没有封装对应的DPI函数，暂时还用不到，如果你需要可以自己封装。
	/// </remarks>
	/// <param name="rect">[in, out]输入客户区大小，返回所需窗口的大小</param>
	/// <param name="style">窗口样式，用于计算，注意不要指定`WS_OVERLAPPED`</param>
	/// <param name="has_menu">指定窗口是否有菜单</param>
	/// <param name="exstyle">窗口扩展样式，用于计算</param>
	/// <returns>返回操作是否成功</returns>
	inline bool client_size_to_window_size(RECT& rect, DWORD style = 0, bool has_menu = false, DWORD exstyle = 0)
	{
		auto val = AdjustWindowRectEx(&rect, style, has_menu, exstyle);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 在系统中寻找对应窗口类和窗口名字的`顶级窗口`句柄(不搜寻子窗口，不区分大小写)
	/// </summary>
	/// <param name="class_name">窗口类的名字，若为""，它将查找标题与window_name匹配的任何窗口</param>
	/// <param name="window_name">窗口的名字，若为""，则所有窗口名称都匹配</param>
	/// <returns>返回对应的窗口句柄(操作失败返回NULL)</returns>
	inline HWND find_window(const std::tstring& class_name = _T(""), const std::tstring& window_name = _T(""))
	{
		auto val = FindWindow( tstring_to_pointer(class_name), tstring_to_pointer(window_name));
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 在指定父窗口句柄中寻找对应窗口类和窗口名字的句柄(不区分大小写)
	/// </summary>
	/// <param name="class_name">窗口类的名字，若为""，它将查找标题与window_name匹配的任何窗口</param>
	/// <param name="window_name">窗口的名字，若为""，则所有窗口名称都匹配</param>
	/// <param name="parent_window">指定要搜索哪个父窗口的子窗口，若为NULL，父窗口为桌面窗口，若为HWND_MESSAGE，则寻找仅消息窗口</param>
	/// <param name="child_after">从该子窗口句柄(必须是直接子窗口)之后开始搜索，顺序是Z轴顺序，若为NULL，则从第一个开始</param>
	/// <returns>返回对应的窗口句柄(操作失败返回NULL)</returns>
	inline HWND find_child_window(const std::tstring& class_name = _T(""),
		const std::tstring& window_name = _T(""), HWND parent_window = nullptr, HWND child_after = nullptr)
	{
		auto val = FindWindowEx( parent_window, child_after,
			tstring_to_pointer(class_name), tstring_to_pointer(window_name));
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 获取窗口句柄对应的窗口类的名字(GetClassName)
	/// </summary>
	/// <param name="window_handle">窗口句柄</param>
	/// <param name="class_name">[out]窗口类的名字</param>
	/// <returns>操作是否成功</returns>
	inline bool get_window_class_name(HWND window_handle, std::tstring& class_name)
	{
		TCHAR temp_str[MW_MAX_TEXT] = { 0 };
		auto is_ok = GetClassName( window_handle, temp_str, MAX_PATH);
		GET_ERROR_MSG_OUTPUT();
		class_name = temp_str;
		return is_ok;
	}

	/// <summary>
	/// 获取指定窗口的类型字符串(RealGetWindowClass)
	/// </summary>
	/// <param name="window_handle">窗口句柄</param>
	/// <param name="class_name">[out]窗口类型字符串</param>
	/// <returns>操作是否成功</returns>
	inline bool get_window_type(HWND window_handle, std::tstring& class_name)
	{
		TCHAR temp_str[MW_MAX_TEXT] = { 0 };
		auto is_ok = RealGetWindowClass( window_handle, temp_str, MAX_PATH);
		GET_ERROR_MSG_OUTPUT();
		class_name = temp_str;
		return is_ok;
	}

	/// <summary>
	/// 获取窗口标题文本(如果有的话)，如果窗口是控件，则获取控件的文本。注意该函数无法检索非本程序的控件的文本。
	/// </summary>
	/// <param name="window_handle">窗口句柄</param>
	/// <param name="text">[out]文本</param>
	/// <returns>操作是否成功</returns>
	inline bool get_window_text(HWND window_handle, std::tstring& text)
	{
		TCHAR temp_str[MW_MAX_TEXT] = { 0 };
		auto is_ok = GetWindowText( window_handle, temp_str, MW_MAX_TEXT);
		GET_ERROR_MSG_OUTPUT();
		text = temp_str;
		return is_ok;
	}


	/// <summary>
	/// 更改指定窗口标题栏的文本（如果有的话）。如果指定的窗口是控件，则更改控件的文本。注意该函数无法修改非本程序的控件的文本。
	/// </summary>
	/// <remarks>使用SetDlgItemText而不是该函数来修改控件的文本或标题</remarks>
	/// <param name="window_handle"></param>
	/// <param name="text"></param>
	/// <returns></returns>
	inline bool set_window_text(HWND window_handle, const std::tstring& text)
	{
		auto val = SetWindowText( window_handle, text.c_str());
		GET_ERROR_MSG_OUTPUT();
		return val;
	}


	/// <summary>
	/// 改变指定窗口的属性，该函数也可以在额外窗口内存中的指定偏移处设置一个值。
	/// </summary>
	/// <remarks>
	/// 封装自`SetWindowLongPtr`，关于index的可选值请看文档
	/// </remarks>
	/// <param name="window_handle">窗口句柄</param>
	/// <param name="index">指定偏移量，具体可选值看文档</param>
	/// <param name="new_attribute">新属性的值</param>
	/// <returns>如果成功，返回值是前一个指定的偏移量，若之前没有设置，则返回0。函数在失败时也返回0</returns>
	inline LONG_PTR set_window_attribute(HWND window_handle, int index, LONG_PTR new_attribute)
	{
		auto val = SetWindowLongPtr(window_handle, index, new_attribute);
		SetLastError(0);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}


	/// <summary>
	/// 改变指定窗口类的属性，该函数也可以在额外窗口类内存中的指定偏移处设置一个值(实际就是改系统维护的这个窗口类的WNDCLASSEX)
	/// </summary>
	/// <remarks>
	/// 封装自`SetClassLongPtr`，关于index的可选值请看文档
	/// </remarks>
	/// <param name="window_handle">属于指定窗口类的窗口的句柄</param>
	/// <param name="index">指定偏移量，具体可选值看文档</param>
	/// <param name="new_attribute">新属性的值</param>
	/// <returns>如果成功，返回值是前一个指定的偏移量，若之前没有设置，则返回0。函数在失败时也返回0</returns>
	inline ULONG_PTR set_window_class_attribute(HWND window_handle, int index, LONG_PTR new_attribute)
	{
		auto val = SetClassLongPtr(window_handle, index, new_attribute);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 获取指定窗口类名字的WNDCLASSEXA信息，一般用于超类化一个系统全局窗口类
	/// </summary>
	/// <param name="window_class_name">指定窗口类名字</param>
	/// <param name="output_window_class">指定窗口类的WNDCLASSEXA信息副本</param>
	/// <param name="instance">一般为NULL，表示获取系统全局窗口类</param>
	/// <returns>若函数找到匹配数据，并成功复制数据，返回true，否则返回false</returns>
	inline bool get_window_class_info(const std::tstring& window_class_name,
		WNDCLASSEX& output_window_class, HINSTANCE instance = nullptr)
	{
		auto val = GetClassInfoEx(instance, window_class_name.c_str(), &output_window_class);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}


	/// <summary>
	/// 获取指定窗口的指定属性，该函数也可以在额外窗口内存中的指定偏移处获得一个值
	/// </summary>
	/// <remarks>
	/// 封装自`GetWindowLongPtr`，关于index的可选值请看文档。
	/// 
	/// 如果之前没有调用过set_window_attribute的情况下请求额外窗口内存的某个值，该函数将返回0
	/// </remarks>
	/// <param name="window_handle">窗口句柄</param>
	/// <param name="index">指定偏移量，具体可选值看文档</param>
	/// <returns>若函数成功，返回请求的值，否则返回0</returns>
	inline LONG_PTR get_window_attribute(HWND window_handle, int index)
	{
		auto val = GetWindowLongPtr( window_handle, index);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}


	/// <summary>
	/// 获取指定窗口类的指定属性，该函数也可以在额外窗口类内存中的指定偏移处获得一个值(实际上就是获取系统维护的这个窗口类的WNDCLASSEX)
	/// </summary>
	/// <remarks>
	/// 封装自`GetClassLongPtr`，关于index的可选值请看文档
	/// </remarks>
	/// <param name="window_handle">属于指定窗口类的窗口的句柄</param>
	/// <param name="index">指定偏移量，具体可选值看文档</param>
	/// <returns>成功则返回请求的值，否则返回0</returns>
	inline ULONG_PTR get_window_class_attribute(HWND window_handle, int index)
	{
		auto val = GetClassLongPtr(window_handle, index);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}



	/// <summary>
	/// 检查指定窗口是否是Unicode窗口(注：与`RegisterClass`的版本有关)
	/// </summary>
	/// <param name="window_handle">要测试的窗口句柄</param>
	/// <returns>若是Unicode窗口返回true，否则是ANSI窗口返回false</returns>
	inline bool is_window_unicode(HWND window_handle)
	{
		return IsWindowUnicode(window_handle);
	}

	/// <summary>
	/// 检查指定窗口句柄是否标识一个存在的窗口
	/// </summary>
	/// <param name="window_handle">要测试的窗口句柄</param>
	/// <returns>是否标识一个存在的窗口</returns>
	inline bool is_window_handle_vaild(HWND window_handle)
	{
		auto val = IsWindow(window_handle);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}


	/// <summary>
	/// 获取包含屏幕指定点的窗口的句柄，若指定点没有窗口则返回NULL。如果指定点在静态控件上，则返回静态控件的父窗口。
	/// </summary>
	/// <remarks>该函数不会获取隐藏或禁止的窗口，就算点在这些窗口上。</remarks>
	/// <param name="point_x">屏幕点的x坐标</param>
	/// <param name="point_y">屏幕点的y坐标</param>
	/// <returns>包含屏幕指定点的窗口的句柄</returns>
	inline HWND get_window_from_screen_point(int point_x, int point_y)
	{
		auto val = WindowFromPoint( POINT{ point_x, point_y });
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 获取指定父窗口句柄的任何一个包含指定点的直接子窗口句柄。该函数默认忽略不可见，禁用，和透明的子窗口。
	/// </summary>
	/// <remarks>注意，该函数只会获取直接后代，不会深度搜索孙代之后的窗口(即子窗口创建的窗口)，封装自`ChildWindowFromPointEx`</remarks>
	/// <param name="parent_window">父窗口句柄</param>
	/// <param name="point_x">相对于父窗口客户区的坐标x</param>
	/// <param name="point_y">相对于父窗口客户区的坐标y</param>
	/// <param name="flags">忽略标志，你可以使用`CWP_ALL`来指定所有窗口(不忽略任何内容)</param>
	/// <returns>返回包含指定点的第一个符合条件的子窗口句柄(取决于子窗口的Z坐标顺序)，否则若该点在父窗口内，则返回父窗口句柄，否则若在父窗口外或操作失败返回NULL</returns>
	inline HWND get_child_window_from_parent_point(HWND parent_window, int point_x,
		int point_y, UINT flags = CWP_SKIPDISABLED | CWP_SKIPINVISIBLE | CWP_SKIPTRANSPARENT)
	{
		auto val = ChildWindowFromPointEx( parent_window, POINT{ point_x, point_y }, flags);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 从指定父窗口获取指定标识符(ID)的子窗口句柄，也可以用来获取对话框的控件句柄(因为控件是对话框的子窗口)
	/// </summary>
	/// <param name="parent_window">父窗口句柄</param>
	/// <param name="child_id">子窗口的ID，可以是CreateWindowEx中hMenu参数指定的值，也可以是对话框控件的ID(它们是一样的)</param>
	/// <returns>指定子窗口的句柄</returns>
	inline HWND get_child_winodw_from_id(HWND parent_window, int child_id)
	{
		auto val = GetDlgItem(parent_window, child_id);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 获取指定控件的标识符(ID),也可以获取子窗口的ID
	/// </summary>
	/// <param name="control_handle">指定控件的句柄，也可以是子窗口的句柄</param>
	/// <returns>指定控件的标识符(ID)，或子窗口ID</returns>
	inline int get_child_window_id(HWND control_handle)
	{
		auto val = GetDlgCtrlID(control_handle);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 获取一个桌面窗口的句柄
	/// </summary>
	/// <remarks>桌面窗口覆盖整个屏幕，桌面窗口是完全掌控其他窗口绘制的区域</remarks>
	/// <returns>返回桌面窗口句柄</returns>
	inline HWND get_desktop_window()
	{
		return GetDesktopWindow();
	}

	/// <summary>
	/// 设置光标形状
	/// </summary>
	/// <param name="cursor">新的光标</param>
	/// <returns>如果之前有光标，则返回之前这个光标，否则返回NULL</returns>
	inline HCURSOR set_cursor(HCURSOR cursor)
	{
		return SetCursor(cursor);
	}

	/// <summary>
	/// 将指定消息信息传给指定窗口过程，一般用于超类或子类过程调用原始窗口过程
	/// </summary>
	/// <param name="window_procedure">指定窗口过程的指针</param>
	/// <param name="hWnd">窗口句柄</param>
	/// <param name="Msg">消息标识符</param>
	/// <param name="wParam">额外消息参数</param>
	/// <param name="lParam">额外消息参数</param>
	/// <returns>指定窗口过程的返回值</returns>
	inline LRESULT call_window_procedure(WNDPROC window_procedure, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		return CallWindowProc(window_procedure, hWnd, Msg, wParam, lParam);
	}

	/// <summary>
	/// 系统定义的默认窗口过程函数，若你的窗口过程没有处理某种消息，就调用该函数来默认处理
	/// </summary>
	/// <param name="hWnd">窗口句柄</param>
	/// <param name="Msg">消息标识符</param>
	/// <param name="wParam">额外消息参数</param>
	/// <param name="lParam">额外消息参数</param>
	/// <returns>默认窗口过程函数的返回值</returns>
	inline LRESULT default_window_procedure(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}



	/// <summary>
	/// 注册一个窗口类，返回一个能指定该窗口类的ATOM
	/// </summary>
	/// <param name="procedure">窗口过程</param>
	/// <param name="class_name">窗口类名字</param>
	/// <param name="event_function_dict">事件函数字典</param>
	/// <param name="hIcon">图标句柄</param>
	/// <param name="hCursor">光标句柄</param>
	/// <param name="hbrBackground">背景刷</param>
	/// <param name="hIconSm">小图标句柄</param>
	/// <param name="style">样式</param>
	/// <param name="menu_name">菜单名字</param>
	/// <param name="cbClsExtra">窗口类额外空间(字节)</param>
	/// <param name="cbWndExtra">窗口额外空间(字节)</param>
	/// <returns>返回一个能唯一指定该窗口类的ATOM，若失败返回0</returns>
	inline ATOM register_window_class(WNDPROC procedure,
		const std::tstring& class_name = _T("my_class"),
		HICON hIcon = nullptr, HCURSOR hCursor = nullptr,
		HBRUSH hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH),
		HICON hIconSm = nullptr, UINT style = CS_HREDRAW | CS_VREDRAW,
		const std::tstring& menu_name = _T(""),
		int cbClsExtra = 0, int cbWndExtra = 0)
	{
		WNDCLASSEX win_class;

		auto hinstance = get_module_handle();
		win_class.cbSize = sizeof(WNDCLASSEX);
		win_class.style = style;
		win_class.cbClsExtra = cbClsExtra;
		win_class.cbWndExtra = cbWndExtra;
		win_class.hIcon = hIcon;
		win_class.hCursor = hCursor;
		win_class.hIconSm = hIconSm;
		win_class.hbrBackground = hbrBackground;
		win_class.lpszMenuName = tstring_to_pointer(menu_name);
		win_class.lpszClassName = tstring_to_pointer(class_name);
		win_class.hInstance = hinstance;
		win_class.lpfnWndProc = procedure;

		auto val = RegisterClassEx(&win_class);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}



	/// <summary>
	/// 创建并返回一个窗口句柄
	/// </summary>
	/// <param name="window_class_name">窗口类的名字</param>
	/// <param name="window_name">窗口的名字，一般窗口会在标题栏显示，有些控件则是在内容区显示</param>
	/// <param name="x">窗口的x位置，可以指定CW_USEDEFAULT</param>
	/// <param name="y">窗口的y位置，可以指定CW_USEDEFAULT</param>
	/// <param name="width">窗口的宽度，可以指定CW_USEDEFAULT</param>
	/// <param name="height">窗口的高度，可以指定CW_USEDEFAULT</param>
	/// <param name="window_parent">该窗口的父窗口句柄或拥有者句柄，若是子窗口(WS_CHILD)就是父窗口句柄，若是重叠(WS_OVERLAPPED)或弹出(WS_POPUP)窗口则是拥有者句柄</param>
	/// <param name="menu">菜单句柄或子窗口标识符，对于顶级窗口是菜单句柄，对于子窗口是子窗口标识符(子窗口没有菜单)</param>
	/// <param name="lParam">会作为`CREATESTRUCT`的`lpCreateParams`成员，在`WM_NCCREATE`和`WM_CREATE`消息的lParam可以访问到(lParam指向CREATESTRUCT)</param>
	/// <param name="style">窗口样式</param>
	/// <param name="ex_style">扩展样式</param>
	/// <returns>返回新创建窗口的句柄</returns>
	inline HWND create_window(const std::tstring& window_class_name, const std::tstring& window_name = _T("my window"),
		int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int width = CW_USEDEFAULT, int height = CW_USEDEFAULT,
		HWND window_parent = nullptr, HMENU menu = nullptr,
		LPVOID lParam = nullptr, DWORD style = WS_OVERLAPPEDWINDOW, DWORD ex_style = 0)
	{
		/*TCHAR buffer[MAX_PATH] = { 0 };
		_tcscpy_s(buffer, MAX_PATH, window_name.c_str())*/

		auto my_hwnd = CreateWindowEx(ex_style, window_class_name.c_str(),
			window_name.c_str(), style, x, y, width, height,
			window_parent, menu, get_module_handle(), lParam);
		GET_ERROR_MSG_OUTPUT();
		return my_hwnd;
	}


	/// <summary>
	/// 注销注册的窗口类，注意，注销之前一定确保由该窗口类生成的窗口都已被销毁
	/// </summary>
	/// <param name="window_class_name">窗口类名字</param>
	/// <param name="ins">模块实例句柄</param>
	/// <returns>操作是否成功</returns>
	inline bool unregister_window_class(const std::tstring& window_class_name, HINSTANCE ins)
	{
		auto val = UnregisterClass(window_class_name.c_str(), ins);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 枚举在桌面上的顶级窗口，通过将每一个顶级窗口句柄依次传入enum_procedure来枚举，直到最后一个顶级窗口被枚举了或回调函数返回FALSE
	/// </summary>
	/// <remarks>
	/// 注意，只枚举顶级窗口，不枚举顶级窗口下的子窗口
	/// 
	/// 注意，在WIN8或以上的版本，只枚举顶级的桌面程序(desktop apps)，该函数比循环调用`GetWindow`更可靠，其他请看文档。
	/// </remarks>
	/// <param name="enum_procedure">回调函数，系统会依次将顶级窗口句柄传入该函数</param>
	/// <param name="lParam">程序定义的值，会被传入enum_procedure</param>
	/// <returns>操作是否成功</returns>
	inline bool enum_window(WNDENUMPROC enum_procedure, LPARAM lParam = 0)
	{
		auto val = EnumWindows(enum_procedure, lParam);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 枚举指定线程的所有非子窗口的窗口，通过将每一个非孩子的窗口句柄依次传入enum_procedure来枚举，直到最后一个非孩子窗口被枚举了或回调函数返回FALSE
	/// </summary>
	/// <param name="thread_id">指定线程的ID</param>
	/// <param name="enum_procedure">回调函数，系统会依次将窗口句柄传入该函数</param>
	/// <param name="lParam">程序定义的值，会被传入enum_procedure</param>
	/// <returns>操作是否成功</returns>
	inline bool enum_thread_window(DWORD thread_id, WNDENUMPROC enum_procedure, LPARAM lParam = 0)
	{
		auto val = EnumThreadWindows(thread_id, enum_procedure, lParam);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 枚举指定父窗口句柄的所有后代窗口(包括子窗口和子窗口的子窗口)，通过将每一个后代窗口句柄依次传入enum_procedure来枚举，直到最后一个后代窗口被枚举了或回调函数返回FALSE
	/// </summary>
	/// <remarks>
	/// 在枚举过程中若有子窗口按Z轴顺序被移动或重新放置，也能正确枚举。该函数不会枚举在枚举之前被销毁或在枚举过程中创建的子窗口。
	/// </remarks>
	/// <param name="enum_procedure">回调函数，系统会依次将窗口句柄传入该函数</param>
	/// <param name="lParam">程序定义的值，会被传入enum_procedure</param>
	/// <param name="parent_handle">指定父窗口句柄，若为NULL，功能与enum_window相同</param>
	/// <returns>返回值并未被使用</returns>
	inline bool enum_child_window(WNDENUMPROC enum_procedure, LPARAM lParam = 0, HWND parent_handle = NULL)
	{
		return EnumChildWindows(parent_handle, enum_procedure, lParam);
	}

	/// <summary>
	/// 获取创建指定窗口的线程ID，可选地，可以同时获取对应的进程ID
	/// </summary>
	/// <param name="window_handle">指定窗口的句柄</param>
	/// <param name="process_id">[out]创建指定窗口的进程ID</param>
	/// <returns>创建指定窗口的线程ID</returns>
	inline DWORD get_thread_process_id_from_window(HWND window_handle, LPDWORD process_id = nullptr)
	{
		auto val = GetWindowThreadProcessId(window_handle, process_id);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 设置指定窗口的显示状态，详情和注意请看官方文档，封装自`ShowWindow`
	/// </summary>
	/// <param name="window_handle">指定窗口的句柄</param>
	/// <param name="cmd_show">控制窗口如何显示，参数请看官方文档</param>
	/// <returns>若窗口之前可见，则返回true，否则返回false</returns>
	inline bool show_window(HWND window_handle, int cmd_show = 1)
	{
		auto val = ShowWindow(window_handle, cmd_show);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 设置指定窗口的显示状态，不等待操作完成，直接返回(异步)，封装自`ShowWindowAsync`
	/// </summary>
	/// <param name="window_handle">指定窗口的句柄</param>
	/// <param name="cmd_show">控制窗口如何显示，参数请看官方文档</param>
	/// <returns>操作是否成功</returns>
	inline bool show_window_async(HWND window_handle, int cmd_show)
	{
		auto val = ShowWindowAsync(window_handle, cmd_show);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 改变指定子窗口的父窗口
	/// </summary>
	/// <remarks>注意，该函数不会改变窗口本身的样式，所以如果你将一个子窗口设置成顶级窗口，需要将`WS_CHILD`样式去掉，顶级窗口变子窗口也同理，具体看文档说明</remarks>
	/// <param name="child_handle">子窗口的句柄</param>
	/// <param name="new_parent_handle">新的父窗口句柄，若为NULL，则桌面窗口成为新的父窗口(即变成顶级窗口),若为HWND_MESSAGE，则该子窗口变成仅消息窗口</param>
	/// <returns>成功返回前一个父窗口句柄，失败则返回NULL</returns>
	inline HWND set_parent(HWND child_handle, HWND new_parent_handle = nullptr)
	{
		auto val = SetParent(child_handle, new_parent_handle);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 获取一个与指定窗口有指定关系的窗口句柄，不要在一个循环中调用该函数，而是使用`enum_child_window`
	/// </summary>
	/// <param name="window_handle">指定窗口的句柄</param>
	/// <param name="cmd">要获取的窗口与指定的窗口之间的关系，具体值看文档，前缀为GW_</param>
	/// <returns>若成功，返回窗口句柄，否则返回NULL</returns>
	inline HWND get_window(HWND window_handle, UINT cmd)
	{
		auto val = GetWindow(window_handle, cmd);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 获取指定窗口的祖先窗口句柄，默认获取父窗口句柄
	/// </summary>
	/// <param name="window_handle">指定窗口句柄</param>
	/// <param name="flags">要获取的祖先，具体看文档，前缀为GA_</param>
	/// <returns>指定祖先窗口的句柄</returns>
	inline HWND get_ancestor(HWND window_handle, UINT flags = GA_PARENT)
	{
		auto val = GetAncestor(window_handle, flags);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 确定一个窗口是否是指定父窗口的子窗口或后代窗口
	/// </summary>
	/// <param name="parent_handle">父窗口句柄</param>
	/// <param name="window_handle">要测试的窗口句柄</param>
	/// <returns>若指定窗口是子窗口或后代窗口返回true，否则返回false</returns>
	inline bool is_child(HWND parent_handle, HWND window_handle)
	{
		auto val = IsChild(parent_handle, window_handle);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 确定指定窗口的可见性状态
	/// </summary>
	/// <remarks>由于返回值指定窗口是否具有WS_VISIBLE样式，因此即使该窗口被其他窗口完全遮挡，它也可能true。</remarks>
	/// <param name="window_handle">窗口的句柄</param>
	/// <returns>如果指定的窗口、其父窗口、其爷窗口等具有WS_VISIBLE样式，则返回值true。否则，返回值为false。</returns>
	inline bool is_window_visible(HWND window_handle)
	{
		auto val = IsWindowVisible(window_handle);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 确定指定的窗口是否启用鼠标和键盘输入。
	/// </summary>
	/// <param name="window_handle">要测试的窗口的句柄</param>
	/// <returns>如果窗口已启用，则返回值true。如果窗口未启用，则返回值false</returns>
	inline bool is_window_enabled(HWND window_handle)
	{
		auto val = IsWindowEnabled(window_handle);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 启用或禁用鼠标和键盘输入到指定窗口或控件。当输入被禁用时，窗口不会接收诸如鼠标点击和按键之类的输入。当输入被启用时，窗口接收所有输入。
	/// </summary>
	/// <remarks>调用该函数会发送消息，具体看文档</remarks>
	/// <param name="window_handle">指定窗口句柄</param>
	/// <param name="is_enable">若为TRUE，则启用，否则为禁用</param>
	/// <returns>若之前被禁用返回true，若之前没有禁用返回false</returns>
	inline bool enable_window(HWND window_handle, bool is_enable)
	{
		auto val = EnableWindow(window_handle, is_enable);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 显示或隐藏指定窗口拥有的所有弹出窗口。
	/// </summary>
	/// <param name="window_handle">拥有要显示或隐藏的弹出窗口的窗口句柄</param>
	/// <param name="is_show">如果此参数为TRUE，则显示所有隐藏的弹出窗口。如果此参数为FALSE，则隐藏所有可见的弹出窗口</param>
	/// <returns>操作是否成功</returns>
	inline bool show_owned_popups(HWND window_handle, bool is_show)
	{
		auto val = ShowOwnedPopups(window_handle, is_show);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 获取前台窗口（用户当前正在使用的窗口）的句柄。系统为创建前台窗口的线程分配比其他线程稍高的优先级。
	/// </summary>
	/// <returns>前台窗口的句柄</returns>
	inline HWND get_foreground_window()
	{
		return GetForegroundWindow();
	}


	/// <summary>
	/// 将创建指定窗口的线程带到前台并激活指定窗口。键盘输入被定向到窗口，并且为用户改变了各种视觉提示。系统为创建前台窗口的线程分配比其他线程稍高的优先级。
	/// </summary>
	/// <remarks>设置前台窗口有许多限制，具体看文档</remarks>
	/// <param name="window_handle">要变成前台窗口的窗口句柄</param>
	/// <returns>若指定窗口被置于前台，返回true，否则返回false</returns>
	inline bool set_foreground_window(HWND window_handle)
	{
		auto val = SetForegroundWindow(window_handle);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 前台进程可以调用该函数来禁用对`SetForegroundWindow`函数的调用。
	/// </summary>
	/// <param name="lock_code">LSFW_前缀，LSFW_LOCK表示禁用其调用，LSFW_UNLOCK表示启用其调用</param>
	/// <returns>操作是否成功</returns>
	inline bool lock_set_foreground_window(UINT lock_code = LSFW_LOCK)
	{
		auto val = LockSetForegroundWindow(lock_code);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 将指定的窗口带到Z轴顺序的顶部。如果窗口是顶级窗口，则它被激活。如果窗口是子窗口，则激活与子窗口关联的顶级父窗口。
	/// </summary>
	/// <param name="window_handle">指定窗口</param>
	/// <returns>操作是否成功</returns>
	inline bool bring_window_to_top(HWND window_handle)
	{
		auto val = BringWindowToTop(window_handle);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 检查与指定父窗口关联的子窗口的Z轴顺序，并获取Z轴顺序顶部的子窗口的句柄。
	/// </summary>
	/// <param name="window_handle">指定父窗口，若为NULL，则返回在Z轴顺序顶部的窗口</param>
	/// <returns>若成功，返回最顶部的子窗口的句柄，若指定窗口没有子窗口，返回NULL</returns>
	inline HWND get_top_window(HWND window_handle)
	{
		auto val = GetTopWindow(window_handle);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}


	/// <summary>
	/// 更改子窗口、弹出窗口或顶级窗口的大小、位置和Z轴顺序。要指定多个窗口的大小，位置和Z轴顺序，使用DeferWindowPos
	/// </summary>
	/// <remarks>若使用SetWindowLong改变了某些窗口数据，使用该函数使其生效，其他注意事项看文档</remarks>
	/// <param name="window_handle">指定要修改的窗口句柄</param>
	/// <param name="flags">以SWP_开头的标志设定，具体含义看文档</param>
	/// <param name="offset_x">相对于客户坐标的x位置，可以通过在flags设置忽略该参数</param>
	/// <param name="offset_y">相对于客户坐标的y位置，可以通过在flags设置忽略该参数</param>
	/// <param name="width">以像素为单位的窗口新宽度</param>
	/// <param name="height">以像素为单位的窗口新高度</param>
	/// <param name="insert_after">可选值，它必须是一个窗口句柄或以HWND_开头的宏，关于宏的含义看文档</param>
	/// <returns>操作是否成功</returns>
	inline bool set_window_pos(HWND window_handle, UINT flags = SWP_NOMOVE|SWP_NOSIZE, int offset_x = 0, int offset_y = 0, int width = 0, int height = 0,  HWND insert_after = HWND_TOP)
	{
		auto val = SetWindowPos(window_handle, insert_after, offset_x, offset_y, width, height, flags);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 激活一个窗口，该窗口必须被附加到调用线程的消息队列，其他事项请看文档
	/// </summary>
	/// <param name="window_handle">要被激活的顶级窗口</param>
	/// <returns>若成功，返回之前被激活的窗口句柄，若失败则返回NULL</returns>
	inline HWND set_active_window(HWND window_handle)
	{
		auto val = SetActiveWindow(window_handle);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 获取附加到调用线程的消息队列的活动窗口的窗口句柄。
	/// </summary>
	/// <returns>返回值是附加到调用线程的消息队列的活动窗口的句柄。否则，返回值为NULL。</returns>
	inline HWND get_active_window()
	{
		return GetActiveWindow();
	}

	/// <summary>
	/// 销毁指定窗口，该函数也会发送WM_DESTROY和WM_NCDESTROY，若指定为父窗口或拥有者窗口，则先删除子窗口或被拥有者窗口。
	/// </summary>
	/// <remarks>线程不能使用destroy_window来销毁由不同线程创建的窗口，其他事项请看文档</remarks>
	/// <param name="window_handle">指定要销毁的窗口</param>
	/// <returns>操作是否成功</returns>
	inline bool destroy_window(HWND window_handle)
	{
		auto val = DestroyWindow(window_handle);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 使您能够在显示或隐藏窗口时产生特殊效果。有四种类型的动画：滚动、滑动、折叠或展开，以及 alpha 混合淡入淡出。
	/// </summary>
	/// <param name="window_handle">要设置动画的窗口句柄。调用线程必须拥有这个窗口</param>
	/// <param name="time">播放动画所需的时间，以毫秒为单位。通常，播放动画需要 200 毫秒</param>
	/// <param name="flags">以AW_开头的宏，默认情况下在显示窗口时生效，若要在隐藏窗口时生效，加入AW_HIDE标记</param>
	/// <returns>操作是否成功，失败原因可以查看文档说明</returns>
	inline bool animate_window(HWND window_handle, DWORD time = 200, DWORD flags = AW_BLEND)
	{
		auto val = AnimateWindow(window_handle, time, flags);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 若指定窗口更新区域不为空，该函数发送WM_PAINT消息给它。该函数绕过程序队列直接发送给窗口过程，若更新区域为空，不发送消息
	/// </summary>
	/// <param name="window_handle">指定要更新的窗口句柄</param>
	/// <returns>操作是否成功</returns>
	inline bool update_window(HWND window_handle)
	{
		auto val = UpdateWindow(window_handle);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 该函数向当前线程消息队列投递一条WM_QUIT消息并立即返回
	/// </summary>
	/// <param name="exit_code">退出代码，它将存储在WM_QUIT消息的wParam参数中</param>
	inline void post_quit_message(int exit_code)
	{
		return PostQuitMessage(exit_code);
	}


	/// <summary>
	/// 返回一个保证在整个系统中唯一的新消息值，该消息值可用于发送或投递消息，一般用于两个程序之间通信。
	/// </summary>
	/// <remarks>其他应用程序出于不同目的使用相同的消息标识符，则使用此功函数可以防止可能出现的冲突(一般用于程序之间的交流)</remarks>
	/// <param name="message_str">消息字符串</param>
	/// <returns>一个在系统唯一的新消息值</returns>
	inline UINT register_window_message(const std::tstring& message_str)
	{
		auto val = RegisterWindowMessage(message_str.c_str());
		GET_ERROR_MSG_OUTPUT();
		return val;
	}


	/// <summary>
	/// 投递一条消息给与指定窗口关联线程的消息队列，并且不等待线程处理消息直接返回(异步)
	/// </summary>
	/// <remarks>跨进程投递消息可能被UIPI阻止，具体看文档。投递WM_QUIT不要用该函数，而是用post_quit_message，其他事项看文档</remarks>
	/// <param name="window_handle">消息投递的目标窗口,若为NULL，投递到当前线程的消息队列，若为HWND_BROADCAST，则投递给系统中所有顶级窗口(不包括子窗口)</param>
	/// <param name="Msg">要投递的消息</param>
	/// <param name="wParam">额外消息参数，注意，异步消息函数发送WM_USER以下范围消息，消息参数不能是指针，具体看文档</param>
	/// <param name="lParam">额外消息参数，注意，异步消息函数发送WM_USER以下范围消息，消息参数不能是指针，具体看文档</param>
	/// <returns>操作是否成功</returns>
	inline bool post_message(HWND window_handle, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		auto val = PostMessage(window_handle, Msg, wParam, lParam);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 投递一条消息给指定线程的消息队列，并且不等待线程处理消息直接返回(异步)
	/// </summary>
	/// <remarks>跨进程投递消息可能被UIPI阻止，具体看文档。被发送消息的线程必须已经创建消息队列，否则调用失败，具体看文档。
	/// 注意，该函数发送的消息与窗口无关，所以使用DispatchMessage来分发与窗口无关的消息是不行的，具体看文档。
	/// </remarks>
	/// <param name="thread_id">消息要投递到的线程的标识符，注意指定线程必须要拥有消息队列，否则调用失败，其他事项看文档</param>
	/// <param name="Msg">要投递的消息</param>
	/// <param name="wParam">额外消息参数，注意，异步消息函数发送WM_USER以下范围消息，消息参数不能是指针，具体看文档</param>
	/// <param name="lParam">额外消息参数，注意，异步消息函数发送WM_USER以下范围消息，消息参数不能是指针，具体看文档</param>
	/// <returns>操作是否成功</returns>
	inline bool post_thread_message(DWORD thread_id, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		auto val = PostThreadMessage(thread_id, Msg, wParam, lParam);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 从调用线程的消息队列中获取一条消息。该函数分发传入的发送(send)消息，直到有一个能获取的投递（post）消息才返回。
	/// </summary>
	/// <remarks>该函数会等待找到一条投递消息才会返回(期间会分发发送(send)的消息)，使用PeekMessage可以直接返回</remarks>
	/// <param name="msg">[out]获取到的消息数据将填充到这个结构体中</param>
	/// <param name="window_handle">从消息队列中获取指定窗口的消息，若为NULL，则获取该线程的消息(包括该线程所有窗口消息和线程自己的消息)</param>
	/// <param name="msg_filter_min">过滤器最低值，若min和max都为0，不执行过滤</param>
	/// <param name="msg_filter_max">过滤器最高值，若min和max都为0，不执行过滤</param>
	/// <returns>接收到WM_QUIT时返回0，否则返回非0值，注意返回-1，则说明发生错误</returns>
	inline BOOL get_message(MSG& msg, HWND window_handle = nullptr, int msg_filter_min = 0, int msg_filter_max = 0)
	{
		auto val = GetMessage(&msg, window_handle, msg_filter_min, msg_filter_max);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 该函数分发传入的发送(send)消息, 检查线程消息队列是否有一个投递消息，如果有就获取。具体看文档
	/// </summary>
	/// <remarks>该函数相比于GetMessage，适合于在做一项需要时间很长的工作时短暂地看看有没有消息要处理，因为前者在没有投递消息时不会返回，而后者会立即返回</remarks>
	/// <param name="msg">[out]获取到的消息数据将填充到这个结构体中</param>
	/// <param name="window_handle">从消息队列中获取指定窗口的消息，若为NULL，则获取该线程的消息，若为-1，则仅获取消息队列中hwnd为NULL的消息</param>
	/// <param name="remove_msg">如何处理消息的标志，有两种标志，一个前缀时PM_，用于指定是否弹出(删除)消息，还有是前缀为PM_QS_，用于指定处理哪些消息类型</param>
	/// <param name="msg_filter_min">过滤器最低值，若min和max都为0，不执行过滤</param>
	/// <param name="msg_filter_max">过滤器最高值，若min和max都为0，不执行过滤</param>
	/// <returns>若有可用消息，返回true，否则返回false</returns>
	inline bool peek_message(MSG& msg, HWND window_handle = nullptr, 
		UINT remove_msg = PM_REMOVE, int msg_filter_min = 0, int msg_filter_max = 0)
	{
		auto val = PeekMessage(&msg, window_handle, msg_filter_min, msg_filter_max, remove_msg);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}


	/// <summary>
	/// 将消息分发给合适的窗口消息，它通常用于GetMessage返回的投递(post)消息的分发，发送(send)消息在GetMessage中已经被分发了
	/// </summary>
	/// <param name="msg">包含消息信息的结构体</param>
	/// <returns>窗口过程返回的值，通常会忽略</returns>
	inline LRESULT dispatch_message(const MSG& msg)
	{
		return DispatchMessage(&msg);
	}

	/// <summary>
	/// 给当前线程设置额外的消息信息，一个额外消息信息是程序定义的关联于当前线程消息队列的值
	/// </summary>
	/// <param name="lParam">要被关联到当前线程的值</param>
	/// <returns>前一个关联当前线程的值</returns>
	inline LPARAM set_message_extra_info(LPARAM lParam)
	{
		return SetMessageExtraInfo(lParam);
	}

	/// <summary>
	/// 从当前线程中获取一个额外消息信息，一个额外消息信息是程序定义的关联于当前线程消息队列的值
	/// </summary>
	/// <returns>返回值指定额外信息</returns>
	inline LPARAM get_message_extra_info()
	{
		return GetMessageExtraInfo();
	}

	/// <summary>
	/// 将指定消息发送给一个或多个窗口，若发送给线程内的窗口，该函数调用指定窗口的窗口过程，直到窗口过程返回后再返回。若是线程间，该线程将进入idle状态，直到另外一个线程处理完消息后返回。
	/// </summary>
	/// <remarks>
	/// 注意，线程间的情况时，只有当目标线程调用GetMessage或PeekMessage之类的函数时，才能处理发送消息。在调用线程等待时，依然会处理发送(send)给它的消息，
	/// 若想它在等待时不处理发送给它的消息，那么可以选择调用SendMessageTimeout。若想发送消息后立即返回(异步)，则使用SendMessageCallback或SendNotifyMessage。
	/// 其他事项请看文档。
	/// </remarks>
	/// <param name="window_handle">指定接收窗口的句柄，若是HWND_BROADCAST，发送给系统所有顶级窗口。消息发送受到UIPI约束</param>
	/// <param name="Msg">要发送的消息</param>
	/// <param name="wParam">额外消息参数</param>
	/// <param name="lParam">额外消息参数</param>
	/// <returns>返回值指定消息处理的结果；这取决于发送的消息。</returns>
	inline LRESULT send_message(HWND window_handle, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		auto val = SendMessage(window_handle, Msg, wParam, lParam);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 将指定消息发送给一个或多个窗口，线程内，该函数调用指定窗口的窗口过程，线程间，则立即返回，窗口处理完消息后，系统调用回调函数，并将消息处理结果和程序定义的值传给回调函数
	/// </summary>
	/// <remarks>
	/// 线程内，则是同步调用窗口过程，并在窗口过程返回后立即调用回调函数，线程间，仅当调用线程调用GetMessage、PeekMessage或WaitMessage时，才会调用回调函数(消息处理完了之后)
	/// </remarks>
	/// <param name="window_handle">指定接收窗口的句柄，若是HWND_BROADCAST，发送给系统所有顶级窗口。消息发送受到UIPI约束</param>
	/// <param name="Msg">要发送的消息</param>
	/// <param name="wParam">额外消息参数，注意，异步消息函数发送WM_USER以下范围消息，消息参数不能是指针，具体看文档</param>
	/// <param name="lParam">额外消息参数，注意，异步消息函数发送WM_USER以下范围消息，消息参数不能是指针，具体看文档</param>
	/// <param name="callback_function">若window_handle为HWND_BROADCAST,则系统为每个顶级窗口调用一次回调函数</param>
	/// <param name="data">作为第三个参数发送给回调函数</param>
	/// <returns>操作是否成功</returns>
	inline bool send_message_callback(HWND window_handle, UINT Msg, WPARAM wParam, LPARAM lParam, 
		SENDASYNCPROC callback_function, ULONG_PTR data = 0)
	{
		auto val = SendMessageCallback(window_handle, Msg, wParam, lParam, callback_function, data);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}


	/// <summary>
	/// 将指定消息发送给一个或多个窗口，线程内，该函数调用指定窗口的窗口过程，线程间，则立即返回。相比于SendMessageCallback，它不会在消息处理之后调用回调函数(也没有)
	/// </summary>
	/// <param name="window_handle">指定接收窗口的句柄，若是HWND_BROADCAST，发送给系统所有顶级窗口。消息发送受到UIPI约束</param>
	/// <param name="Msg">要发送的消息</param>
	/// <param name="wParam">额外消息参数，注意，异步消息函数发送WM_USER以下范围消息，消息参数不能是指针，具体看文档</param>
	/// <param name="lParam">额外消息参数，注意，异步消息函数发送WM_USER以下范围消息，消息参数不能是指针，具体看文档</param>
	/// <returns>操作是否成功</returns>
	inline bool send_notify_message(HWND window_handle, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		auto val = SendNotifyMessage(window_handle, Msg, wParam, lParam);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 将指定消息发送给一个或多个窗口，线程内，该函数调用指定窗口的窗口过程，忽略超时值，线程间，在超时之前或消息处理完之前不会返回
	/// </summary>
	/// <param name="window_handle">指定接收窗口的句柄，若是HWND_BROADCAST，发送给系统所有顶级窗口。在每个窗口超时之前不会返回，即总的等待时间是timeout乘上顶级窗口数量</param>
	/// <param name="Msg">要发送的消息</param>
	/// <param name="wParam">额外消息参数</param>
	/// <param name="lParam">额外消息参数</param>
	/// <param name="flags">控制函数的行为，以SMTO_开头的宏，具体含义看文档</param>
	/// <param name="timeout">超时值，以毫秒为单位，若是广播消息，每个窗口可以使用一个完整的超时值</param>
	/// <param name="result">[out]消息处理的结果，其值取决于消息</param>
	/// <returns>操作是否成功，对于Windows 2000，函数超时时调用GetLastError返回0,其他返回ERROR_TIMEOUT</returns>
	inline bool send_message_timeout(HWND window_handle, UINT Msg, WPARAM wParam, LPARAM lParam, 
		UINT flags, UINT timeout, PDWORD_PTR result = nullptr)
	{
		auto val = SendMessageTimeout(window_handle, Msg, wParam, lParam, flags, timeout, result);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 向指定的接收方发送消息。接收者可以是应用程序、可安装驱动程序、网络驱动程序、系统级设备驱动程序或这些系统组件的任意组合，具体事项请看文档。
	/// </summary>
	/// <param name="flags">广播选项，以BSF_开头的值，具体含义看文档</param>
	/// <param name="Msg">要发送的消息</param>
	/// <param name="wParam">额外消息参数</param>
	/// <param name="lParam">额外消息参数</param>
	/// <param name="info">[in,out,optional]可以为NULL，表示向所有组件广播，可以传入以BSM_开头的宏，表示向这些组件发送，返回实际接收人(也是BSM_宏)</param>
	/// <param name="sm_info">[out,optional]如果请求被拒绝，且flags设置为BSF_QUERY，指向包含附加消息的BSMINFO结构的指针</param>
	/// <returns>函数成功则是正值，若无法广播消息，则返回-1，若flags是BSF_QUERY，并且至少一个接收方返回BROADCAST_QUERY_DENY，则返回值为0</returns>
	inline long broadcast_system_message(DWORD flags, UINT Msg, WPARAM wParam, LPARAM lParam, 
		LPDWORD info = nullptr, PBSMINFO sm_info = nullptr)
	{
		auto val = BroadcastSystemMessageEx(flags, info , Msg, wParam, lParam, sm_info);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 将虚拟键消息转换为字符消息。字符消息被投递到调用线程的消息队列，以便在线程下次调用GetMessage或PeekMessage函数时读取。
	/// </summary>
	/// <param name="msg">包含消息的MSG结构体</param>
	/// <returns>若消息被翻译，返回true，若消息是WM_KEYDOWN, WM_KEYUP, WM_SYSKEYDOWN,或WM_SYSKEYUP，无论翻译如何都返回true，若没有被翻译返回false.具体看文档</returns>
	inline bool translate_key_to_character(const MSG& msg)
	{
		return TranslateMessage(&msg);
	}
	
	/// <summary>
	/// 确定当前窗口过程是否正在处理从另一个线程（在同一进程或不同进程中）发送的消息。
	/// </summary>
	/// <returns>如果消息未发送，返回0，否则是以ISMEX_开头的值或值的组合，具体看文档</returns>
	inline DWORD in_send_message()
	{
		return InSendMessageEx(nullptr);
	}

	/// <summary>
	/// in_send_message的包装，用于检测消息是否来自不同线程，以及它是否被阻塞，如果这些条件都满足，则返回true，这时你需要调用ReplyMessage，防止消息死锁
	/// </summary>
	/// <returns>是否发送者被阻塞</returns>
	inline bool is_sender_blocked()
	{
		return ((in_send_message() & (ISMEX_REPLIED | ISMEX_SEND)) == ISMEX_SEND);
	}

	/// <summary>
	/// 回复一条来自另一个线程使用SendMessage发送的消息，这样发送方就能恢复正常运行了(从idle状态恢复)，若消息不是SendMessage发的，或消息是同一线程发的，此函数无效。
	/// </summary>
	/// <remarks>注意，就算不调用该函数，只要不调用使线程"放弃控制权"的函数(如GetMessage等)，正常结束窗口过程也能使发送方恢复运行，具体看消息死锁和函数文档</remarks>
	/// <param name="result">消息处理的结果，其值基于发送的消息</param>
	/// <returns>若调用线程没有处理从另一个线程或进程发送的消息，返回false，否则返回true</returns>
	inline bool reply_message(LRESULT result = 0)
	{
		return ReplyMessage(result);
	}

	/// <summary>
	/// 创建一个指定超时值(time_out，以毫秒为单位)的计时器，也可以修改一个存在的计时器
	/// </summary>
	/// <remarks>注意，对于句柄为NULL生成的计时器，你需要在消息循环中捕获这种计时器的消息，然后将它分发给合适的窗口或自己处理(因为dispatch无法分发句柄为NULL的消息)</remarks>
	/// <param name="window_handle">指定关联的窗口句柄，若为NULL，并且timer_id指定一个存在的计时器，那么该计时器被新计时器替换(当计时器被替换时会被重置)</param>
	/// <param name="timer_id">一个非零的计时器标识符，若窗口句柄为NULL，并且标识符不匹配任何存在计时器，那么生成一个新计时器并返回一个新计时器的ID</param>
	/// <param name="time_out_val">超时值，以毫秒为单位，不能小于USER_TIMER_MINIMUM，不能大于USER_TIMER_MAXIMUM</param>
	/// <returns>若函数成功返回非0值，否则返回0，若句柄为NULL，且标识符不匹配任何存在计时器，则生成一个计时器并将其ID返回</returns>
	inline UINT_PTR set_timer(HWND window_handle, UINT_PTR timer_id, UINT time_out_val)
	{
		auto val = SetTimer(window_handle, timer_id, time_out_val, nullptr);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 销毁指定ID的计时器，若是将句柄设为NULL生成的计时器，你需要将其函数返回值作为ID传入timer_id参数
	/// </summary>
	/// <param name="window_handle">窗口句柄，若不是NULL生成的计时器，那么应该指定与它关联的窗口句柄</param>
	/// <param name="timer_id">计时器ID</param>
	/// <returns>操作是否成功</returns>
	inline bool kill_timer(HWND window_handle, UINT_PTR timer_id)
	{
		auto val = KillTimer(window_handle, timer_id);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 添加或修改指定窗口的所有物列表的一项，若所有列表不存在指定所有物名字，则添加一项，否则将修改它。
	/// </summary>
	/// <remarks>在销毁窗口之前，程序必须删除添加到所有物列表的所有条目，使用RemoveProp</remarks>
	/// <param name="window_handle">指定修改所有物列表的窗口句柄，注意若设置非本进程的窗口，可能受到UIPI限制</param>
	/// <param name="prop_name">所有物名字，用于唯一标识该所有物</param>
	/// <param name="data">所有物对应的数据，它可以是指向任何有用数据的指针</param>
	/// <returns>操作是否成功</returns>
	inline bool set_prop(HWND window_handle, const std::tstring& prop_name, HANDLE data)
	{
		auto val = SetProp(window_handle, prop_name.c_str(), data);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 从指定窗口的列表所有物列表中，删除指定所有物名字的条目
	/// </summary>
	/// <param name="window_handle">指定条目所在所有物列表对应的窗口句柄</param>
	/// <param name="prop_name">所有物名字，用于唯一标识该所有物</param>
	/// <returns>返回存储的数据指针，若找不到指定所有物，返回NULL</returns>
	inline HANDLE remove_prop(HWND window_handle, const std::tstring& prop_name)
	{
		auto val = RemoveProp(window_handle, prop_name.c_str());
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 从指定窗口的所有物列表中获取数据指针。字符串标识要获取的所有物。
	/// </summary>
	/// <param name="window_handle">要获取所有物关联的窗口句柄</param>
	/// <param name="prop_name">所有物名字，用于唯一标识该所有物</param>
	/// <returns>返回数据指针，若没找到返回NULL</returns>
	inline HANDLE get_prop(HWND window_handle, const std::tstring& prop_name)
	{
		auto val = GetProp(window_handle, prop_name.c_str());
		GET_ERROR_MSG_OUTPUT();
		return val;
	}


	/// <summary>
	/// 枚举窗口所有物列表中的所有条目，方法是将它们一一传递给指定的回调函数，直到最后一个条目被枚举或回调函数返回FALSE
	/// </summary>
	/// <remarks>注意，该函数可能枚举不是你创建的所有物</remarks>
	/// <param name="window_handle">要枚举所有物列表对应的窗口句柄</param>
	/// <param name="enum_function">回调函数，用于枚举所有物列表</param>
	/// <param name="lParam">程序定义的数据，传给回调函数</param>
	/// <returns>返回值指定回调函数返回的最后一个值。如果函数未找到所有物，则为-1</returns>
	inline int enum_props(HWND window_handle, PROPENUMPROCEX enum_function, LPARAM lParam = 0)
	{
		auto val = EnumPropsEx(window_handle, enum_function, lParam);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}


	/// <summary>
	/// 安装一个程序定义的挂钩过程到挂钩链中，安装挂钩过程能让你监控系统特定类型的事件。可以安装全局挂钩(即为与调用线程相同桌面的所有线程安装挂钩)或线程特定挂钩(为特定线程安装挂钩)
	/// </summary>
	/// <remarks>该函数可用于将DLL注入另一个进程，32位DLL不能注入64位进程，64位DLL不能注入32位进程，详情请看文档</remarks>
	/// <param name="hook_type">安装哪种类型的挂钩，其是以WH_开头的宏，具体含义请查看文档</param>
	/// <param name="hook_procedure">指向挂钩过程的指针，注意，若thread_id为0或指定的是不同进程的线程的标识符，该挂钩过程必须在一个DLL中(因为进程地址空间不同，不能放在exe中)</param>
	/// <param name="module_handle">指定挂钩过程在哪个模块，若在exe中，置为NULL即可，若在DLL中，则必须是指定DLL的实例句柄</param>
	/// <param name="thread_id">指定要与挂钩过程关联的线程ID，若为0，则设为全局挂钩(为与调用线程相同桌面的所有线程安装挂钩)</param>
	/// <returns>若成功，返回挂钩过程的句柄，若失败返回NULL</returns>
	inline HHOOK set_windows_hook(int hook_type, HOOKPROC hook_procedure, HINSTANCE module_handle, DWORD thread_id)
	{
		auto val = SetWindowsHookEx(hook_type, hook_procedure, module_handle, thread_id);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 将事件消息传递给当前挂钩链的下一个挂钩过程，挂钩过程可以在处理该事件之前或之后调用该函数
	/// </summary>
	/// <remarks>
	/// 虽然在挂钩过程调用该函数是可选的，但是强烈推荐调用，否则其他安装了挂钩的程序不会受到挂钩通知，因此可能出现不正确的行为。
	/// 除非你绝对需要防止其他程序看到通知，否则你应该调用该函数
	/// </remarks>
	/// <param name="nCode">传递给当前挂钩过程的挂钩代码</param>
	/// <param name="wParam">传递给当前挂钩过程的额外参数，含义取决于挂钩类型</param>
	/// <param name="lParam">传递给当前挂钩过程的额外参数，含义取决于挂钩类型</param>
	/// <returns>该值由下一个挂钩过程返回，当前挂钩过程也必须返回这个值，返回值取决于挂钩类型</returns>
	inline LRESULT call_next_hook(int nCode, WPARAM wParam, LPARAM lParam)
	{
		return CallNextHookEx(nullptr, nCode, wParam, lParam);
	}

	/// <summary>
	/// 删除由SetWindowsHookEx函数安装在挂钩链中的挂钩过程
	/// </summary>
	/// <remarks>即使在UnhookWindowsHookEx返回后，挂钩过程也可能处于被另一个线程调用的状态。如果挂钩过程没有被并发调用，挂钩过程会在UnhookWindowsHookEx返回之前立即被移除。</remarks>
	/// <param name="hook_handle">要移除的挂钩过程的句柄，该参数通过SetWindowsHookEx返回值获得</param>
	/// <returns>操作是否成功</returns>
	inline bool remove_windows_hook(HHOOK hook_handle)
	{
		auto val = UnhookWindowsHookEx(hook_handle);
		GET_ERROR_MSG_OUTPUT();
		return val;
	}

	/// <summary>
	/// 将指定点的数组从一个窗口坐标系(from)映射到另一个窗口坐标系(to)
	/// </summary>
	/// <param name="window_handle_from">转换点的窗口句柄，若为NULL或HWND_DESKTOP，则假定点数组在屏幕坐标中</param>
	/// <param name="window_handle_to">点转换到的窗口句柄，若为NULL或HWND_DESKTOP，则将点转换为屏幕坐标</param>
	/// <param name="points">[in,out]POINT结构的数组，它也可以是RECT结构，在这种情况，points_count应为2</param>
	/// <param name="points_count">POINT结构数组的数量</param>
	/// <returns>若函数失败，返回值为0，否则低位是x轴之差，高位是y轴之差(from坐标点到to坐标点)</returns>
	inline int map_window_points(HWND window_handle_from, HWND window_handle_to, LPPOINT points, UINT points_count)
	{
		return MapWindowPoints(window_handle_from, window_handle_to, points, points_count);
	}

	/// <summary>
	/// 简单的窗口包装类，里面有一些有用的方法(持续更新)
	/// </summary>
	class window_instance {
	public:
		/// <summary>
		/// 窗口构造函数，默认句柄为NULL
		/// </summary>
		window_instance() : window_handle(nullptr) {}
		/// <summary>
		/// 窗口构造函数
		/// </summary>
		/// <param name="window_handle">窗口句柄</param>
		explicit window_instance(HWND window_handle) : window_handle(window_handle) {}
	public:
		/// <summary>
		/// 获取当前类存储的窗口句柄
		/// </summary>
		/// <returns>返回窗口句柄</returns>
		HWND get_handle() { return window_handle; }
		/// <summary>
		/// 设置当前类的窗口句柄
		/// </summary>
		/// <param name="window_handle">窗口句柄</param>
		void set_handle(HWND window_handle) { this->window_handle = window_handle; }
		/// <summary>
		/// 该成员函数可以调用指定窗口函数并对其传入存储的句柄
		/// </summary>
		/// <param name="f">指定要调用的函数</param>
		/// <param name="...args">函数的参数(除开第一个句柄)</param>
		/// <returns>返回该函数的返回值</returns>
		template <typename Function, typename... Args>
		auto to(Function f, Args&&... args)
		{
			return f(window_handle, std::forward<Args>(args)...);
		}
	private:
		HWND window_handle;
	};

	/// <summary>
	/// 回调函数模板工具类，使用字典查询关键字调用相应函数的模式，适用于所有的Windows回调函数。dict_depth表示关键字的个数，或字典嵌套的深度。
	/// </summary>
	/// <remarks>
	/// 该模板已经被弃用，不要再使用它
	/// </remarks>
	/// <typeparam name="dict_type">字典类型，这应该是一个完整的字典类型</typeparam>
	/// <typeparam name="default_process_function_type">默认处理函数的类型，当查询不到关键字对应的值时调用进行默认处理</typeparam>
	template<int dict_depth, typename _dict_type, typename _default_process_function_type>
	class procedure_dict_tools {
	private:
		/// <summary>
		/// 获取嵌套字典的最终的value类型(递归)
		/// </summary>
		/// <typeparam name="current_type">当前类型</typeparam>
		template<int depth, typename current_type>
		struct get_dict_value_type {
			using type = typename get_dict_value_type<depth - 1, typename current_type::mapped_type>::type;
		};

		/// <summary>
		/// 获取嵌套字典的最终的value类型(终点)
		/// </summary>
		/// <typeparam name="current_type">当前类型</typeparam>
		template<typename current_type>
		struct get_dict_value_type<1, current_type> {
			using type = typename current_type::mapped_type;
		};

		/// <summary>
		/// 获取嵌套字典的最终的key类型(递归)
		/// </summary>
		/// <typeparam name="current_type">当前类型</typeparam>
		template<int depth, typename current_type>
		struct get_dict_key_type {
			using type = typename get_dict_key_type<depth - 1, typename current_type::mapped_type>::type;
		};

		/// <summary>
		/// 获取嵌套字典的最终的key类型(终点)
		/// </summary>
		/// <typeparam name="current_type">当前类型</typeparam>
		template<typename current_type>
		struct get_dict_key_type<1, current_type> {
			using type = typename current_type::key_type;
		};

	public:
		/// <summary>
		/// 字典类型
		/// </summary>
		using dict_type = _dict_type;
		/// <summary>
		/// 默认处理函数类型
		/// </summary>
		using default_process_function_type = _default_process_function_type;
		/// <summary>
		/// 字典的key类型，对于嵌套字典，这是最顶层的key类型
		/// </summary>
		using dict_key_type = typename dict_type::key_type;
		/// <summary>
		/// 字典的value类型，对于嵌套字典，这是最顶层的value类型
		/// </summary>
		using dict_value_type = typename dict_type::mapped_type;
		/// <summary>
		/// 字典最终的key类型，对于嵌套字典，这是最里面的key类型。对于非嵌套字典，其类型等于dict_key_type
		/// </summary>
		using final_dict_key_type = typename get_dict_key_type<dict_depth, dict_type>::type;
		/// <summary>
		/// 字典最终的value类型，对于嵌套字典，这是最里面的value类型。对于非嵌套字典，其类型等于dict_value_type
		/// </summary>
		using final_dict_value_type = typename get_dict_value_type<dict_depth, dict_type>::type;

	private:
		/// <summary>
		/// 查询字典的模板函数，可以查找嵌套的字典，若未找到指定项，则返回false，否则返回true。使用keys_num指定嵌套的数量(或关键字的数量)
		/// </summary>
		/// <param name="val">[out]用于返回查询到的值</param>
		/// <param name="dict">用于查询的字典</param>
		/// <param name="key">第一个用于查询的key</param>
		/// <param name="...arg">其他需要查询的key，顺序应该是由外到内</param>
		/// <returns>若找到指定项，返回true，否则返回false</returns>
		template<int keys_num, typename dict_type, typename current_dict_key_type, typename... Args>
		static bool check_item(final_dict_value_type& val, dict_type& dict, current_dict_key_type key, Args&&... args)
		{
			auto iter = dict.find(key);
			if (iter == dict.end()) return false;

			return check_item<keys_num - 1>(val, iter->second, std::forward<Args>(args)...);
		}

		/// <summary>
		/// 查询字典的模板函数，若未找到指定项，则返回false，否则返回true。（该版本只适用于查询未嵌套的字典）
		/// </summary>
		/// <param name="val">[out]用于返回查询到的值</param>
		/// <param name="dict">用于查询的字典</param>
		/// <param name="key">用于查询的key</param>
		/// <returns>若找到指定项，返回true，否则返回false</returns>
		template<int keys_num = 1, typename dict_type, typename... Args>
		static bool check_item(final_dict_value_type& val, dict_type& dict, final_dict_key_type key, Args&&... args)
		{
			auto iter = dict.find(key);
			if (iter == dict.end()) return false;

			val = iter->second;
			return true;
		}

		// 上面都是模板元工具，用于获取嵌套字典的值或值的类型
	public:
		/// <summary>
		/// 模板回调函数，适用于所有Windows的回调函数，通过字典关键字调用特定的函数，支持字典嵌套(即多关键字)
		/// </summary>
		/// <typeparam name="return_type">返回类型</typeparam>
		/// <param name="...args">回调函数需要的参数包</param>
		/// <returns>返回指定类型的值</returns>
		template<typename return_type, typename... Args>
		static return_type CALLBACK callback_function(Args... args)
		{
			final_dict_value_type f;
			return_type r;

			// 调用前置处理函数
			if (get_my_pref())
				get_my_pref()(args...);

			// 寻找特定消息对应的函数
			if (check_item<dict_depth>(f, get_dict(), args...))
				if (f(r, args...))
					return r;

			// 否则使用默认处理函数
			if (get_my_f())
				return get_my_f()(args...);
			else return return_type();
		}

		/// <summary>
		/// 设置默认的可调用对象，用于当回调函数模板未找到对应key的value时调用
		/// </summary>
		/// <param name="f">默认可调用对象，可以是函数指针，lambda表达式，或重载了()运算符的类</param>
		static void set_default_process_function(default_process_function_type f)
		{
			get_my_f() = f;
		}

		/// <summary>
		/// 设置前置的可调用对象，模板回调函数第一个调用的函数，然后查询对应的处理函数，若未找到，则使用默认处理函数
		/// </summary>
		/// <param name="f">前置可调用对象，可以是函数指针，lambda表达式，或重载了()运算符的类</param>
		static void set_pre_process_function(default_process_function_type f)
		{
			get_my_pref() = f;
		}

		/// <summary>
		/// 将一个项加入到字典
		/// </summary>
		/// <param name="key">指定关键字</param>
		/// <param name="val">指定对应值</param>
		/// <returns>操作是否成功</returns>
		static bool add_item_to_dict(const dict_key_type& key, const dict_value_type& val)
		{
			auto return_value = get_dict().insert(std::pair(key, val));
			return return_value.second;
		}

		/// <summary>
		/// 从字典删除一个项
		/// </summary>
		/// <param name="key">指定项的关键字</param>
		/// <returns>操作是否成功</returns>
		static bool remove_item_from_dict(const dict_key_type& key)
		{
			return (get_dict().erase(key) == 1);
		}

		/// <summary>
		/// 使用指定可调用对象f和参数包args创建一个key，并将key和对应的value添加到字典中
		/// </summary>
		/// <param name="val">新创建的key对应的value值</param>
		/// <param name="f">可调用对象，该对象能返回一个key类型</param>
		/// <param name="...args">用于f的参数包</param>
		/// <returns>返回新创建的key</returns>
		template<typename create_function, typename... Args>
		static dict_key_type create_key_and_add(const dict_value_type& val, create_function f, Args&&... args)
		{
			auto key = f(std::forward<Args>(args)...);
			add_item_to_dict(key, val);
			return key;
		}

		
		/// <summary>
		/// 使用指定可调用对象和参数包args销毁指定key对应的对象或数据，并将key从字典中移除
		/// </summary>
		/// <param name="key">指定要删除和销毁的key</param>
		/// <param name="f">可调用对象，该对象能销毁key对应的对象或数据</param>
		/// <param name="...args">用于f的参数包</param>
		/// <returns>销毁操作是否成功</returns>
		template<typename destroy_function, typename... Args>
		static bool destroy_key_and_remove(const dict_key_type& key, destroy_function f, Args&&... args)
		{	
			return f(std::forward<Args>(args)...) && remove_item_from_dict(key);
		}


	public:
		/// <summary>
		/// 默认处理函数
		/// </summary>
		static auto& get_my_f()
		{
			static std::function<default_process_function_type> my_f;
			return my_f;
		}

		/// <summary>
		/// 前置处理函数
		/// </summary>
		static auto& get_my_pref()
		{
			static std::function<default_process_function_type> my_pref;
			return my_pref;
		}

	public:
		/// <summary>
		/// 静态变量字典包装函数，只有声明为静态或全局，窗口过程才能访问到这个字典
		/// </summary>
		/// <returns>字典</returns>
		static auto& get_dict()
		{
			static dict_type dict;
			return dict;
		}
	};

	// 针对窗口的实例化
	using window_dict [[deprecated]] = mw::user::procedure_dict_tools<2,
		std::unordered_map<HWND, std::unordered_map<UINT, std::function<bool(LRESULT&, HWND, UINT, WPARAM, LPARAM)>>>,
		decltype(mw::user::default_window_procedure)>;

	// 针对对话框的实例化
	using dialog_dict [[deprecated]] = mw::user::procedure_dict_tools<2,
		std::unordered_map<HWND, std::unordered_map<UINT, std::function<bool(INT_PTR&, HWND, UINT, WPARAM, LPARAM)>>>,
		INT_PTR(HWND, UINT, WPARAM, LPARAM)>;

};//user
};//mw