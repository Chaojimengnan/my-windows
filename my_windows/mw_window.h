#pragma once
#include <unordered_map>

namespace mw {
namespace user {

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
		GET_ERROR_MSG_OUTPUT(std::cout);
		return val;
	}

	/// <summary>
	/// 在系统中寻找对应窗口类和窗口名字的`顶级窗口`句柄(不搜寻子窗口，不区分大小写)
	/// </summary>
	/// <param name="class_name">窗口类的名字，若为""，它将查找标题与window_name匹配的任何窗口</param>
	/// <param name="window_name">窗口的名字，若为""，则所有窗口名称都匹配</param>
	/// <returns>返回对应的窗口句柄(操作失败返回NULL)</returns>
	inline HWND find_window(const std::string& class_name = "", const std::string& window_name = "")
	{
		auto val = FindWindowA( string_to_pointer(class_name), string_to_pointer(window_name));
		GET_ERROR_MSG_OUTPUT(std::cout);
		return val;
	}

	/// <summary>
	/// 在指定父窗口句柄中寻找对应窗口类和窗口名字的句柄(不区分大小写)
	/// </summary>
	/// <param name="class_name">窗口类的名字，若为""，它将查找标题与window_name匹配的任何窗口</param>
	/// <param name="window_name">窗口的名字，若为""，则所有窗口名称都匹配</param>
	/// <param name="parent_window">指定要搜索哪个父窗口的子窗口，若为NULL，父窗口为桌面窗口</param>
	/// <param name="child_after">从该子窗口句柄(必须是直接子窗口)之后开始搜索，顺序是Z轴顺序，若为NULL，则从第一个开始</param>
	/// <returns>返回对应的窗口句柄(操作失败返回NULL)</returns>
	inline HWND find_child_window(const std::string& class_name = "",
		const std::string& window_name = "", HWND parent_window = nullptr, HWND child_after = nullptr)
	{
		auto val = FindWindowExA( parent_window, child_after,
			string_to_pointer(class_name), string_to_pointer(window_name));
		GET_ERROR_MSG_OUTPUT(std::cout);
		return val;
	}

	/// <summary>
	/// 获取窗口句柄对应的窗口类的名字(GetClassNameA)
	/// </summary>
	/// <param name="window_handle">窗口句柄</param>
	/// <param name="class_name">[out]窗口类的名字</param>
	/// <returns>操作是否成功</returns>
	inline bool get_window_class_name(HWND window_handle, std::string& class_name)
	{
		CHAR temp_str[MW_MAX_TEXT] = { 0 };
		auto is_ok = GetClassNameA( window_handle, temp_str, MAX_PATH);
		GET_ERROR_MSG_OUTPUT(std::cout);
		class_name = temp_str;
		return is_ok;
	}

	/// <summary>
	/// 获取指定窗口的类型字符串(RealGetWindowClassA)
	/// </summary>
	/// <param name="window_handle">窗口句柄</param>
	/// <param name="class_name">[out]窗口类型字符串</param>
	/// <returns>操作是否成功</returns>
	inline bool get_window_type(HWND window_handle, std::string& class_name)
	{
		CHAR temp_str[MW_MAX_TEXT] = { 0 };
		auto is_ok = RealGetWindowClassA( window_handle, temp_str, MAX_PATH);
		GET_ERROR_MSG_OUTPUT(std::cout);
		class_name = temp_str;
		return is_ok;
	}

	/// <summary>
	/// 获取窗口标题文本(如果有的话)，如果窗口是控件，则获取控件的文本。注意该函数无法检索非本程序的控件的文本。
	/// </summary>
	/// <param name="window_handle">窗口句柄</param>
	/// <param name="text">[out]文本</param>
	/// <returns>操作是否成功</returns>
	inline bool get_window_text(HWND window_handle, std::string& text)
	{
		CHAR temp_str[MW_MAX_TEXT] = { 0 };
		auto is_ok = GetWindowTextA( window_handle, temp_str, MW_MAX_TEXT);
		GET_ERROR_MSG_OUTPUT(std::cout);
		text = temp_str;
		return is_ok;
	}


	/// <summary>
	/// 更改指定窗口标题栏的文本（如果有的话）。如果指定的窗口是控件，则更改控件的文本。注意该函数无法修改非本程序的控件的文本。
	/// </summary>
	/// <param name="window_handle"></param>
	/// <param name="text"></param>
	/// <returns></returns>
	inline bool set_window_text(HWND window_handle, const std::string& text)
	{
		auto val = SetWindowTextA( window_handle, text.c_str());
		GET_ERROR_MSG_OUTPUT(std::cout);
		return val;
	}


	/// <summary>
	/// 改变指定窗口的属性，该函数也可以在额外窗口内存中的指定偏移处设置一个值。
	/// </summary>
	/// <remarks>
	/// 封装自`SetWindowLongPtrA`，关于index的可选值请看文档
	/// </remarks>
	/// <param name="window_handle">窗口句柄</param>
	/// <param name="index">指定偏移量，具体可选值看文档</param>
	/// <param name="new_attribute">新属性的值</param>
	/// <returns>如果成功，返回值是前一个指定的偏移量，若之前没有设置，则返回0。函数在失败时也返回0</returns>
	inline LONG_PTR set_window_attribute(HWND window_handle, int index, LONG_PTR new_attribute)
	{
		auto val = SetWindowLongPtrA(window_handle, index, new_attribute);
		SetLastError(0);
		GET_ERROR_MSG_OUTPUT(std::cout);
		return val;
	}


	/// <summary>
	/// 改变指定窗口类的属性，该函数也可以在额外窗口类内存中的指定偏移处设置一个值(实际就是改系统维护的这个窗口类的WNDCLASSEX)
	/// </summary>
	/// <remarks>
	/// 封装自`SetClassLongPtrA`，关于index的可选值请看文档
	/// </remarks>
	/// <param name="window_handle">属于指定窗口类的窗口的句柄</param>
	/// <param name="index">指定偏移量，具体可选值看文档</param>
	/// <param name="new_attribute">新属性的值</param>
	/// <returns>如果成功，返回值是前一个指定的偏移量，若之前没有设置，则返回0。函数在失败时也返回0</returns>
	inline ULONG_PTR set_window_class_attribute(HWND window_handle, int index, LONG_PTR new_attribute)
	{
		auto val = SetClassLongPtrA(window_handle, index, new_attribute);
		GET_ERROR_MSG_OUTPUT(std::cout);
		return val;
	}

	/// <summary>
	/// 获取指定窗口类名字的WNDCLASSEXA信息，一般用于超类化一个系统全局窗口类
	/// </summary>
	/// <param name="window_class_name">指定窗口类名字</param>
	/// <param name="output_window_class">指定窗口类的WNDCLASSEXA信息副本</param>
	/// <param name="instance">一般为NULL，表示获取系统全局窗口类</param>
	/// <returns>若函数找到匹配数据，并成功复制数据，返回true，否则返回false</returns>
	inline bool get_window_class_info(const std::string& window_class_name,
		WNDCLASSEXA& output_window_class, HINSTANCE instance = nullptr)
	{
		auto val = GetClassInfoExA(instance, window_class_name.c_str(), &output_window_class);
		GET_ERROR_MSG_OUTPUT(std::cout);
		return val;
	}


	/// <summary>
	/// 获取指定窗口的指定属性，该函数也可以在额外窗口内存中的指定偏移处获得一个值
	/// </summary>
	/// <remarks>
	/// 封装自`GetWindowLongPtrA`，关于index的可选值请看文档。
	/// 
	/// 如果之前没有调用过set_window_attribute的情况下请求额外窗口内存的某个值，该函数将返回0
	/// </remarks>
	/// <param name="window_handle">窗口句柄</param>
	/// <param name="index">指定偏移量，具体可选值看文档</param>
	/// <returns>若函数成功，返回请求的值，否则返回0</returns>
	inline LONG_PTR get_window_attribute(HWND window_handle, int index)
	{
		auto val = GetWindowLongPtrA( window_handle, index);
		GET_ERROR_MSG_OUTPUT(std::cout);
		return val;
	}


	/// <summary>
	/// 获取指定窗口类的指定属性，该函数也可以在额外窗口类内存中的指定偏移处获得一个值(实际上就是获取系统维护的这个窗口类的WNDCLASSEX)
	/// </summary>
	/// <remarks>
	/// 封装自`GetClassLongPtrA`，关于index的可选值请看文档
	/// </remarks>
	/// <param name="window_handle">属于指定窗口类的窗口的句柄</param>
	/// <param name="index">指定偏移量，具体可选值看文档</param>
	/// <returns>成功则返回请求的值，否则返回0</returns>
	inline ULONG_PTR get_window_class_attribute(HWND window_handle, int index)
	{
		auto val = GetClassLongPtrA(window_handle, index);
		GET_ERROR_MSG_OUTPUT(std::cout);
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
		GET_ERROR_MSG_OUTPUT(std::cout);
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
		GET_ERROR_MSG_OUTPUT(std::cout);
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
		GET_ERROR_MSG_OUTPUT(std::cout);
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
		return CallWindowProcA(window_procedure, hWnd, Msg, wParam, lParam);
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
		return DefWindowProcA(hWnd, Msg, wParam, lParam);
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
	MW_API ATOM register_window_class(WNDPROC procedure,
		const std::string& class_name = "my_class",
		HICON hIcon = nullptr, HCURSOR hCursor = nullptr,
		HBRUSH hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH),
		HICON hIconSm = nullptr, UINT style = CS_HREDRAW | CS_VREDRAW,
		const std::string& menu_name = "",
		int cbClsExtra = 0, int cbWndExtra = 0);



	/// <summary>
	/// 创建并返回一个窗口句柄
	/// </summary>
	/// <param name="window_class_name">窗口类的名字</param>
	/// <param name="window_name">窗口的名字</param>
	/// <param name="x">窗口的x位置</param>
	/// <param name="y">窗口的y位置</param>
	/// <param name="width">窗口的宽度</param>
	/// <param name="height">窗口的高度</param>
	/// <param name="window_parent">该窗口的父窗口句柄</param>
	/// <param name="menu">菜单句柄</param>
	/// <param name="lParam"></param>
	/// <param name="style">样式</param>
	/// <param name="ex_style">扩展样式</param>
	/// <returns>窗口句柄</returns>
	MW_API HWND create_window(const std::string& window_class_name, const std::string& window_name = "my window",
		int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int width = CW_USEDEFAULT, int height = CW_USEDEFAULT,
		HWND window_parent = nullptr, HMENU menu = nullptr,
		LPVOID lParam = nullptr, DWORD style = WS_OVERLAPPEDWINDOW, DWORD ex_style = 0);


	/// <summary>
	/// 注销注册的窗口类，注意，注销之前一定确保由该窗口类生成的窗口都已被销毁
	/// </summary>
	/// <param name="window_class_name">窗口类名字</param>
	/// <param name="ins">模块实例句柄</param>
	/// <returns>操作是否成功</returns>
	inline bool unregister_window_class(const std::string& window_class_name, HINSTANCE ins)
	{
		auto val = UnregisterClassA(window_class_name.c_str(), ins);
		GET_ERROR_MSG_OUTPUT(std::cout);
		return val;
	}


	/// <summary>
	/// 窗口类，可以生成窗口
	/// </summary>
	/// <remarks>
	/// 要自定义窗口过程，你需要先定义一个`event_function_dict_type`类型的函数字典，
	/// 该字典使用消息作为关键字，窗口过程会根据接受到的消息的值在这个字典中寻找对应的可调用对象，它不仅可以是
	/// 函数指针，还可以是lambda表达式，也可以是一个可调用类型，只要满足`bool foo(HWND, UINT, WPARAM, LPARAM, LRESULT&)`
	/// 这样的形式就可以了。其中HWND为窗口句柄，UINT为消息标识符,WPARAM, LPARAM分别是两个额外消息参数，LRESULT&表示窗口过程的返回值，当你的可调用
	/// 对象返回true时，窗口过程使用这个返回值进行返回，若返回false，窗口过程会检查你是否定义了默认处理函数，若没有定义，窗口过程则调用默认窗口过程函数，
	/// 并返回它的返回值。定义默认处理函数和定义一般的消息处理函数一样，你要在事件函数字典中定义key为`DEFALT_PROCESS_FUNCTION`的项，然后对应的函数
	/// 即为默认处理函数，注意`DEFALT_PROCESS_FUNCTION` = 0x7FFF，这意味着你不能再自定义值为0x7FFF的消息，因为窗口过程会将其作为默认处理函数调用。
	/// 尽管key为`DEFALT_PROCESS_FUNCTION`，窗口过程在调用默认处理函数时，依然会传入正确的消息标识符，而不是`DEFALT_PROCESS_FUNCTION`。
	/// 该功能一般用于超类或子类的窗口过程，这样你就可以在`DEFALT_PROCESS_FUNCTION`调用被超类或子类的原窗口过程。
	/// 
	/// 你可以不定义任何消息的可调用对象，仅仅只需要定义一个`event_function_dict_type`类型的函数字典，当你不定义任何信息的可调用对象时
	/// ，窗口过程就只会调用默认窗口过程函数了。
	/// 
	/// 注意，除了调用你写的函数或默认窗口过程函数之外，窗口过程没有再做任何其他事情，你可以完全定制你自己的消息函数就像是定义窗口过程一样。
	/// 
	/// 例如：我想只想定义一个`WM_PAINT`(绘制消息)和带有默认处理函数的函数字典，可以这么写:
	/// ```cpp
	/// mw::window_class::event_function_dict_type my_event;
	/// 
	/// // 接收到WM_PAINT时调用
	/// my_event[WM_PAINT] = [](HWND hwnd, WPARAM, LPARAM, LRESULT&)->bool {
	///		PAINTSTRUCT ps;
	///		HDC hdc = BeginPaint(hwnd, &ps);
	///
	///		MoveToEx(hdc, 30, 10, NULL);
	///		LineTo(hdc, 20, 50);
	///		LineTo(hdc, 50, 20);
	///		LineTo(hdc, 10, 20);
	///		LineTo(hdc, 40, 50);
	///		LineTo(hdc, 30, 10);
	/// 
	///		EndPaint(hwnd, &ps);
	///		return true;
	/// };
	/// 
	/// // 当接收到任何事件函数字典没有定义的消息时调用，若不定义默认处理函数，则调用系统定义的默认窗口函数
	/// my_event[mw::user::window_class::DEFALT_PROCESS_FUNCTION] = 
	/// [](HWND hwnd, UINT msg, WPARAM w, LPARAM l, LRESULT& errorcode)->bool
	/// {
	/// 	errorcode = DefWindowProcW(hwnd, msg, w, l);
	/// 	return true;
	/// };
	/// ```
	/// </remarks>
	class MW_API window_class {
	public:
		/// <summary>
		/// 窗口消息类型
		/// </summary>
		using message_type = UINT;
		/// <summary>
		/// 事件消息函数字典类型，根据消息的类型来调用相应的函数
		/// </summary>
		/// <remarks>
		/// 你应该实现bool foo(HWND, WPARAM, LPARAM, LRESULT&)这样的可调用对象，返回值若为false，
		/// 则调用系统默认处理函数，否则不调用。LRESULT为返回代码，当返回值为true时，使用该代码返回，默认为0
		/// </remarks>
		using event_function_dict_type = std::unordered_map<message_type, std::function<bool(HWND, UINT, WPARAM, LPARAM, LRESULT&)>>;
		/// <summary>
		/// 句柄字典类型，根据句柄来获得对应的事件消息函数字典
		/// </summary>
		using handle_dict_type = std::unordered_map<HWND, event_function_dict_type>;
		/// <summary>
		/// 调用默认处理函数消息，一般用于子类化或超类化的窗口过程，若未指定该消息对应的函数，则使用默认窗口过程函数处理
		/// </summary>
		static constexpr int DEFALT_PROCESS_FUNCTION = 0x7FFF;
	public:
		/// <summary>
		/// 窗口类构造函数，注意，该类有可能构造失败，因为在构造函数中调用了注册窗口类函数，使用前先调用is_vaild()来查看是否成功
		/// </summary>
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
		explicit window_class(const event_function_dict_type& event_function_dict, 
			const std::string& class_name = "my_class", 
			HICON hIcon = nullptr, HCURSOR hCursor = nullptr,
			HBRUSH hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH), 
			HICON hIconSm = nullptr, UINT style = CS_HREDRAW | CS_VREDRAW,
			const std::string& menu_name = "",
			int cbClsExtra = 0, int cbWndExtra = 0);
	public:
		/// <summary>
		/// 创建并返回一个窗口句柄，默认窗口名字为my window
		/// </summary>
		HWND create();
		/// <summary>
		/// 创建并返回一个窗口句柄
		/// </summary>
		/// <param name="window_name">窗口的名字</param>
		HWND create(const std::string& window_name);
		/// <summary>
		/// 创建并返回一个窗口句柄
		/// </summary>
		/// <param name="window_name">窗口的名字</param>
		/// <param name="width">窗口的宽度</param>
		/// <param name="height">窗口的高度</param>
		/// <param name="x">窗口的x位置</param>
		/// <param name="y">窗口的y位置</param>
		HWND create(const std::string& window_name, int width, int height, int x, int y );
		/// <summary>
		/// 创建并返回一个窗口句柄
		/// </summary>
		/// <param name="window_name">窗口的名字</param>
		/// <param name="x">窗口的x位置</param>
		/// <param name="y">窗口的y位置</param>
		/// <param name="width">窗口的宽度</param>
		/// <param name="height">窗口的高度</param>
		/// <param name="window_parent">该窗口的父窗口句柄</param>
		/// <param name="menu">菜单句柄</param>
		/// <param name="lParam"></param>
		/// <param name="style">样式</param>
		/// <param name="ex_style">扩展样式</param>
		/// <returns>窗口句柄</returns>
		HWND create(const std::string& window_name, int x, int y, int width, int height
			, HWND window_parent, HMENU menu, LPVOID lParam, DWORD style, DWORD ex_style);
		/// <summary>
		/// 注销注册的窗口类，注意，注销之前一定确保由该窗口类生成的窗口都已被销毁
		/// </summary>
		/// <remarks>一般来说不需要自己去unregister</remarks>
		/// <returns>操作是否成功</returns>
		bool unregister_class();
		/// <summary>
		/// 检查该类是否构造成功
		/// </summary>
		/// <returns>是否构造成功</returns>
		bool is_vaild() { return vaild; }
	private:
		// 句柄字典操作
		static handle_dict_type& get_handle_dict();
		static bool add_item_handle_dict(HWND window_handle, const event_function_dict_type& event_function_dict);
		static bool add_item_handle_dict(HWND window_handle, event_function_dict_type&& event_function_dict);
		static bool remove_item_handle_dict(HWND window_handle);
	public:
		/// <summary>
		/// 窗口过程，以HWND为key，调用事件函数字典来处理对应的消息。
		/// </summary>
		static LRESULT CALLBACK window_process(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	private:
		WNDCLASSEXA win_class;
		ATOM win_class_id;
		bool vaild = true;
		event_function_dict_type event_function_dict;
	};

	/// <summary>
	/// 简单的窗口包装类，里面有一些有用的方法(持续更新)
	/// </summary>
	class MW_API window_instance {
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
		/// 以指定模式显示窗口
		/// </summary>
		/// <param name="iCmdShow">显示模式</param>
		/// <returns>操作是否成功</returns>
		bool show_window(int iCmdShow = SW_SHOWNORMAL);
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
	/// 如我们想要用该模板实例化一个窗口过程，可以这么写：
	/// ```cpp
	/// // 先写一个类型别名，由于窗口过程需要两个关键字，一个是窗口句柄HWND，一个是消息标识符UINT。
	/// // 没有句柄就无法找到指定窗口的处理函数字典，没有消息标识符就无法找到指定窗口的指定消息的处理函数
	/// // 所以应该使用一个嵌套字典，其深度为2，即如下指定。我们还需要一个默认处理函数，所以我们使用
	/// // `decltype(mw::user::default_window_procedure)`来指定默认处理函数的类型
	/// using window_dict = mw::user::procedure_dict_tools<2,
	/// std::unordered_map<HWND, std::unordered_map<UINT, std::function<bool(LRESULT&, HWND, UINT, WPARAM, LPARAM)>>>,
	/// 	decltype(mw::user::default_window_procedure)> ;
	/// 
	/// // 然后我们需要将默认函数的指针传进去
	/// window_dict::set_default_process_function(mw::user::default_window_procedure);
	/// 
	/// // 使用包装函数注册一个窗口类，并将我们的实例化的模板函数传进去，注意，由于是模板，所以
	/// // 实例化代码是放在你的程序的，若你想将其放在DLL中，那么就在DLL中实例化模板
	/// mw::user::register_window_class(window_dict::callback_function<LRESULT, HWND, UINT, WPARAM, LPARAM>, "my_class");
	/// 
	/// // 现在我们需要创建子字典，该字典用于指定特定窗口的消息字典
	/// window_dict::dict_value_type my_event;
	/// 
	/// // 注意，最终调用的函数的第一个参数，类型必须是LRESULT的引用，窗口用这个值来返回，
	/// // 且返回值必须是布尔值，若返回true，系统将不调用默认处理函数，否则将调用，且用它的返回值返回
	/// my_event[WM_PAINT] = [](LRESULT&, HWND hwnd, UINT, WPARAM, LPARAM)->bool {
	///		PAINTSTRUCT ps;
	///		HDC hdc = BeginPaint(hwnd, &ps);
	///
	///		MoveToEx(hdc, 30, 10, nullptr);
	///		LineTo(hdc, 20, 50);
	///		LineTo(hdc, 50, 20);
	///		LineTo(hdc, 10, 20);
	///		LineTo(hdc, 40, 50);
	///		LineTo(hdc, 30, 10);
	///
	///		EndPaint(hwnd, &ps);
	///		return true;
	/// };
	/// 
	/// // 然后我们创建一个窗口，并将它的句柄和我们之前写的消息子字典一起添加进字典，这样字典上就登记了这个窗口句柄以及它对应的消息字典了。
	/// // 这样当模板回调函数被系统调用时，通过查询传入的句柄，就能找到我们写的消息字典，从而实现了我们的自定义消息操作
	/// // 注意，由于是先创建窗口，才能获得句柄，才能将句柄拿去注册字典，在这段时间中，可能已经错过了WM_CREATE等消息
	/// // 你若必须要在接收WM_CREATE时进行一些操作，那么你可以定义一个你自己的回调函数，然后自己捕获WM_CREATE等消息，然后其他消息传给
	/// // 模板回调函数即可，就像默认处理函数那样用。
	/// mw::user::window_instance dada(mw::user::create_window("my_class"));
	/// window_dict::add_item_to_dict(dada.get_handle(), my_event);
	/// ```
	/// </remarks>
	/// <typeparam name="dict_type">字典类型，这应该是一个完整的字典类型</typeparam>
	/// <typeparam name="default_process_function_type">默认处理函数的类型，当查询不到关键字对应的值时调用进行默认处理</typeparam>
	template<int dict_depth, typename dict_type, typename default_process_function_type>
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

			// 寻找特定消息对应的函数
			if (check_item<dict_depth>(f, get_dict(), args...))
				if (f(r, args...))
					return r;

			// 否则使用默认处理函数
			return my_f(args...);
		}

		/// <summary>
		/// 模板回调函数，无返回值的版本
		/// </summary>
		/// <param name="...args">回调函数需要的参数包</param>
		template<typename... Args>
		static void CALLBACK callback_function(Args... args)
		{
			final_dict_value_type f;

			// 寻找特定消息对应的函数
			if (check_item<dict_depth>(f, get_dict(), args...))
				if (f(args...))
					return;

			// 否则使用默认处理函数
			my_f(args...);
			return;
		}

		/// <summary>
		/// 设置默认的可调用对象，用于当回调函数模板未找到对应key的value时调用
		/// </summary>
		/// <param name="f">默认可调用对象，可以是函数指针，lambda表达式，或重载了()运算符的类</param>
		static void set_default_process_function(default_process_function_type f)
		{
			my_f = f;
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

	public:
		/// <summary>
		/// 默认处理函数的指针
		/// </summary>
		static default_process_function_type* my_f;

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

	/// <summary>
	/// 定义这个默认处理函数静态变量
	/// </summary>
	template<int dict_depth, typename dict_type, typename default_process_function_type>
	default_process_function_type* procedure_dict_tools<dict_depth, dict_type, default_process_function_type>::my_f = nullptr;


};//window
};//mw