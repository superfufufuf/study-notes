#ifdef _FOR_X11_

#include "MyWindow.h"
#include "MyEGLClass.h"
#include "LogManager.h"
#include <thread>

MyX11Class::MyX11Class(const Rect rect)
{
    quit = false;

    // 与Xserver建立连接
    display = XOpenDisplay(NULL);
    if (display == NULL)
    {
        _LOG("------X11 Cannot open display------", LogLevel::ERROR);
        exit(1);
    }
    else
    {
        _LOG("X11 open display success", LogLevel::DEBUG);
    }

    // 获取默认屏幕
    int screen = DefaultScreen(display);

    // 创建一个窗口
    window = XCreateSimpleWindow(display,
                                 RootWindow(display, screen),
                                 rect.x,
                                 rect.y,
                                 rect.width,
                                 rect.height,
                                 0,
                                 BlackPixel(display, screen),
                                 WhitePixel(display, screen));

    // 选择一种感兴趣的事件进行监听
    XSelectInput(display, window, KeyPressMask | ButtonPressMask);

    // 显示窗口
    XMapWindow(display, window);
}
MyX11Class::~MyX11Class()
{
    // 关闭与Xserver服务器的连接
    XCloseDisplay(display);
    _LOG("MyX11Class exit", LogLevel::DEBUG);
}
Display *MyX11Class::GetDisplay()
{
    return display;
}
Window MyX11Class::GetWindow()
{
    return window;
}
void MyX11Class::Stop()
{
    quit = true;
}
void MyX11Class::X11EventRecv()
{
    _LOG("start get X11 event", LogLevel::DEBUG);
    // 事件遍历
    XEvent event;
    KeySym event_key_0;
    string str;
    char *key_0_string = NULL;
    while (!quit)
    {
        XNextEvent(display, &event);

        // 鼠标事件
        if (event.type == ButtonPress)
        {
            str = "mouse event";
            _LOG(str, LogLevel::DEBUG);
        }
        // 键盘事件
        else if (event.type == KeyPress)
        {
            event_key_0 = XLookupKeysym(&(event.xkey), 0);
            key_0_string = XKeysymToString(event_key_0);
            str = key_0_string;
            for (int a = 0; a < str.length(); a++)
            {
                if (str[a] >= 'a' && str[a] <= 'z')
                {
                    str[a] = str[a] - 'a' + 'A';
                }
            }
            _LOG("key event:" + str, LogLevel::DEBUG);
        }
    }
}
void MyX11Class::Start()
{
    std::thread thread(&MyX11Class::X11EventRecv, this);
    thread.detach();
}

MyWindow::MyWindow()
{
}

MyWindow::~MyWindow()
{
}

void MyWindow::Start()
{
    // 初始化EGL要显示的窗口坐标和大小
    unsigned long eglDisplay = 0;
    unsigned long eglWindow = 0;
    int xOri = 0;
    int yOri = 0;
    unsigned int widthOri = 1080;
    unsigned int heightOri = 720;
    int displayX = 0;
    int displayY = 0;
    unsigned int displayWidth = 1080;
    unsigned int displayHeight = 720;

    // 初始化X11要显示的窗口坐标和大小
    Rect X11Rect;
    X11Rect.x = displayX;
    X11Rect.y = displayY;
    X11Rect.width = displayWidth;
    X11Rect.height = displayHeight;

    // 使用窗口坐标和大小初始化X11
    MyX11Class x11Test(X11Rect);
    // 键盘和鼠标事件的接收和转发
    x11Test.Start();
    eglDisplay = (unsigned long)x11Test.GetDisplay();
    eglWindow = (unsigned long)x11Test.GetWindow();

    if (0 == eglDisplay)
    {
        _LOG("eglDisplay ini failed.", LogLevel::ERROR);
    }
    else
    {
        _LOG("eglDisplay ini success.", LogLevel::DEBUG);
    }
    if (0 == eglWindow)
    {
        _LOG("eglWindow ini failed.", LogLevel::ERROR);
    }
    else
    {
        _LOG("eglWindow ini success.", LogLevel::DEBUG);
    }

    thread EGLRenderThread(std::bind(&MyWindow::StartEgl, this, eglDisplay, eglWindow, X11Rect));

    EGLRenderThread.detach();
    while (true)
    {
        this_thread::sleep_for(chrono::milliseconds(1));
    }
}

void MyWindow::StartEgl(unsigned long display, unsigned long window, const Rect rect)
{
    // 初始化egl,渲染过程
    MyEGLClass myEgl(display, window, rect);
    // opengl渲染,egl刷新窗口显示,根据外外部事件，键盘鼠标事件的处理结果完成渲染过程
    myEgl.Render();
}

#endif