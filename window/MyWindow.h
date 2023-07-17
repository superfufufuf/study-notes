#ifndef MY_WINDOW_H
#define MY_WINDOW_H

#ifdef _FOR_X11_

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include "const.h"

class MyX11Class
{
public:
    MyX11Class() = default;
    MyX11Class(const Rect rect);
    ~MyX11Class();

    void Start();
    void Stop();
    void X11EventRecv();
    Window GetWindow();
    Display *GetDisplay();

private:
    Display *display;
    Window window;
    bool quit = false;
};

class MyWindow
{
public:
    MyWindow();
    ~MyWindow();

    void Start();
private:
    void StartEgl(unsigned long display, unsigned long window, const Rect rect);
};

#endif
#endif