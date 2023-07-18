#ifndef MY_EGLCLASS_H
#define MY_EGLCLASS_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
// #include <GLES3/gl3ext.h>
#include "../const.h"

class MyEGLClass
{
public:
    MyEGLClass() = default;
    MyEGLClass(unsigned long display, unsigned long window, const Rect rect);
    ~MyEGLClass();

    void MakeCurrent();
    void ReleasContext();
    GLuint LoadTexture(const string &_textureName);
    void Render();
private:
    GLuint LoadShader(const string &_str, GLenum _type);
    string GetStringFromFile(const string &_fileName);

    EGLDisplay eglDisplay = 0;
    EGLSurface eglSurface;
    EGLConfig eglConfig;
    Rect m_viewRect;
    map<thread::id, EGLContext> m_mapContext; // key: threadId
    thread::id m_curContextThreadId;
    thread::id m_mainContextThreadId;
};
#endif