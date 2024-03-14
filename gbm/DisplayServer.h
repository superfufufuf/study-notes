#ifndef DISPLAY_SERVER_H
#define DISPLAY_SERVER_H

#include <thread>
#include <unistd.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <cstring>
#include <dirent.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <gbm.h>
#include <drm/drm.h>
#include <drm/drm_fourcc.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <map>
#include <vector>
#include <iostream>

struct DisplayContext {
    int fd = -1;
    EGLDisplay display;
    drmModeModeInfo mode;
    uint32_t conn; 
    uint32_t crtc; 
    struct gbm_surface *gbmSurface;
    EGLSurface surface;
    struct gbm_bo *bo;
    struct gbm_bo *next_bo;
    uint32_t next_fb_id; 
    bool pflip_pending;
    bool cleanup;
};

struct BstWindowRect
{
    float x = 0;
    float y = 0;
    float width = 0;
    float height = 0;
    bool transparency = false; // 窗口是否透明
    bool offScreen = false;    // 是否离屏渲染
};

class DisplayServer
{
public:
    DisplayServer() = default;
    DisplayServer(BstWindowRect &bstRectDisplay, DisplayContext &dcTemp, int screenIndexTemp);
    ~DisplayServer();

private:

};

#endif