#include "stdafx.h"
#include "example_9.h"
#include <fstream>

// 本篇是一个截图示例

// 截图矩阵
RECT rect {};
// 截图窗口句柄
HWND capture_window = nullptr;
// 主窗口句柄
HWND main_window_handle = nullptr;

// 热键ID
constexpr int start_capture_id = 10086;
constexpr int finish_capture_id = 10087;
constexpr int cancle_capture_id = 10088;

// 自定义消息，由截图窗口发送给主窗口，表示捕获结束
constexpr int finish_capture = WM_USER + 0x40;


// 截图窗口
LRESULT CALLBACK capture_procedure(
    HWND window_handle,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    static POINT first_point {};
    static POINT second_point {};
    static bool is_start = false;
    static bool is_over = false;

    auto calculate_point = [&] {
        int x = second_point.x - first_point.x;
        int y = second_point.y - first_point.y;

        if (second_point.x >= first_point.x)
        {
            rect.left = first_point.x;
            rect.right = second_point.x;
        } else {
            rect.left = second_point.x;
            rect.right = first_point.x;
        }

        if (second_point.y >= first_point.y)
        {
            rect.top = first_point.y;
            rect.bottom = second_point.y;
        } else {
            rect.top = second_point.y;
            rect.bottom = first_point.y;
        }
    };

    switch (message)
    {
    case WM_CREATE:
    {
        is_over = false;
        is_start = false;
        memset(&rect, 0, sizeof(rect));
        memset(&first_point, 0, sizeof(first_point));
        memset(&second_point, 0, sizeof(second_point));
        mw::user::register_hotkey(window_handle, finish_capture_id, 'F', MOD_ALT | MOD_CONTROL | MOD_SHIFT);
        mw::user::register_hotkey(window_handle, cancle_capture_id, 'C', MOD_ALT | MOD_CONTROL | MOD_SHIFT);
        return 0;
    }
    case WM_PAINT:
    {
        if (is_start)
        {
            RECT rc {};
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(window_handle, &ps);
            SelectObject(hdc, GetStockObject(BLACK_BRUSH));
            Rectangle(hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom);
            HPEN hpen = CreatePen(PS_INSIDEFRAME, 5, RGB(60, 210, 60));
            HPEN holdpen = (HPEN)::SelectObject(hdc, hpen);
            SelectObject(hdc, ::GetStockObject(WHITE_BRUSH));
            Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
            SelectObject(hdc, holdpen);
            DeleteObject(hpen);
            EndPaint(window_handle, &ps);
        }
        return 0;
    }
    case WM_LBUTTONDOWN:
    {
        if (!is_start && !is_over)
        {
            is_start = true;
            first_point.x = LOWORD(lParam);
            first_point.y = HIWORD(lParam);
            second_point.x = LOWORD(lParam);
            second_point.y = HIWORD(lParam);
            calculate_point();
        }
        return 0;
    }
    case WM_LBUTTONUP:
    {
        if (is_start && !is_over)
        {
            is_over = true;
            second_point.x = LOWORD(lParam);
            second_point.y = HIWORD(lParam);
            calculate_point();
            //mw::user::send_message(window_handle, WM_PAINT, 0, 0);
            RedrawWindow(window_handle, nullptr, nullptr, RDW_UPDATENOW);
        }
        return 0;
    }
    case WM_MOUSEMOVE:
    {
        if (is_start && !is_over)
        {
            second_point.x = LOWORD(lParam);
            second_point.y = HIWORD(lParam);
            calculate_point();
            RedrawWindow(window_handle, nullptr, nullptr, RDW_UPDATENOW | RDW_INVALIDATE);
        }
        return 0;
    }
    case WM_HOTKEY:
    {
        switch (wParam)
        {
        case finish_capture_id:
        {
            if (is_start && is_over && (rect.left <= rect.right && rect.top <= rect.bottom))
                mw::user::post_message(main_window_handle, finish_capture, 0, 0);
            mw::user::send_message(window_handle, WM_CLOSE, 0, 0);
            return 0;
        }
        case cancle_capture_id:
        {
            mw::user::send_message(window_handle, WM_CLOSE, 0, 0);
            return 0;
        }
        default:
            break;
        }
        return 0;
    }
    case WM_CLOSE:
    {
        capture_window = nullptr;
        mw::user::destroy_window(window_handle);
        return 0;
    }
    default:
        return mw::user::default_window_procedure(window_handle, message, wParam, lParam);
    }
    return 0;
} 

//#pragma warning(disable : 4996)
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include "stb_image_write.h"

// 主对话框
BOOL CALLBACK dialog_procedure(
    HWND dialog_handle,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    static const auto screen_width = mw::get_system_metrics(SM_CXSCREEN);
    static const auto screen_height = mw::get_system_metrics(SM_CYSCREEN);

    auto capture_func = [&]() {
        mw::user::register_window_class(capture_procedure, _T("capture_class"), nullptr, nullptr,
            (HBRUSH)GetStockObject(BLACK_BRUSH), nullptr, CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS);
        capture_window = mw::user::create_window(_T("capture_class"), _T("capture window"), 0, 0, screen_width, 
            screen_height, nullptr, nullptr, nullptr, WS_POPUP, WS_EX_TOOLWINDOW | WS_EX_LAYERED  | WS_EX_TOPMOST);
        mw::user::set_layered_window_attributes(capture_window, RGB(255, 255, 255), 0x70, LWA_COLORKEY | LWA_ALPHA);
        mw::user::show_window(capture_window);
    };

    switch (message)
    {
    case WM_INITDIALOG:
    {
        mw::user::register_hotkey(dialog_handle, start_capture_id, VK_F1, MOD_ALT | MOD_CONTROL | MOD_SHIFT);
        return true;
    }
    case WM_COMMAND:
    {
        WORD control_message = HIWORD(wParam);
        WORD control_id = LOWORD(wParam);
        HWND control_handle = (HWND)lParam;
        switch (control_id)
        {
        case IDCAPTURE:
            if (capture_window)
                return false;
            capture_func();
            break;
        default:
            break;
        }
    } break;
    case WM_HOTKEY:
    {
        if (capture_window)
            return false;
        capture_func();
    } break;
    case finish_capture:
    {
        std::wstringstream o;
        o << rect.left << _T(",") << rect.top << _T(" ") << rect.right << _T(",") << rect.bottom;
        mw::user::set_dialog_item_text(dialog_handle, IDC_RAW_TEXT, o.str());
        //std::stringstream file_stream;
        //BITMAPINFOHEADER info_header {};

        //mw::gdi::grab_screen_area_raw(rect, file_stream, info_header);
        //stbi_write_jpg("1.jpg", info_header.biWidth, info_header.biHeight, 4, file_stream.str().c_str(), 60);

        //std::ofstream file_out("1.jpg");
        //file_out << file_stream.rdbuf();
        //file_out.close();
        return true;
    }
    case WM_CLOSE:
    {
        mw::user::destroy_window(dialog_handle);      // 如果是非模态对话框
        return true;
    }
    case WM_DESTROY: // 如果该对话框是非模态对话框，且是主窗口
    {
        mw::user::post_quit_message(0);
        return true;
    }
    }
    return false;
} 


void example_9()
{
    //stbi_flip_vertically_on_write(true);
    auto dialog_handle = mw::user::create_modeless_dialog(IDD_DIALOG2, (DLGPROC)dialog_procedure);
    mw::user::show_window(dialog_handle);
    main_window_handle = dialog_handle;


    MSG msg;
    while (mw::user::get_message(msg))
    {
        if (!mw::user::is_dialog_message(dialog_handle, msg))
        {
            mw::user::translate_key_to_character(msg);
            mw::user::dispatch_message(msg);
        }
    }
}