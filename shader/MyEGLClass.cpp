#include "MyEGLClass.h"
#include <sstream>
#include <fstream>

MyEGLClass::MyEGLClass(unsigned long display, unsigned long window, const Rect rect)
{
    m_mainContextThreadId = std::this_thread::get_id();
    /* EGLDisplay eglDisplay，在Microsoft Windows上是设备上下文的句柄HDC，为了方便将代码移植到不同的操作系统和平台应该使用EGL_DEFAULT_DISPLAY，
     * 返回默认原生显示的连接，如果显示连接不可用，将返回EGL_NO_DISPLAY，表示EGL不可用，因此也无法使用OpenGL ES 3.0 */
    eglDisplay = eglGetDisplay((EGLNativeDisplayType)display);
    if (eglDisplay == EGL_NO_DISPLAY)
    {
        cout << "------ERROR:eglGetDisplay failed------"<< endl;
        return;
    }
    else
    {
        cout << "eglGetDisplay success"<< endl;
    }

    /* 成功打开连接后，需要初始化EGL，这通过如下函数完成，这个函数初始化EGL内部数据结构，返回EGL实现的主版本号和次版本号
     * 如果EGL无法初始化，将返回EGL_FALSE，并将EGL的错误代码设置为:
     * EGL_BAD_DISPLAY--如果display没有指定有效的EGLDisplay，EGL_NOT_INITIALIZED--EGL不能初始化 */
    EGLint majorVersion, minorVersion;
    if (!eglInitialize(eglDisplay, &majorVersion, &minorVersion))
    {
        cout << "------ERROR:eglInitialize failed------"<< endl;
        return;
    }
    else
    {
        cout << "eglInitialize success"<< endl;
    }
    cout << "majorVersion-" + to_string(majorVersion) + " minorVersion" + to_string(minorVersion)<< endl;

    /* 让EGL选择匹配的配置（EGLConfig） */
    EGLint numConfigs;
    EGLint eglConfigAttribList[] = {

        /* EGL_SURFACE_TYPE可填EGL_WINDOW_BIT、EGL_PBUFFER_BIT等，分别表示屏幕上的渲染表面窗口表面和屏幕外渲染表面，
         * 要使用相对应的API eglCreateWindowSurface和eglCreatePbufferSurface，否则会创建失败 */

        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 8,
        EGL_STENCIL_SIZE, 8,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SAMPLE_BUFFERS, 1, // Added for anti-aliased lines
        EGL_NONE               //end
    };

    if (!eglChooseConfig(eglDisplay, eglConfigAttribList, &eglConfig, 1 /*指定配置的大小*/, &numConfigs))
    {
        cout << "------ERROR:eglChooseConfig failed------"<< endl;
        return;
    }
    else
    {
        cout << "eglChooseConfig success"<< endl;
    }

    /*创建窗口表面*/
    eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, (EGLNativeWindowType)window, NULL);
    if (eglSurface == EGL_NO_SURFACE)
    {
        EGLint eglError = eglGetError();
        cout << "------ERROR:eglCreateWindowSurface failed, eglError-0x" + to_string(eglError) + "------"<< endl;
        return;
    }
    else
    {
        cout << "eglCreateWindowSurface success"<< endl;
    }

    cout << "display:"+to_string(display)+", window:"+to_string(window)<< endl;;

    MakeCurrent();
}

MyEGLClass::~MyEGLClass()
{
    cout << "MyEGLClass exit"<< endl;
}

void MyEGLClass::MakeCurrent()
{
    thread::id envThread = std::this_thread::get_id();

    if (envThread == m_curContextThreadId)
    {
        return;
    }

    EGLContext eglContext = nullptr;
    auto iter = m_mapContext.find(envThread);
    if (iter == m_mapContext.end())
    {
        /* 渲染上下文是OpenGL ES 3.0的内部数据结构，包含操作所需的所有状态信息，例如顶点和片段着色器及顶点数据数组的引用 */
        EGLint eglContextAttribList[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2, // 指定OpenGL ES版本相关的上下文类型，1指定OpenGL ES 1.x，3指定OpenGL ES 3.x
            EGL_NONE};
        if (envThread == m_mainContextThreadId)
        {
            eglContext = eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, eglContextAttribList);
        }
        else if (m_mapContext.find(m_mainContextThreadId) != m_mapContext.end())
        {
            EGLContext mainContext = m_mapContext.find(m_mainContextThreadId)->second;
            eglContext = eglCreateContext(eglDisplay, eglConfig, mainContext, eglContextAttribList);
        }
        else
        {
            cout << "------ERROR:eglCreateContext failed, main context can not find------"<< endl;
            return;
        }

        if (eglContext == EGL_NO_CONTEXT)
        {
            cout << "------ERROR:eglCreateContext failed------"<< endl;
            return;
        }
        else
        {
            cout << "eglCreateContext success"<< endl;
        }

        m_mapContext.insert(make_pair(envThread, eglContext));
    }
    else
    {
        eglContext = iter->second;
    }

    if (eglContext == nullptr)
    {
        return;
    }

    /* 因为一个应用程序可能创建多个EGLContext用于不同用途，所有我们需要关联EGLContext和EGLSurface，这一过程通常被称作“指定当前上下文”。
     * draw指定EGL绘图表面，read指定EGL读取表面，这里传入可以传入一样的值*/
    bool isOk{false};
    if (envThread == m_mainContextThreadId)
    {
        isOk = eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
    }
    else
    {
        isOk = eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, eglContext);
    }

    if (!isOk)
    {
        cout << "------ERROR:eglMakeCurrent failed------"<< endl;
        return;
    }
    else
    {
        cout << "eglMakeCurrent success, thread:" + to_string(*(unsigned int *)&envThread)<< endl;
    }

    m_curContextThreadId = envThread;
}

void MyEGLClass::ReleasContext()
{
    thread::id envThread = std::this_thread::get_id();

    if (envThread == m_mainContextThreadId)
    {
        return;
    }
    else
    {
        auto iter = m_mapContext.find(envThread);
        if (iter != m_mapContext.end())
        {
            EGLContext eglContext = iter->second;
            bool isOk = eglDestroyContext(eglDisplay, eglContext);

            if (!isOk)
            {
                cout << "eglDestroyContext failed, thread:" + to_string(*(unsigned int *)&envThread)<< endl;
                return;
            }
            else
            {
                cout << "eglDestroyContext success, thread:" + to_string(*(unsigned int *)&envThread)<< endl;
            }
            m_mapContext.erase(iter);
        }
    }
}

void MyEGLClass::Render()
{
    cout << "MyEGLClass::Render start"<< endl;

    CreateProgram("../../shader/test.vsh", "../../shader/test.fsh");

    // gles渲染，egl更新渲染内容到窗口
    while (1)
    {
        auto startTime = chrono::steady_clock::now();

        eglSwapBuffers(eglDisplay, eglSurface);
        glFinish();

        auto endTime = chrono::steady_clock::now();
        cout << "MyEGLClass::Render use time :" << chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count() << endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

// 创造一个着色器对象，载入着色资源和编译着色器
GLuint MyEGLClass::LoadShader(const string &_str, GLenum _type)
{
    cout << _str << endl;
    GLint glLength = _str.length();
    GLuint shader;
    GLint compiled;
    // 创建着色器对象
    shader = glCreateShader(_type);
    if (shader == 0)
    {
        cout << "glCreateShader failed." << endl;
        return 0;
    }
    // 载入着色器资源
    const char* data = _str.c_str();
    glShaderSource(shader, 1, (const GLchar **)&data, &glLength);
    // 编译着色器
    glCompileShader(shader);
    // 检查编译器状态
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        cout << "Compilation of vertexShader Failed!" << endl;
        glDeleteShader(shader);
        return 0;
    }
    else
    {
        cout << "Compilation of vertexShader Success!" << endl;
    }
    return shader;
}

string MyEGLClass::GetStringFromFile(const string &_fileName)
{
    ifstream file(_fileName);
    ostringstream ost;
    char ch;

    while (ost && file.get(ch))
    {
        ost.put(ch);
    }

    return ost.str();
}

GLuint MyEGLClass::CreateProgram(const string &_vshFileName, const string &_fshFileName)
{
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
    GLint linked;
    // 载入顶点、片段着色器
    
    vertexShader = LoadShader(GetStringFromFile(_vshFileName), GL_VERTEX_SHADER);
    fragmentShader = LoadShader(GetStringFromFile(_fshFileName), GL_FRAGMENT_SHADER);
    // 创建程序对象
    programObject = glCreateProgram();
    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);
    // 结合vPosition到attribute 0
    glBindAttribLocation(programObject, 0, "vPosition");
    // 结合vTexture到attribute 1
    glBindAttribLocation(programObject, 1, "vTexture");
    // 结合vColor到attribute 2
    glBindAttribLocation(programObject, 2, "vColor");
    // 链接程序
    glLinkProgram(programObject);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    // 检查链接状态
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        cout << "programObjectWindow Failed!" << endl;
        glDeleteProgram(programObject);
        return 0;
    }
    else
    {
        cout << "programObjectWindow link Succes!" << endl;
    }
    m_oneMvpLoc = glGetUniformLocation(programObject, "mvpMatrix");
    cout << "oneMvpLoc:" + to_string(m_oneMvpLoc) << endl;

    return programObject;
}
