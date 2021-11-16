#pragma once

namespace mw {
namespace user {

	/// <summary>
	/// 创建并弹出一个消息框，消息框是一种特殊的模态对话框
	/// </summary>
	/// <param name="caption">对话框标题</param>
	/// <param name="text">对话框文本</param>
	/// <param name="type">对话框种类</param>
	/// <param name="parent_window">父窗口，默认没有</param>
	/// <param name="language_id">消息框按钮中显示的文本的语言，0表示以默认系统语言显示按钮文本</param>
	/// <returns>操作失败返回0，否则标识用户按下哪个按钮</returns>
	inline int message_box(const std::string& caption,
		const std::string& text, UINT type = MB_OK, HWND parent_window = nullptr, WORD language_id = 0)
	{
		auto val = MessageBoxExA(parent_window, text.c_str(), caption.c_str(), type, language_id);
		GET_ERROR_MSG_OUTPUT(std::cout);
		return val;
	}


	/// <summary>
	/// 从指定模板资源中创建一个模态对话框，该函数内部调用CreateWindowEx，并在创建之后发送WM_INITDIALOG消息给对话框过程。使用EndDialog而不是DestroyWindow来销毁模态对话框
	/// </summary>
	/// <remarks>
	/// 注意，该函数内部使用的Unicode版本，因为资源数据都是以Unicode存储的，而且不知道为何，若是用ANSI版本，中文只能显示一半。
	/// 另外，该函数直到模态对话框被销毁时才返回，这意味着在模态对话框销毁前不使用主程序的消息循环，而是使用系统的消息循环，
	/// 该系统定义的消息循环也会分发其他窗口的消息。
	/// 模态对话框不需要调用ShowWindow，系统自动显示
	/// </remarks>
	/// <param name="dialog_id">对话框资源ID</param>
	/// <param name="dialog_procedure">对话框过程，返回值应该是一个布尔值，若为false，对话框管理器执行默认对话框操作以响应消息</param>
	/// <param name="parent_handle">拥有者句柄，可以为nullptr，但不推荐，因为违反模态对话框的设计原理</param>
	/// <param name="init_param">初始化参数，将作为WM_INITDIALOG的lParam参数发送给对话框过程</param>
	/// <param name="module_instance">用于指定哪个模块包含指定模板资源，若在exe中置为NULL即可，若在DLL中，应该传入正确的DLL实例句柄</param>
	/// <returns>若函数成功，返回值是调用EndDialog时指定的返回值，若因为parent_handle无效而失败，返回0，若是其他原因失败返回-1</returns>
	inline INT_PTR create_modal_dialog(int dialog_id, DLGPROC dialog_procedure, HWND parent_handle, 
		LPARAM init_param = 0 , HINSTANCE module_instance = nullptr)
	{
		auto val = DialogBoxParamW(module_instance, MAKEINTRESOURCEW(dialog_id), parent_handle, dialog_procedure, init_param);
		GET_ERROR_MSG_OUTPUT(std::cout);
		return val;
	}

	/// <summary>
	/// 从内存中的对话框模板中创建一个模态对话框，该函数内部调用CreateWindowEx，并在创建之后发送WM_INITDIALOG消息给对话框过程。使用EndDialog而不是DestroyWindow来销毁模态对话框
	/// </summary>
	/// <remarks>
	/// 关于对话框模板更多细节，请看文档。注意模板内的所有字符串都必须是Unicode字符串
	/// 模态对话框不需要调用ShowWindow，系统自动显示
	/// </remarks>
	/// <param name="dialog_template">用于创建对话框的模板。对话框模板由一个描述对话框的标题组成，后跟一个或多个描述对话框中每个控件的附加数据块。</param>
	/// <param name="dialog_procedure">对话框过程，返回值应该是一个布尔值，若为false，对话框管理器执行默认对话框操作以响应消息</param>
	/// <param name="parent_handle">拥有者句柄，可以为nullptr，但不推荐，因为违反模态对话框的设计原理</param>
	/// <param name="init_param">初始化参数，将作为WM_INITDIALOG的lParam参数发送给对话框过程</param>
	/// <param name="module_instance">用于指定哪个模块包含指定模板资源，若在exe中置为NULL即可，若在DLL中，应该传入正确的DLL实例句柄</param>
	/// <returns>若函数成功，返回值是调用EndDialog时指定的返回值，若因为parent_handle无效而失败，返回0，若是其他原因失败返回-1</returns>
	inline INT_PTR create_modal_dialog_indirect(LPCDLGTEMPLATEA dialog_template, DLGPROC dialog_procedure, HWND parent_handle, 
		LPARAM init_param = 0, HINSTANCE module_instance = nullptr)
	{
		auto val = DialogBoxIndirectParamW(module_instance, dialog_template, parent_handle, dialog_procedure, init_param);
		GET_ERROR_MSG_OUTPUT(std::cout);
		return val;
	}

	/// <summary>
	/// 销毁一个模态对话框，若是非模态对话框请不要使用本函数，而是使用DestroyWindow来销毁非模态对话框。该函数将导致创建该对话框的函数返回。
	/// </summary>
	/// <remarks>对话框过程可以在任何时候调用该函数来销毁它的对话框</remarks>
	/// <param name="dialog_handle">要被销毁模态对话框的句柄</param>
	/// <param name="result">将作为创建该模态对话框函数的返回值</param>
	/// <returns>操作是否成功</returns>
	inline bool end_modal_dialog(HWND dialog_handle, INT_PTR result)
	{
		auto val = EndDialog(dialog_handle, result);
		GET_ERROR_MSG_OUTPUT(std::cout);
		return val;
	}

	/// <summary>
	/// 从指定模板资源中创建一个非模态对话框，该函数内部调用CreateWindowEx，并在创建之后发送WM_INITDIALOG消息给对话框过程。使用DestroyWindow来销毁非模态对话框。
	/// </summary>
	/// <remarks>
	/// 注意，该函数内部使用的Unicode版本，因为资源数据都是以Unicode存储的，而且不知道为何，若是用ANSI版本，中文只能显示一半。
	/// 相比于模态对话框，非模态对话框使用主程序的消息循环，为了支持键盘导航和其他对话框功能，你必须在消息循环中调用IsDialogMessage来分发对话框的键盘消息
	/// 非模态对话框需要调用ShowWindow，系统不会自动显示
	/// </remarks>
	/// <param name="dialog_id">对话框资源ID</param>
	/// <param name="dialog_procedure">对话框过程，返回值应该是一个布尔值，若为false，对话框管理器执行默认对话框操作以响应消息</param>
	/// <param name="parent_handle">拥有者句柄，可以为nullptr</param>
	/// <param name="init_param">初始化参数，将作为WM_INITDIALOG的lParam参数发送给对话框过程</param>
	/// <param name="module_instance">用于指定哪个模块包含指定模板资源，若在exe中置为NULL即可，若在DLL中，应该传入正确的DLL实例句柄</param>
	/// <returns>若成功，返回值是当前非模态对话框的句柄，否则是NULL</returns>
	inline HWND create_modeless_dialog(int dialog_id, DLGPROC dialog_procedure, HWND parent_handle = nullptr,
		LPARAM init_param = 0, HINSTANCE module_instance = nullptr)
	{
		auto val = CreateDialogParamW(module_instance, MAKEINTRESOURCEW(dialog_id), parent_handle, dialog_procedure, init_param);
		GET_ERROR_MSG_OUTPUT(std::cout);
		return val;
	}

	/// <summary>
	/// 从内存中的对话框模板中创建一个非模态对话框，该函数内部调用CreateWindowEx，并在创建之后发送WM_INITDIALOG消息给对话框过程。使用DestroyWindow来销毁非模态对话框。
	/// </summary>
	/// <remarks>
	/// 关于对话框模板更多细节，请看文档。注意模板内的所有字符串都必须是Unicode字符串
	/// 非模态对话框需要调用ShowWindow，系统不会自动显示
	/// </remarks>
	/// <param name="dialog_template">用于创建对话框的模板。对话框模板由一个描述对话框的标题组成，后跟一个或多个描述对话框中每个控件的附加数据块。</param>
	/// <param name="dialog_procedure">对话框过程，返回值应该是一个布尔值，若为false，对话框管理器执行默认对话框操作以响应消息</param>
	/// <param name="parent_handle">拥有者句柄，可以为nullptr</param>
	/// <param name="init_param">初始化参数，将作为WM_INITDIALOG的lParam参数发送给对话框过程</param>
	/// <param name="module_instance">用于指定哪个模块包含指定模板资源，若在exe中置为NULL即可，若在DLL中，应该传入正确的DLL实例句柄</param>
	/// <returns>若成功，返回值是当前非模态对话框的句柄，否则是NULL</returns>
	inline HWND create_modeless_dialog_indirect(LPCDLGTEMPLATEA dialog_template, DLGPROC dialog_procedure, HWND parent_handle = nullptr,
		LPARAM init_param = 0, HINSTANCE module_instance = nullptr)
	{
		auto val = CreateDialogIndirectParamW(module_instance, dialog_template, parent_handle, dialog_procedure, init_param);
		GET_ERROR_MSG_OUTPUT(std::cout);
		return val;
	}

	/// <summary>
	/// 确定消息是否用于指定的对话框，若是，则处理该消息。该函数可以用于任何包含控件的窗口，使得窗口能够提供与在对话框中使用的相同的键盘选择(如通过按TAB切换按钮)。
	/// </summary>
	/// <remarks>注意，使用该函数处理的消息就不要传给TranslateMessage或DispatchMessage函数了，通过返回值判断是否处理了消息</remarks>
	/// <param name="dialog_handle">对话框的句柄</param>
	/// <param name="msg">要检查的消息MSG指针</param>
	/// <returns>消息是否被处理</returns>
	inline bool is_dialog_message(HWND dialog_handle, MSG& msg)
	{
		auto val = IsDialogMessageA(dialog_handle, &msg);
		GET_ERROR_MSG_OUTPUT(std::cout);
		return val;
	}

	/// <summary>
	/// 将指定对话框模板单位转换为屏幕单位，该函数考虑对话框使用的字体，并正确地将矩形从对话框模板单位转换为像素
	/// </summary>
	/// <param name="dialog_handle">对话框句柄，它必须是对话框创建函数返回的句柄</param>
	/// <param name="rect">[in,out]传入要转换的对话框模板单位矩阵，返回对应的屏幕像素单位矩阵</param>
	/// <returns>操作是否成功</returns>
	inline bool map_dialog_rect(HWND dialog_handle, RECT& rect)
	{
		auto val = MapDialogRect(dialog_handle, &rect);
		GET_ERROR_MSG_OUTPUT(std::cout);
		return val;
	}

};//user
};//mw