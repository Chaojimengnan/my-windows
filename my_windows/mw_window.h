#pragma once
#include <unordered_map>

namespace mw {

	/// <summary>
	/// 寻找对应窗口类和窗口名字的`顶级窗口`句柄(不搜寻子窗口)
	/// </summary>
	/// <param name="class_name">窗口类的名字，若为""，它将查找标题与window_name匹配的任何窗口</param>
	/// <param name="window_name">窗口的名字，若为""，则所有窗口名称都匹配</param>
	/// <returns>返回对应的窗口句柄(操作失败返回NULL)</returns>
	MW_API HWND find_window(const std::string& class_name = "", const std::string& window_name = "");

	/// <summary>
	/// 获取窗口句柄对应的窗口类的名字
	/// </summary>
	/// <param name="window_handle">窗口句柄</param>
	/// <param name="class_name">[out]窗口类的名字</param>
	/// <returns>操作是否成功</returns>
	MW_API bool get_window_class_name(HWND window_handle, std::string& class_name);

	/// <summary>
	/// 获取窗口标题文本(如果有的话)，如果窗口是控件，则获取控件的文本。注意该函数无法检索非本程序的控件的文本。
	/// </summary>
	/// <param name="window_handle">窗口句柄</param>
	/// <param name="text">[out]文本</param>
	/// <returns>操作是否成功</returns>
	MW_API bool get_window_text(HWND window_handle, std::string& text);

	/// <summary>
	/// 获取一个桌面窗口的句柄
	/// </summary>
	/// <remarks>桌面窗口覆盖整个屏幕，桌面窗口是完全掌控其他窗口绘制的区域</remarks>
	/// <returns>返回桌面窗口句柄</returns>
	MW_API HWND get_desktop_window();


	/// <summary>
	/// 窗口类，可以生成窗口
	/// </summary>
	/// <remarks>
	/// 要自定义窗口过程，你需要先定义一个event_function_dict_type类型的函数字典，
	/// 该字典使用消息作为关键字，窗口过程会根据接受到的消息的值在这个字典中寻找对应的可调用对象，它不仅可以是
	/// 函数指针，还可以是lambda表达式，也可以是一个可调用类型，只要满足`bool foo(HWND, WPARAM, LPARAM, LRESULT&)`
	/// 这样的形式就可以了。其中HWND为窗口句柄，WPARAM, LPARAM分别是两个额外参数，LRESULT&表示窗口过程的返回值，当你的可调用
	/// 对象返回true时，窗口过程使用这个返回值进行返回，若返回false，窗口过程则调用系统默认处理函数，并返回它的返回值。
	/// 你可以不定义任何消息的可调用对象，仅仅只需要定义一个event_function_dict_type类型的函数字典，当你不定义任何信息的可调用对象时
	/// ，窗口过程就只会调用系统默认的处理函数了。
	/// 
	/// 注意，除了调用你写的函数或系统默认的处理函数之外，窗口过程没有再做任何其他事情，你可以完全定制你自己的信息函数就像是定义窗口过程一样。
	/// 
	/// 例如：我想只想定义一个WM_PAINT(绘制消息)处理函数，可以这么写:
	/// ```cpp
	/// mw::window_class::event_function_dict_type my_event;
	/// 
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
		using event_function_dict_type = std::unordered_map<message_type, std::function<bool(HWND, WPARAM, LPARAM, LRESULT&)>>;
		/// <summary>
		/// 句柄字典类型，根据句柄来获得对应的事件消息函数字典
		/// </summary>
		using handle_dict_type = std::unordered_map<HWND, event_function_dict_type>;
	public:
		/// <summary>
		/// 窗口类构造函数，注意，该类有可能构造失败，因为在构造函数中调用了注册窗口类函数，使用前先调用is_vaild()来查看是否成功
		/// </summary>
		/// <param name="event_function_dict">事件函数字典</param>
		window_class(const event_function_dict_type& event_function_dict);
		/// <summary>
		/// 窗口类构造函数，注意，该类有可能构造失败，因为在构造函数中调用了注册窗口类函数，使用前先调用is_vaild()来查看是否成功
		/// </summary>
		/// <param name="class_name">窗口类名字</param>
		/// <param name="event_function_dict">事件函数字典</param>
		explicit window_class(const std::string& class_name, const event_function_dict_type& event_function_dict);
		/// <summary>
		/// 窗口类构造函数，注意，该类有可能构造失败，因为在构造函数中调用了注册窗口类函数，使用前先调用is_vaild()来查看是否成功
		/// </summary>
		/// <param name="class_name">窗口类名字</param>
		/// <param name="event_function_dict">事件函数字典</param>
		/// <param name="style">样式</param>
		/// <param name="hIcon">图标</param>
		/// <param name="hCursor">鼠标</param>
		/// <param name="hbrBackground">背景颜色</param>
		/// <param name="hIconSm">小图标</param>
		window_class(const std::string& class_name, const event_function_dict_type& event_function_dict, 
			UINT style, HICON hIcon, HCURSOR hCursor,
			HBRUSH hbrBackground, HICON hIconSm);
		/// <summary>
		/// 窗口类构造函数，注意，该类有可能构造失败，因为在构造函数中调用了注册窗口类函数，使用前先调用is_vaild()来查看是否成功
		/// </summary>
		/// <param name="class_name">窗口类名字</param>
		/// <param name="event_function_dict">事件函数字典</param>
		/// <param name="menu_name">菜单名字</param>
		/// <param name="style">样式</param>
		/// <param name="cbClsExtra">额外空间</param>
		/// <param name="cbWndExtra">额外空间</param>
		/// <param name="hIcon">图标</param>
		/// <param name="hCursor">鼠标</param>
		/// <param name="hbrBackground">背景颜色</param>
		/// <param name="hIconSm">小图标</param>
		window_class(const std::string& class_name, 
			const event_function_dict_type& event_function_dict, const std::string& menu_name
			, UINT style, int cbClsExtra, int cbWndExtra, HICON hIcon, HCURSOR hCursor,
			HBRUSH hbrBackground, HICON hIconSm);
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
		/// <param name="x">窗口的x位置</param>
		/// <param name="y">窗口的y位置</param>
		/// <param name="width">窗口的宽度</param>
		/// <param name="height">窗口的高度</param>
		HWND create(const std::string& window_name, int x, int y, int width, int height);
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
	private:
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
		window_instance() : window_handle(NULL) {}
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
	private:
		HWND window_handle;
	};

};//mw