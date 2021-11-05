#pragma once
#include <unordered_map>

namespace mw {

	class window_class;
	class window_instance;

	/// <summary>
	/// 该类型用于存储窗口实例以及它的窗口类的指针
	/// </summary>
	using window_refer = std::pair<window_instance*, window_class*>;

	/// <summary>
	/// 创建并返回一个指向`window_class`或它的派生类的shared指针
	/// </summary>
	/// <remarks>
	/// 强烈推荐使用该函数而不是自己创建一个`window_class`或它的派生类
	/// </remarks>
	/// <typeparam name="the_window_class">`window_class`或它的派生类的类型</typeparam>
	/// <typeparam name="...Args"></typeparam>
	/// <param name="...args">构造函数参数</param>
	/// <returns>一个指向`window_class`或它的派生类的shared指针</returns>
	template <typename the_window_class = window_class, typename... Args>
	inline std::shared_ptr<the_window_class> make_window_class(Args&&... args)
	{
		return std::shared_ptr<the_window_class>(new the_window_class(std::forward<Args>(args)...));
	}


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
	/// 窗口类，可以生成窗口实例
	/// </summary>
	/// <remarks>
	/// 你可以继承该类并重新实现事件函数以自定义窗口过程，或者直接使用该类创建一个最简单的窗口。
	/// 需要注意的是：当窗口类被销毁时，其生成的所有的窗口实例都会被销毁，所以务必将窗口类创建在堆中，
	/// 以确保其窗口实例不会被突然销毁掉。最好的办法是调用`make_window_class`创建一个指向`window_class`或其派生类的智能指针，
	/// 并确保在所有实例销毁之前,至少有一个智能指针指向它。
	/// 
	/// </remarks>
	class MW_API window_class
	{
	public:
		/// <summary>
		/// 窗口类构造函数
		/// </summary>
		/// <param name="class_name">窗口类名字</param>
		explicit window_class(const std::wstring& class_name);
		/// <summary>
		/// 窗口类构造函数
		/// </summary>
		/// <param name="class_name">窗口类名字</param>
		/// <param name="style">样式</param>
		/// <param name="hIcon">图标</param>
		/// <param name="hCursor">鼠标</param>
		/// <param name="hbrBackground">背景颜色</param>
		/// <param name="hIconSm">小图标</param>
		window_class(const std::wstring& class_name, UINT style, HICON hIcon, HCURSOR hCursor,
			HBRUSH hbrBackground, HICON hIconSm);

		/// <summary>
		/// 窗口类构造函数
		/// </summary>
		/// <param name="class_name">窗口类名字</param>
		/// <param name="menu_name">菜单名字</param>
		/// <param name="style">样式</param>
		/// <param name="cbClsExtra">额外空间</param>
		/// <param name="cbWndExtra">额外空间</param>
		/// <param name="hIcon">图标</param>
		/// <param name="hCursor">鼠标</param>
		/// <param name="hbrBackground">背景颜色</param>
		/// <param name="hIconSm">小图标</param>
		window_class(const std::wstring& class_name, const std::wstring & menu_name
			, UINT style, int cbClsExtra, int cbWndExtra, HICON hIcon, HCURSOR hCursor,
			HBRUSH hbrBackground, HICON hIconSm);
		virtual ~window_class();

		window_class(const window_class&) = delete;
		window_class(window_class&&) = delete;
		window_class& operator=(const window_class&) = delete;
		window_class& operator=(window_class&&) = delete;
	public:
		/// <summary>
		/// 创建并返回一个窗口实例
		/// </summary>
		/// <returns>由智能指针包装的窗口实例</returns>
		std::shared_ptr<window_instance> create();
		/// <summary>
		/// 创建并返回一个窗口实例
		/// </summary>
		/// <param name="window_name">窗口实例的名字</param>
		/// <returns>由智能指针包装的窗口实例</returns>
		std::shared_ptr<window_instance> create(const std::wstring& window_name);
		/// <summary>
		/// 创建并返回一个窗口实例
		/// </summary>
		/// <param name="window_name">窗口实例的名字</param>
		/// <param name="x">窗口的x位置</param>
		/// <param name="y">窗口的y位置</param>
		/// <param name="width">窗口的宽度</param>
		/// <param name="height">窗口的高度</param>
		/// <returns>由智能指针包装的窗口实例</returns>
		std::shared_ptr<window_instance> create(const std::wstring& window_name, int x, int y, int width, int height);
		/// <summary>
		/// 创建并返回一个窗口实例
		/// </summary>
		/// <param name="window_name">窗口实例的名字</param>
		/// <param name="x">窗口的x位置</param>
		/// <param name="y">窗口的y位置</param>
		/// <param name="width">窗口的宽度</param>
		/// <param name="height">窗口的高度</param>
		/// <param name="window_parent">该窗口的父窗口句柄</param>
		/// <param name="menu">菜单句柄</param>
		/// <param name="lParam"></param>
		/// <param name="style">样式</param>
		/// <param name="ex_style">扩展样式</param>
		/// <returns>由智能指针包装的窗口实例</returns>
		std::shared_ptr<window_instance> create(const std::wstring& window_name, int x, int y, int width, int height
			, HWND window_parent, HMENU menu, LPVOID lParam, DWORD style, DWORD ex_style);

	public:
		/// <summary>
		/// 返回当前窗口类的配置信息
		/// </summary>
		/// <returns>`WNDCLASSEX`</returns>
		const WNDCLASSEX get_window_class() const { return win_class; }

		/// <summary>
		/// 获得当前窗口生成的实例的数量
		/// </summary>
		/// <returns>返回当前窗口生成的实例的数量</returns>
		size_t get_instance_count() { return instance_vec.size(); }

		/// <summary>
		/// 获得当前窗口生成的存活的实例的数量
		/// </summary>
		/// <returns>返回当前窗口生成的存活的实例的数量</returns>
		size_t get_alive_instance_count() {
			size_t counts = 0;
			for (auto& i : instance_vec)
			{
				if (!i.expired())
					++counts;
			}
			return counts;
		}

		/// <summary>
		/// 获得所有存活的实例的数量
		/// </summary>
		/// <returns>返回所有存活的实例的数量</returns>
		static size_t get_all_alive_instance_counts() { return window_instance_count(); }

	protected:

		/// <summary>
		/// 当窗口过程接受到`WM_PAINT`消息时调用
		/// </summary>
		/// <param name="hwnd">窗口句柄</param>
		virtual void on_paint(HWND hwnd) {}

		/// <summary>
		/// 当窗口过程接受到`WM_CLOSE`消息时调用
		/// </summary>
		/// <param name="hwnd">窗口句柄</param>
		virtual void on_close(HWND hwnd) {}

		/// <summary>
		/// 当窗口过程接受到`WM_DESTROY`消息时调用
		/// </summary>
		/// <param name="hwnd">窗口句柄</param>
		virtual void on_destroy(HWND hwnd) {}

	private:
		WNDCLASSEX win_class;
		ATOM win_class_id;
		std::vector<std::weak_ptr<window_instance>> instance_vec;
		static size_t& window_instance_count();

	private:
		static LRESULT CALLBACK window_process(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

		// 由于WNDCLASSEX必须传入一个函数指针，而不是一个可调用对象，
		// 所以我创建了一个map，使用窗口句柄来映射对应的窗口实例和窗口类
		static window_refer get_window_refer(HWND hwnd);
		static bool add_window_refer_with_handle(HWND hwnd, window_class* win_class, window_instance* win_ins);
		static bool remove_window_refer_with_handle(HWND hwnd);
		static std::unordered_map<HWND, window_refer>& handle_map();
	};


	/// <summary>
	/// 窗口实例，用于管理窗口句柄，当被销毁时，对应的真实窗口也会被销毁
	/// </summary>
	/// <remarks>
	/// 注意：必须只用窗口类的create函数来创建窗口实例！因为窗口实例要与对应的窗口类关联
	/// 才能找到它的事件函数。窗口实例被销毁时，其真实窗口也会被销毁，所以一定注意至少有一个智能指针指向它。
	/// 由于真实窗口可能比窗口实例先被销毁，所以你必须先调用`is_vaild`函数来检查真实窗口是否还活着
	/// </remarks>
	class MW_API window_instance {
		friend class window_class;
	public:
		explicit window_instance(HWND window_handle);
		~window_instance();

		window_instance(const window_instance&) = delete;
		window_instance(window_instance&&) = delete;
		window_instance& operator=(const window_instance&) = delete;
		window_instance& operator=(window_instance&&) = delete;

	public:
		/// <summary>
		/// 检查真实窗口是否活着
		/// </summary>
		/// <returns>是否活着</returns>
		bool is_vaild() { return isvaild; }

		/// <summary>
		/// 显示窗口，当第一次创建后调用
		/// </summary>
		/// <param name="iCmdShow">显示模式</param>
		void show_window(int iCmdShow = SW_SHOWNORMAL);

		/// <summary>
		/// 获取当前窗口实例的句柄
		/// </summary>
		/// <returns>窗口句柄</returns>
		HWND get_handle();

		/// <summary>
		/// 销毁真实窗口
		/// </summary>
		/// <returns>真实窗口销毁后的返回值</returns>
		LRESULT destroy();
	private:
		HWND my_hwnd;
		bool isvaild;
	};

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
	/// mw::window_class_new::event_function_dict_type my_event;
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
	class MW_API window_class_new {
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
		window_class_new(const event_function_dict_type& event_function_dict);
		/// <summary>
		/// 窗口类构造函数，注意，该类有可能构造失败，因为在构造函数中调用了注册窗口类函数，使用前先调用is_vaild()来查看是否成功
		/// </summary>
		/// <param name="class_name">窗口类名字</param>
		/// <param name="event_function_dict">事件函数字典</param>
		explicit window_class_new(const std::string& class_name, const event_function_dict_type& event_function_dict);
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
		window_class_new(const std::string& class_name, const event_function_dict_type& event_function_dict, 
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
		window_class_new(const std::string& class_name, 
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
	class MW_API window_instance_new {
	public:
		/// <summary>
		/// 窗口构造函数，默认句柄为NULL
		/// </summary>
		window_instance_new() : window_handle(NULL) {}
		/// <summary>
		/// 窗口构造函数
		/// </summary>
		/// <param name="window_handle">窗口句柄</param>
		explicit window_instance_new(HWND window_handle) : window_handle(window_handle) {}
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