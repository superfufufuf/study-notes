#include "MyEGLClass.h"
#include "LogManager.h"
#include <sstream>
#include <fstream>
#include "LogManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

MyEGLClass::MyEGLClass(unsigned long display, unsigned long window, const Rect rect)
{
    m_mainContextThreadId = std::this_thread::get_id();
    /* EGLDisplay eglDisplay，在Microsoft Windows上是设备上下文的句柄HDC，为了方便将代码移植到不同的操作系统和平台应该使用EGL_DEFAULT_DISPLAY，
     * 返回默认原生显示的连接，如果显示连接不可用，将返回EGL_NO_DISPLAY，表示EGL不可用，因此也无法使用OpenGL ES 3.0 */
    eglDisplay = eglGetDisplay((EGLNativeDisplayType)display);
    if (eglDisplay == EGL_NO_DISPLAY)
    {
        _LOG("------ERROR:eglGetDisplay failed------", LogLevel::ERROR);
        return;
    }
    else
    {
        _LOG("eglGetDisplay success", LogLevel::DEBUG);
    }

    /* 成功打开连接后，需要初始化EGL，这通过如下函数完成，这个函数初始化EGL内部数据结构，返回EGL实现的主版本号和次版本号
     * 如果EGL无法初始化，将返回EGL_FALSE，并将EGL的错误代码设置为:
     * EGL_BAD_DISPLAY--如果display没有指定有效的EGLDisplay，EGL_NOT_INITIALIZED--EGL不能初始化 */
    EGLint majorVersion, minorVersion;
    if (!eglInitialize(eglDisplay, &majorVersion, &minorVersion))
    {
        _LOG("------ERROR:eglInitialize failed------", LogLevel::ERROR);
        return;
    }
    else
    {
        _LOG("eglInitialize success", LogLevel::DEBUG);
    }
    _LOG("majorVersion-" + to_string(majorVersion) + " minorVersion" + to_string(minorVersion), LogLevel::DEBUG);

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
        EGL_NONE               // end
    };

    if (!eglChooseConfig(eglDisplay, eglConfigAttribList, &eglConfig, 1 /*指定配置的大小*/, &numConfigs))
    {
        _LOG("------ERROR:eglChooseConfig failed------", LogLevel::ERROR);
        return;
    }
    else
    {
        _LOG("eglChooseConfig success", LogLevel::DEBUG);
    }

    /*创建窗口表面*/
    eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, (EGLNativeWindowType)window, NULL);
    if (eglSurface == EGL_NO_SURFACE)
    {
        EGLint eglError = eglGetError();
        _LOG("------ERROR:eglCreateWindowSurface failed, eglError-0x" + to_string(eglError) + "------", LogLevel::ERROR);
        return;
    }
    else
    {
        _LOG("eglCreateWindowSurface success", LogLevel::DEBUG);
    }

    _LOG("display:" + to_string(display) + ", window:" + to_string(window), LogLevel::DEBUG);
    ;

    MakeCurrent();
}

MyEGLClass::~MyEGLClass()
{
    _LOG("MyEGLClass exit", LogLevel::DEBUG);
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
            _LOG("------ERROR:eglCreateContext failed, main context can not find------", LogLevel::ERROR);
            return;
        }

        if (eglContext == EGL_NO_CONTEXT)
        {
            _LOG("------ERROR:eglCreateContext failed------", LogLevel::ERROR);
            return;
        }
        else
        {
            _LOG("eglCreateContext success", LogLevel::DEBUG);
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
        _LOG("------ERROR:eglMakeCurrent failed------", LogLevel::ERROR);
        return;
    }
    else
    {
        _LOG("eglMakeCurrent success, thread:" + to_string(*(unsigned int *)&envThread), LogLevel::DEBUG);
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
                _LOG("eglDestroyContext failed, thread:" + to_string(*(unsigned int *)&envThread), LogLevel::ERROR);
                return;
            }
            else
            {
                _LOG("eglDestroyContext success, thread:" + to_string(*(unsigned int *)&envThread), LogLevel::DEBUG);
            }
            m_mapContext.erase(iter);
        }
    }
}

GLuint MyEGLClass::LoadTexture(const string &_textureName)
{
    ifstream fileChecker(_textureName);
    if (!fileChecker.good())
    {
        _LOG("texture[" + _textureName + "] not find.", LogLevel::ERROR);
        return 0;
    }
    // 纹理对象的句柄
    unsigned int textureId = 0;
    int iw = 0;
    int ih = 0;
    int iwOrg = 0;
    int ihOrg = 0;
    int n = 0;
    // 加载图片获取宽、高、颜色通道信息
    unsigned char *idataLoad = stbi_load(_textureName.c_str(), &iwOrg, &ihOrg, &n, 0);
    if (NULL == idataLoad)
    {
        return 0;
    }
    else
    {
    }
    unsigned char *resizeIdataLoad;
    iw = iwOrg;
    ih = ihOrg;
    resizeIdataLoad = idataLoad;
    unsigned char *idataLoadTemp = resizeIdataLoad;
    unsigned char *idataTo = (unsigned char *)malloc(iw * ih * 4 * sizeof(unsigned char));
    unsigned char *idataToTemp = idataTo;
    // 1 ： 灰度图
    if (1 == n)
    {
        for (int a = 0; a < iw * ih;)
        {
            *idataToTemp = *idataLoadTemp;
            idataToTemp++;
            *idataToTemp = *idataLoadTemp;
            idataToTemp++;
            *idataToTemp = *idataLoadTemp;
            idataToTemp++;
            idataLoadTemp++;
            *idataToTemp = 128;
            idataToTemp++;
            a++;
        }
    }
    // 2 ： 灰度图加透明度
    else if (2 == n)
    {
        for (int a = 0; a < iw * ih * 2;)
        {
            *idataToTemp = *idataLoadTemp;
            idataToTemp++;
            *idataToTemp = *idataLoadTemp;
            idataToTemp++;
            *idataToTemp = *idataLoadTemp;
            idataToTemp++;
            idataLoadTemp++;
            *idataToTemp = *idataLoadTemp;
            idataToTemp++;
            idataLoadTemp++;
            a += 2;
        }
    }
    // 3 ： 红绿蓝 RGB 三色图
    else if (3 == n)
    {
        for (int a = 0; a < iw * ih * 3;)
        {
            *idataToTemp = *idataLoadTemp;
            idataToTemp++;
            idataLoadTemp++;
            *idataToTemp = *idataLoadTemp;
            idataToTemp++;
            idataLoadTemp++;
            *idataToTemp = *idataLoadTemp;
            idataToTemp++;
            idataLoadTemp++;

            *idataToTemp = 255;
            idataToTemp++;
            a += 3;
        }
    }
    // 4 ： 红绿蓝加透明度 RGBA 图
    else if (4 == n)
    {
        memcpy(idataTo, resizeIdataLoad, iw * ih * 4 * sizeof(unsigned char));
    }
    // 创建一个纹理对象
    glGenTextures(1, &textureId);
    // 绑定纹理对象
    glBindTexture(GL_TEXTURE_2D, textureId);
    // 加载纹理
    glTexImage2D(
        GL_TEXTURE_2D,    // target
        0,                // level
        GL_RGBA,          // internalformat
        iw,               // width
        ih,               // height
        0,                // border
        GL_RGBA,          // format
        GL_UNSIGNED_BYTE, // type
        idataTo);         // pixels
    free(idataTo);
    stbi_image_free(resizeIdataLoad);
    // 设置纹理过滤模式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 设置纹理坐标包装
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // 解除绑定
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureId;
}

void MyEGLClass::Render()
{
    _LOG("MyEGLClass::Render start", LogLevel::DEBUG);

    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
    GLint linked;
    // 载入顶点、片段着色器

    vertexShader = LoadShader(GetStringFromFile("shader/test.vsh"), GL_VERTEX_SHADER);
    fragmentShader = LoadShader(GetStringFromFile("shader/test.fsh"), GL_FRAGMENT_SHADER);
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
        _LOG("programObjectWindow Failed!", LogLevel::ERROR);
        glDeleteProgram(programObject);
        return;
    }
    else
    {
        _LOG("programObjectWindow link Succes!", LogLevel::DEBUG);
    }

    GLuint oneMvpLoc = glGetUniformLocation(programObject, "mvpMatrix");
    _LOG("oneMvpLoc:" + to_string(oneMvpLoc), LogLevel::DEBUG);

    GLuint textureId = LoadTexture("./image/white-rect.png");

    float mvp[16] = {0};
    GLfloat pos[12] = {-0.5f, 0.5f, 0,
                       0.5f, 0.5f, 0,
                       0.5f, -0.5f, 0,
                       -0.5f, -0.5f, 0};
    // 左上，左下，右上，右下
    float vTexture[] = {0.0f, 0.0f,  // 右上角,按屏幕坐标系处理即可
                        0.0f, 1.0f,  // 右下角
                        1.0f, 0.0f,  // 左下角
                        1.0f, 1.0f}; // 左上角
    float rectColorTemp[] = {0.0f, 0.0f, 0.0f, 1.0f,
                             0.0f, 0.0f, 0.0f, 1.0f,
                             0.0f, 0.0f, 0.0f, 1.0f,
                             0.0f, 0.0f, 0.0f, 1.0f};

    // gles渲染，egl更新渲染内容到窗口
    while (true)
    {
        auto startTime = chrono::steady_clock::now();

        // 使用程序对象
        glUseProgram(programObject);
        glUniformMatrix4fv(oneMvpLoc, 1, GL_FALSE, (float *)mvp);
        // // 激活一个纹理采集器
        glActiveTexture(GL_TEXTURE0);
        // 绑定纹理
        glBindTexture(GL_TEXTURE_2D, textureId);
        // 载入顶点数据
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, pos);
        glEnableVertexAttribArray(0);
        // 载入纹理坐标数据
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, vTexture);
        glEnableVertexAttribArray(1);
        // 载入颜色数据
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, rectColorTemp);
        glEnableVertexAttribArray(2);
        // 根据缓存步数绘制
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        // 解绑纹理
        glBindTexture(GL_TEXTURE_2D, GL_NONE);

        eglSwapBuffers(eglDisplay, eglSurface);
        glFinish();

        auto endTime = chrono::steady_clock::now();
        _LOG("MyEGLClass::Render use time :" + to_string(chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count()), LogLevel::DEBUG);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

// 创造一个着色器对象，载入着色资源和编译着色器
GLuint MyEGLClass::LoadShader(const string &_str, GLenum _type)
{
    GLint glLength = _str.length();
    GLuint shader;
    GLint compiled;
    // 创建着色器对象
    shader = glCreateShader(_type);
    if (shader == 0)
    {
        _LOG("glCreateShader failed.", LogLevel::ERROR);
        return 0;
    }
    // 载入着色器资源
    const char *data = _str.c_str();
    glShaderSource(shader, 1, (const GLchar **)&data, &glLength);
    // 编译着色器
    glCompileShader(shader);
    // 检查编译器状态
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        _LOG("Compilation of vertexShader Failed!", LogLevel::ERROR);
        glDeleteShader(shader);
        return 0;
    }
    else
    {
        _LOG("Compilation of vertexShader Success!", LogLevel::DEBUG);
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
