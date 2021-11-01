#pragma once
#include <unordered_map>

namespace mw {

	class window_class;
	class window_instance;
	using window_refer = std::pair<window_instance*, window_class*>;

	/* Inheirt this class to custom your own window class */
	class MW_API window_class
	{
	public:
		window_class(const std::wstring& class_name);
		window_class(const std::wstring& class_name, UINT style, HICON hIcon, HCURSOR hCursor,
			HBRUSH hbrBackground, HICON hIconSm);
		window_class(const std::wstring& class_name, const std::wstring & menu_name
			, UINT style, int cbClsExtra, int cbWndExtra, HICON hIcon, HCURSOR hCursor,
			HBRUSH hbrBackground, HICON hIconSm);
		virtual ~window_class();
	public:
		window_instance* create();
		window_instance* create(const std::wstring& window_name);
		window_instance* create(const std::wstring& window_name, int x, int y, int width, int height);
		window_instance* create(const std::wstring& window_name, int x, int y, int width, int height
			, HWND window_parent, HMENU menu, LPVOID lParam, DWORD style, DWORD ex_style);

	public:
		const WNDCLASSEX get_window_refer() const { return win_class; }

	protected:
		WNDCLASSEX child_get_wndclass() { return win_class; }

		/* Paint event for window */
		virtual void on_paint(HWND hwnd) {}

		/* Close event for window, before calling DestroyWindow */
		virtual void on_close(HWND hwnd) {}

		/* Destroy event for window */
		virtual void on_destroy(HWND hwnd) {}

	private:
		WNDCLASSEX win_class;
		ATOM win_class_id;
		size_t window_instance_count;

	private:
		static LRESULT CALLBACK window_process(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

		/*
		* std::bind cannot be used because a function pointer must be passed in, 
		* but since HWND uniquely specifies a window instance, which can be used to make a mapping.  
		*/
		static window_refer get_window_refer(HWND hwnd);
		static bool add_window_refer_with_handle(HWND hwnd, window_class* win_class, window_instance* win_ins);
		static bool remove_window_refer_with_handle(HWND hwnd);
		static std::unordered_map<HWND, window_refer>& handle_map();
	};

	class MW_API window_instance {
		friend class window_class;
	public:
		window_instance(HWND window_handle);
		~window_instance();
		bool is_vaild() { return isvaild; }
		void show_window(int iCmdShow = SW_SHOWNORMAL);
		HWND get_handle();
	private:
		HWND my_hwnd;
		bool isvaild;
	};
};//mw