#include "DisplayServer.h"

using namespace std;

static DisplayContext bst_gbm_dc;

struct SendFdSt
{
    string cardXPath = "";
    int fd = -1;
};

#define BST_SENDRECV_FD_SOCK_PATH "/tmp/bst_socket_fd_for_dev_dri_"
static map<string, int> mapCardXFd; // card0,card1...

static void getFiles(std::string path, std::vector<std::string> &files)
{
    DIR *dir;
    struct dirent *ptr;
    if ((dir = opendir(path.c_str())) == NULL)
    {
        printf("Open dir %s error.\n", path.c_str());
        return;
    }
    else
    {
        printf("Open dir %s success.\n", path.c_str());
    }

    while ((ptr = readdir(dir)) != NULL)
    {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) /// current dir OR parrent dir
        {
            // do nothing
        }
        else
        {
            std::string strFile;
            strFile += ptr->d_name;
            if (NULL != strstr(strFile.c_str(), "card") /*|| NULL != strstr(strFile.c_str(), "render")*/)
            {
                files.push_back(strFile);
            }
        }
    }
    closedir(dir);
}
static void IniMapCardXFd()
{
    vector<std::string> vFiles;
    getFiles("/dev/dri/", vFiles);
    vector<std::string>::iterator vItor;
    for (vItor = vFiles.begin(); vFiles.end() != vItor; vItor++)
    {
        if (mapCardXFd.end() == mapCardXFd.find(*vItor))
        {
            mapCardXFd[*vItor] = -1;
        }
    }
}
static int BstSendfd(const char *socket_name, int fd_to_send)
{
    int sock_fd;
    struct sockaddr_un sock_addr;
    struct msghdr msg;
    struct iovec iov[1];
    char ctrl_buf[CMSG_SPACE(sizeof(int))];
    struct cmsghdr *cmsg = NULL;
    void *data;
    int res;

    sock_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        printf("sock_fd < 0.\n");
        return -1;
    }
    // printf("sock_fd is : %d\n", sock_fd);

    memset(&sock_addr, 0, sizeof(struct sockaddr_un));
    sock_addr.sun_family = AF_UNIX;
    strncpy(sock_addr.sun_path,
            socket_name,
            sizeof(sock_addr.sun_path) - 1);

    while (connect(sock_fd,
                   (const struct sockaddr *)&sock_addr,
                   sizeof(struct sockaddr_un)))
    {
        usleep(100 * 1000);
        // printf("waiting fd receiver.\n");
    }

    memset(&msg, 0, sizeof(msg));

    iov[0].iov_len = 1;            // must send at least 1 byte
    iov[0].iov_base = (void *)"x"; // any byte value (value ignored)
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    memset(ctrl_buf, 0, sizeof(ctrl_buf));
    msg.msg_control = ctrl_buf;
    msg.msg_controllen = sizeof(ctrl_buf);

    cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));
    data = CMSG_DATA(cmsg);
    *(int *)data = fd_to_send;

    msg.msg_controllen = cmsg->cmsg_len;

    res = sendmsg(sock_fd, &msg, 0);

    if (res <= 0)
    {
        printf("send_fd: sendmsg error.\n");
        return -1;
    }

    // printf("send_fd is %d.\n",fd_to_send);

    close(sock_fd);

    return 0;
}
static int BstReceivefdSingle(const char *socket_name)
{
    int listen_fd;
    struct sockaddr_un sock_addr;
    int connect_fd;
    struct sockaddr_un connect_addr;
    socklen_t connect_addr_len = 0;
    struct msghdr msg;
    struct iovec iov[1];
    char msg_buf[1];
    char ctrl_buf[CMSG_SPACE(sizeof(int))];
    struct cmsghdr *cmsg;
    void *data;
    int recvfd;

    listen_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    if (listen_fd < 0)
    {
        printf("listen_fd < 0.\n");
        return -1;
    }

    unlink(socket_name);

    memset(&sock_addr, 0, sizeof(struct sockaddr_un));
    sock_addr.sun_family = AF_UNIX;
    strncpy(sock_addr.sun_path,
            socket_name,
            sizeof(sock_addr.sun_path) - 1);

    if (bind(listen_fd,
             (const struct sockaddr *)&sock_addr,
             sizeof(struct sockaddr_un)))
    {
        printf("bind error.\n");
        return -1;
    }

    if (listen(listen_fd, 1))
    {
        printf("listen error.\n");
        return -1;
    }

    connect_fd = accept(
        listen_fd,
        (struct sockaddr *)&connect_addr,
        &connect_addr_len);
    close(listen_fd);
    unlink(socket_name);
    if (connect_fd < 0)
    {
        printf("connect_fd < 0.\n");
        return -1;
    }

    memset(&msg, 0, sizeof(msg));

    iov[0].iov_base = msg_buf;
    iov[0].iov_len = sizeof(msg_buf);
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    msg.msg_control = ctrl_buf;
    msg.msg_controllen = sizeof(ctrl_buf);

    if (recvmsg(connect_fd, &msg, 0) <= 0)
    {
        printf("receive_fd: recvmsg err\n");
        close(connect_fd);
        return -1;
    }

    cmsg = CMSG_FIRSTHDR(&msg);
    if (!cmsg)
    {
        printf("receive_fd: NULL message header\n");
        close(connect_fd);
        return -1;
    }
    if (cmsg->cmsg_level != SOL_SOCKET)
    {
        printf("receive_fd: Message level is not SOL_SOCKET\n");
        close(connect_fd);
        return -1;
    }
    if (cmsg->cmsg_type != SCM_RIGHTS)
    {
        printf("receive_fd: Message type is not SCM_RIGHTS\n");
        close(connect_fd);
        return -1;
    }

    data = CMSG_DATA(cmsg);
    recvfd = *(int *)data;

    // printf("receive_fd: %d\n.",recvfd);

    close(connect_fd);

    return recvfd;
}
static void SendFdThread(SendFdSt SendFdStOne)
{
    while (true)
    {
        BstSendfd(SendFdStOne.cardXPath.c_str(), SendFdStOne.fd);
        std::cout << "SendFdThread " + SendFdStOne.cardXPath + " " << SendFdStOne.fd << endl;
    }
    return;
}
static void SendFd()
{
    // 确保要发送的设备节点被打开
    map<string, int>::iterator mapItor;
    for (mapItor = mapCardXFd.begin(); mapCardXFd.end() != mapItor; mapItor++)
    {
        if (mapItor->second < 0)
        {
            string file_path = "/dev/dri/" + mapItor->first;
            mapItor->second = open(file_path.c_str(), O_RDWR | O_CLOEXEC | O_NONBLOCK);
            if (mapItor->second < 0)
            {
                std::cout << "Couldn't open path " << file_path << endl;
            }
            else
            {
                std::cout << "Open path " << file_path << endl;
            }
        }
    }
    // 为每个设备节点开启一个线程发送文件描述符
    for (mapItor = mapCardXFd.begin(); mapCardXFd.end() != mapItor; mapItor++)
    {
        SendFdSt SendFdStOne;
        SendFdStOne.cardXPath = BST_SENDRECV_FD_SOCK_PATH + mapItor->first;
        SendFdStOne.fd = mapItor->second;
        thread StartThread(SendFdThread, SendFdStOne);
        StartThread.detach();
    }
    return;
}
static void ReceiveFdThread()
{
    while (true)
    {
        map<string, int>::iterator mapItor;
        bool needReceFd = false;
        // 判断设备节点是否已经被打开
        for (mapItor = mapCardXFd.begin(); mapCardXFd.end() != mapItor; mapItor++)
        {
            if (mapItor->second < 0)
            {
                needReceFd = true;
            }
        }
        // 设备节点已经被打开
        if (false == needReceFd)
        {
            usleep(100 * 1000);
            continue;
        }
        // 通过domain socket接收设备描述符
        for (mapItor = mapCardXFd.begin(); mapCardXFd.end() != mapItor; mapItor++)
        {
            string socket_path = BST_SENDRECV_FD_SOCK_PATH + mapItor->first;
            int fd = BstReceivefdSingle(socket_path.c_str());
            if (mapItor->second < 0)
            {
                std::cout << "ReceiveFdThread " << socket_path << " " << fd << endl;
                mapItor->second = fd;
            }
        }
    }
    return;
}
static void ReceiveFd()
{
    thread StartThread(ReceiveFdThread);
    StartThread.detach();
    usleep(200 * 1000);
    return;
}

static void drm_fb_destroy_callback(struct gbm_bo *bo, void *data)
{
    uint32_t fb_id = (uint32_t)(unsigned long)data;
    // drmModeRmFB(bst_gbm_dc.fd, fb_id);
    std::cerr << __func__ << " destroy fb " << fb_id << std::endl;
}
static uint32_t bo_to_fb(gbm_bo *bo)
{
    uint32_t handle = gbm_bo_get_handle(bo).u32;
    uint32_t stride = gbm_bo_get_stride(bo);
    int width = gbm_bo_get_width(bo);
    int height = gbm_bo_get_height(bo);
    uint32_t fb_id = 0;
    int ret = drmModeAddFB(bst_gbm_dc.fd, width, height, 24, 32, stride, handle, &fb_id);

    gbm_bo_set_user_data(bo, (void *)(unsigned long)fb_id, drm_fb_destroy_callback);
    if (ret)
    {
        printf("Could not add framebuffer(%d)!", errno);
        exit(0);
    }
    return fb_id;
}
#ifdef EGL_PLATFORM_GBM_KHR
EGLDisplay eglGetPlatformDisplayEXT(EGLenum platform, void *native_display, const EGLint *attrib_list) __attribute__((weak)); // May not be in libEGL symbol table, resolve manually :(
EGLDisplay 
eglGetPlatformDisplayEXT(EGLenum platform, void *native_display, const EGLint *attrib_list){
    PFNEGLGETPLATFORMDISPLAYEXTPROC GetPlatformDisplayEXT = 0;
    if(!GetPlatformDisplayEXT)
        GetPlatformDisplayEXT = (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");
    return GetPlatformDisplayEXT(platform, native_display, attrib_list);
}
EGLSurface eglCreatePlatformWindowSurfaceEXT(EGLDisplay display, EGLConfig config, void *platform_window,
                                             const EGLint *attrib_list) __attribute__((weak));
EGLSurface eglCreatePlatformWindowSurfaceEXT(EGLDisplay display, EGLConfig config, void *platform_window,
                                             const EGLint *attrib_list){
    PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC CreatePlatformWindowSurfaceEXT = 0;
    if(!CreatePlatformWindowSurfaceEXT)
        CreatePlatformWindowSurfaceEXT = (PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC)eglGetProcAddress("eglCreatePlatformWindowSurfaceEXT");
    return CreatePlatformWindowSurfaceEXT(display, config, platform_window, attrib_list);
}
#endif
static void GbmOffScreen(BstWindowRect &bstRectDisplay, DisplayContext &dcTemp)
{
    map<string, int>::iterator mapItor;
    for (mapItor = mapCardXFd.begin(); mapCardXFd.end() != mapItor; mapItor++)
    {
        if (mapItor->second >= 0)
        {
            // 创建 GBM 设备
            dcTemp.display = gbm_create_device(mapItor->second);
            if (NULL == dcTemp.display)
            {
                std::cout << "gbm_create_device fail." << endl;
                exit(0);
            }
            else
            {
                std::cout << "gbm_create_device success." << endl;
            }
            // 创建 GBM surface并将其赋值给变量 gs。
            if (true == bstRectDisplay.transparency)
            {
                dcTemp.gbmSurface = gbm_surface_create(
                    (gbm_device *)dcTemp.display, 1, 1, GBM_FORMAT_ARGB8888,
                    GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
            }
            else
            {
                dcTemp.gbmSurface = gbm_surface_create(
                    (gbm_device *)dcTemp.display, 1, 1, GBM_FORMAT_XRGB8888,
                    GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
            }
            if (NULL == dcTemp.gbmSurface)
            {
                std::cout << "gbm_surface_create fail." << endl;
                exit(0);
            }
            else
            {
                std::cout << "gbm_surface_create success." << endl;
            }
            break;
        }
    }
    return;
}

DisplayServer::DisplayServer(BstWindowRect &bstRectDisplay, DisplayContext &dcTemp, int screenIndexTemp)
{
    drmModeRes *resources;       // resource array
    drmModeConnector *connector; // connector array
    drmModeEncoder *encoder;     // encoder array

    IniMapCardXFd();
    ReceiveFd();
    SendFd();

    // 离屏渲染
    if (true == bstRectDisplay.offScreen)
    {
        GbmOffScreen(bstRectDisplay,dcTemp);
        return;
    }

    // 物理显示器编号
    int screenIndex = 0;
    int i;

    map<string, int>::iterator mapItor;
    for (mapItor = mapCardXFd.begin(); mapCardXFd.end() != mapItor; mapItor++)
    {
        if (mapItor->second >= 0)
        {
            bst_gbm_dc.fd = mapItor->second;
            resources = drmModeGetResources(bst_gbm_dc.fd);
            if (resources == 0)
            {
                printf("drmModeGetResources failed.\n");
                exit(0);
            }
            else
            {
                printf("drmModeGetResources success.\n");
            }
            std::cout << "count_connectors :" << resources->count_connectors << endl;
            for (i = 0; i < resources->count_connectors; ++i)
            {
                connector = drmModeGetConnector(bst_gbm_dc.fd, resources->connectors[i]);
                if (connector == 0)
                {
                    continue;
                }
                if (connector->connection == DRM_MODE_CONNECTED && connector->count_modes > 0)
                {
                    bst_gbm_dc.conn = connector->connector_id;
                    std::cout << "i:" << i << endl;
                    std::cerr << "find connected connector id " << bst_gbm_dc.conn << std::endl;
                    if (screenIndex == screenIndexTemp)
                    {
                        break;
                    }
                    screenIndex++;
                }
                drmModeFreeConnector(connector);
            }

            if (screenIndex == screenIndexTemp)
            {
                break;
            }
            drmModeFreeResources(resources);
        }
    }

    if (i == resources->count_connectors)
    {
        printf("No active connector found!\n");
        exit(0);
    }
    else
    {
        printf("Active connector found!\n");
    }

    encoder = NULL;
    if (connector->encoder_id)
    {
        encoder = drmModeGetEncoder(bst_gbm_dc.fd, connector->encoder_id);
        if (encoder)
        {
            bst_gbm_dc.crtc = encoder->crtc_id;
            drmModeFreeEncoder(encoder);
        }
    }

    if (!encoder)
    {
        std::cout << "count_encoders :" << resources->count_encoders << endl;
        for (i = 0; i < resources->count_encoders; ++i)
        {
            encoder = drmModeGetEncoder(bst_gbm_dc.fd, resources->encoders[i]);
            if (encoder == 0)
            {
                continue;
            }
            for (int j = 0; j < resources->count_crtcs; ++j)
            {
                if (encoder->possible_crtcs & (1 << j))
                {
                    bst_gbm_dc.crtc = resources->crtcs[j];
                    break;
                }
            }
            drmModeFreeEncoder(encoder);
            if (bst_gbm_dc.crtc)
                break;
        }

        if (i == resources->count_encoders)
        {
            printf("No active encoder found!\n");
            exit(0);
        }
        else
        {
            printf("Active encoder found!\n");
        }
    }

    bst_gbm_dc.mode = connector->modes[0];
    if (i == connector->count_modes)
    {
        printf("Requested mode not found!\n");
        exit(0);
    }
    else
    {
        printf("Requested mode found!\n");
    }
    struct gbm_device *gbm = gbm_create_device(bst_gbm_dc.fd);

    EGLint major;
    EGLint minor;
    const char *ver, *extensions;
#ifdef EGL_PLATFORM_GBM_KHR
    bst_gbm_dc.display = eglGetPlatformDisplayEXT(EGL_PLATFORM_GBM_KHR, (EGLNativeDisplayType)gbm, NULL);
    std::cout<<"EGL_PLATFORM_GBM_KHR is defined."<<endl;
#else
    bst_gbm_dc.display = eglGetDisplay((EGLNativeDisplayType)gbm);
    std::cout<<"EGL_PLATFORM_GBM_KHR is not defined."<<endl;
#endif
    eglInitialize(bst_gbm_dc.display, &major, &minor);
    ver = eglQueryString(bst_gbm_dc.display, EGL_VERSION);
    extensions = eglQueryString(bst_gbm_dc.display, EGL_EXTENSIONS);
    fprintf(stderr, "ver: %s, ext: %s\n", ver, extensions);

    if (!eglBindAPI(EGL_OPENGL_ES_API))
    {
        std::cerr << "bind api failed." << std::endl;
        exit(-1);
    }
    else
    {
        std::cerr << "bind api success." << std::endl;
    }

    EGLContext ctx;
    uint32_t gbm_format;
    EGLint egl_native_id;

    /* 让EGL选择匹配的配置（EGLConfig） */
    EGLConfig eglConfig;
    EGLint numConfigs;
    /* EGL_SURFACE_TYPE可填EGL_WINDOW_BIT、EGL_PBUFFER_BIT等，分别表示屏幕上的渲染表面窗口表面和屏幕外渲染表面，
     * 要使用相对应的API eglCreateWindowSurface和eglCreatePbufferSurface，否则会创建失败 */
    EGLint eglConfigAttribListFirst[] = {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 8,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SAMPLE_BUFFERS, 1, // Added for anti-aliased lines
        EGL_NONE};
    if (!eglChooseConfig(bst_gbm_dc.display, eglConfigAttribListFirst, &eglConfig, 1 /*指定配置的大小*/, &numConfigs))
    {
        std::cout<<"error:eglChooseConfig EGL_SAMPLE_BUFFERS:1 failed"<<endl;
    }
    else
    {
        std::cout<<"eglChooseConfig EGL_SAMPLE_BUFFERS:1 success"<<endl;
    }
    EGLint eglConfigAttribListSecond[] = {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 8,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SAMPLE_BUFFERS, 0, // Added for anti-aliased lines
        EGL_NONE};
    if (0 == numConfigs)
    {
        if (!eglChooseConfig(bst_gbm_dc.display, eglConfigAttribListSecond, &eglConfig, 1 /*指定配置的大小*/, &numConfigs))
        {
            std::cout<<"error:eglChooseConfig EGL_SAMPLE_BUFFERS:0 failed"<<endl;
            exit(-1);
        }
        else
        {
            std::cout<<"eglChooseConfig EGL_SAMPLE_BUFFERS:0 success"<<endl;
        }
    }

    EGLBoolean ret = eglGetConfigAttrib(bst_gbm_dc.display, eglConfig, EGL_NATIVE_VISUAL_ID, &egl_native_id);
    if (EGL_TRUE != ret)
    {
        printf("eglGetConfigAttrib EGL_NATIVE_VISUAL_ID failed.\n");
        exit(-1);
    }
    else
    {
        printf("eglGetConfigAttrib EGL_NATIVE_VISUAL_ID success.\n");
    }
    gbm_format = egl_native_id;

    bstRectDisplay.width = bst_gbm_dc.mode.hdisplay;
    bstRectDisplay.height = bst_gbm_dc.mode.vdisplay;
    if (true == bstRectDisplay.transparency)
    {
        bst_gbm_dc.gbmSurface = gbm_surface_create(gbm, bst_gbm_dc.mode.hdisplay,
                                                   bst_gbm_dc.mode.vdisplay, GBM_FORMAT_ARGB8888,
                                                   GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
    }
    else
    {
        bst_gbm_dc.gbmSurface = gbm_surface_create(gbm, bst_gbm_dc.mode.hdisplay,
                                                   bst_gbm_dc.mode.vdisplay, GBM_FORMAT_XRGB8888,
                                                   GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
    }
    if (!bst_gbm_dc.gbmSurface)
    {
        printf("cannot create gbm surface (%d): %m.\n", errno);
        exit(-EFAULT);
    }
    else
    {
        printf("create gbm surface (%d): %m.\n", errno);
    }
    /* 渲染上下文是OpenGL ES 3.0的内部数据结构，包含操作所需的所有状态信息，例如顶点和片段着色器及顶点数据数组的引用 */
    EGLint eglContextAttribList[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2, // 指定OpenGL ES版本相关的上下文类型，1指定OpenGL ES 1.x，3指定OpenGL ES 3.x
        EGL_NONE};
    ctx = eglCreateContext(bst_gbm_dc.display, eglConfig, EGL_NO_CONTEXT, eglContextAttribList);
    if (ctx == EGL_NO_CONTEXT)
    {
        printf("no context created.\n");
        exit(0);
    }
    else
    {
        printf("context created.\n");
    }
#ifdef EGL_PLATFORM_GBM_KHR
    bst_gbm_dc.surface = eglCreatePlatformWindowSurfaceEXT(bst_gbm_dc.display, eglConfig,
                                                           (void *)bst_gbm_dc.gbmSurface,
                                                           NULL);
    std::cout<<"EGL_PLATFORM_GBM_KHR is defined."<<endl;
#else
    bst_gbm_dc.surface = eglCreateWindowSurface(bst_gbm_dc.display, eglConfig,
                                                (EGLNativeWindowType)bst_gbm_dc.gbmSurface,
                                                NULL);
    std::cout<<"EGL_PLATFORM_GBM_KHR is not defined."<<endl;
#endif

    if (bst_gbm_dc.surface == EGL_NO_SURFACE)
    {
        printf("cannot create EGL window surface.\n");
        exit(-1);
    }
    else
    {
        printf("create EGL window surface.\n");
    }

    if (!eglMakeCurrent(bst_gbm_dc.display, bst_gbm_dc.surface, bst_gbm_dc.surface, ctx))
    {
        printf("cannot activate EGL context.\n");
        exit(-1);
    }
    else
    {
        printf("activate EGL context.\n");
    }

    glClearColor(1.0, 1.0, 1.0, 1.0);
    if (!eglSwapBuffers(bst_gbm_dc.display, bst_gbm_dc.surface))
    {
        printf("cannot swap buffers.\n");
        exit(-1);
    }
    else
    {
        printf("swap buffers.\n");
    }

    bst_gbm_dc.bo = gbm_surface_lock_front_buffer(bst_gbm_dc.gbmSurface);
    if (!bst_gbm_dc.bo)
    {
        printf("cannot lock front buffer during creation.\n");
        exit(-1);
    }
    else
    {
        printf("lock front buffer during creation.\n");
    }

    uint32_t fb_id = bo_to_fb(bst_gbm_dc.bo);
    ret = drmModeSetCrtc(bst_gbm_dc.fd, bst_gbm_dc.crtc, fb_id, 0, 0, &bst_gbm_dc.conn, 1, &bst_gbm_dc.mode);
    if (ret)
    {
        printf("Could not set mode.\n");
        exit(0);
    }
    else
    {
        printf("set mode.\n");
    }
    dcTemp = bst_gbm_dc;
}

DisplayServer::~DisplayServer()
{
    std::cout << "DisplayServer exit." << endl;
}
