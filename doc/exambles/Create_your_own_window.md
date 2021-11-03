# 创建你自己的窗口

本例教你如何继承`window_class`并覆盖`on_paint`函数，并创建自己的窗口。

代码如下:

```cpp
#include "my_windows.h"

class my_window : public mw::window_class
{
protected:
	using mw::window_class::window_class;
	void on_paint(HWND hwnd) override
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		MoveToEx(hdc, 30, 10, NULL);
		LineTo(hdc, 20, 50);
		LineTo(hdc, 50, 20);
		LineTo(hdc, 10, 20);
		LineTo(hdc, 40, 50);
		LineTo(hdc, 30, 10);

		EndPaint(hwnd, &ps);
	}
};

int main(int argc, char *argv[])
{
    auto wnd_class = mw::make_window_class<my_window>(L"my_window_class");

    auto wnd_ins = wnd_class->create();
    wnd_ins->show_window();

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
```

下面依次讲解这些代码的作用：


我们需要引用`my_windows.h`头文件来使用该封装库的内容。

```cpp
#include "my_windows.h"
```


我们使用如下代码来继承`window_class`的构造函数，这样就不需要自己去写构造函数了。

```cpp
using mw::window_class::window_class;
```

这里我覆盖了基类`window_class`的虚函数，该函数在窗口过程接收到`WM_PAINT`消息时调用。这里你可以写你的绘制代码。

```cpp
void on_paint(HWND hwnd) override
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    MoveToEx(hdc, 30, 10, NULL);
    LineTo(hdc, 20, 50);
    LineTo(hdc, 50, 20);
    LineTo(hdc, 10, 20);
    LineTo(hdc, 40, 50);
    LineTo(hdc, 30, 10);

    EndPaint(hwnd, &ps);
}
```

写了一个派生类之后，我们需要创建它，强烈推荐使用`make_window_class`来创建一个派生类：

```cpp
auto wnd_class = mw::make_window_class<my_window>(L"my_window_class");
```

原因是当我们的窗口类被销毁时，它的析构函数会将它所生成的窗口实例全部杀死，所以最好将其创建在堆内存中，又因为在堆内存中很容易忘记`delete`它，所以我使用了智能指针来包装这个类。至于为什么析构函数要将窗口实例杀死，是因为窗口过程所需要调用的事件函数都放在窗口类中，一旦窗口类被销毁，窗口过程就再也找不到这些虚函数了。出于以上的考虑，调用类似于标准库的`make_window_class`函数就是最合适不过了，将窗口类创建在堆中，并用智能指针包装，这样能让用户掌管其生存期又不至于忘记删除它。

然后我们需要调用`create`成员函数来生成一个窗口实例，一个窗口实例唯一管理一个窗口句柄，所以出于同样的原因，当窗口实例被销毁时，对应的真实窗口也会被销毁。我们只能通过`create`成员函数来获得窗口实例，因为窗口实例必须与窗口类进行关联，才能找到对应的虚函数。

之后我们还需要调用窗口实例的`show_window`成员函数来显示窗口，代码如下：

```cpp
auto wnd_ins = wnd_class->create();
wnd_ins->show_window();
```

最后我们要使用如下代码来获得当前线程的所有信息并分配它们，当所有窗口实例被销毁时，窗口过程给线程发送关闭消息，`GetMessage`就会返回0，使得我们退出循环：

```cpp
MSG msg;
while (GetMessage(&msg, NULL, 0, 0))
{
    TranslateMessage(&msg);
    DispatchMessage(&msg);
}
```