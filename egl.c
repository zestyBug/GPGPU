#include "glcorearb.h"
#include "EGL/egl.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#if defined(__CYGWIN32__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(WINAPI_FAMILY)
	#include <windows.h>
	#define GLWLibraryName					"opengl32.dll"
	#define GLWHandle						HMODULE
	#define GLWOpen(N)						LoadLibrary(N)
	// wglGetProcAddress cant obtain legacy opengl functions
	#define GLWObtainLegacy(HANDLE,SYMBOL)	GetProcAddress(HANDLE,SYMBOL)
	#define GLWObtain(HANDLE,SYMBOL)		wglGetProcAddress((LPCSTR)SYMBOL)
	#define GLWClose(HANDLE)				FreeLibrary(HANDLE)
	#define GLWGetProcAddress(name)			wglGetProcAddress((LPCSTR)name)
	#define GLWErrorReason()				"win32-UNKNOWN"
	// why decltype?
#ifdef _MSC_VER
	#define PROC_LEGACY(V)					retval|=(V=GLWObtainLegacy(libgl,#V))==NULL
	#define PROC(V)							retval|=(V=GLWObtain(libgl,#V))==NULL
	#define PROC_EXT(V)						if(V==NULL) V=GLWObtain(libgl,#V "EXT");
#else
	#define PROC_LEGACY(V)					retval|=(V=(__typeof__(V))GLWObtainLegacy(libgl,#V))==NULL
	#define PROC(V)							retval|=(V=(__typeof__(V))GLWObtain(libgl,#V))==NULL
	#define PROC_EXT(V)						if(V==NULL) V=(__typeof__(V))GLWObtain(libgl,#V "EXT");
#endif
#elif defined(__ANDROID__) || defined(__linux__)
	#include <dlfcn.h>
	#define EGLWLibraryName					"/usr/lib/libEGL.so.1"
	#define GLWHandle						void*
	#define GLWOpen(N)						dlopen(N, RTLD_NOW | RTLD_LOCAL)
	#define GLWObtain(HANDLE,SYMBOL)		dlsym(HANDLE,SYMBOL)
	#define GLWClose(HANDLE)				dlclose(HANDLE)
	#define GLWGetProcAddress(name)			NULL
	#define GLWErrorReason()				dlerror()
	#define PROC_EGL(V)						retval|=(V=(__typeof__(V))GLWObtain(libegl,#V))==NULL
	#define PROC_EGLX(V)					retval|=(V=(__typeof__(V))eglGetProcAddress(#V))==NULL
//#else#error "undefined compiler"
#else
#error undefined platform
#endif

#define DEBUG(...) printf(__VA_ARGS__)
static GLWHandle libegl=NULL;

int32_t eglwDestroy()
{
	if(libegl)
		return GLWClose(libegl);
	return 0;
}
int32_t eglwInitialize()
{
    int32_t retval=0;
    if(!libegl)
		libegl = GLWOpen(EGLWLibraryName);
    if(!libegl)
	{
		DEBUG("Unable to open %s: %s\n",EGLWLibraryName,GLWErrorReason());
		return 1;
	}


    PROC_EGL(eglChooseConfig);
    PROC_EGL(eglCreateContext);
    PROC_EGL(eglCreatePbufferSurface);
    PROC_EGL(eglDestroyContext);
    PROC_EGL(eglDestroySurface);
    PROC_EGL(eglGetConfigAttrib);
    PROC_EGL(eglGetConfigs);
    PROC_EGL(eglGetCurrentDisplay);
    PROC_EGL(eglGetCurrentSurface);
    PROC_EGL(eglGetDisplay);
    PROC_EGL(eglGetError);
    PROC_EGL(eglGetProcAddress);
    PROC_EGL(eglInitialize);
    PROC_EGL(eglMakeCurrent);
    PROC_EGL(eglQueryContext);
    PROC_EGL(eglQueryString);
    PROC_EGL(eglQuerySurface);
    PROC_EGL(eglSwapBuffers);
    PROC_EGL(eglTerminate);
    PROC_EGL(eglWaitGL);
    PROC_EGL(eglWaitNative);
    if(retval)
	{
		DEBUG("was unable to load some EGL 1.0 functions\n");
		return 1;
	}

    PROC_EGL(eglBindTexImage);
    PROC_EGL(eglReleaseTexImage);
    PROC_EGL(eglSurfaceAttrib);
    PROC_EGL(eglSwapInterval);
    if(retval)
	{
		DEBUG("was unable to load some EGL 1.1 functions\n");
		return 1;
	}

    PROC_EGL(eglBindAPI);
    PROC_EGL(eglQueryAPI);
    PROC_EGL(eglCreatePbufferFromClientBuffer);
    PROC_EGL(eglReleaseThread);
    PROC_EGL(eglWaitClient);
    if(retval)
	{
		DEBUG("was unable to load some EGL 1.2 functions\n");
		return 1;
	}

	PROC_EGLX(eglGetPlatformDisplayEXT);
    if(retval)
	{
		DEBUG("was unable to load some EGL_EXT_platform_base functions\n");
		return 1;
	}

	return 0;
}

PFNEGLCHOOSECONFIGPROC eglChooseConfig;
PFNEGLCREATECONTEXTPROC eglCreateContext;
PFNEGLCREATEPBUFFERSURFACEPROC eglCreatePbufferSurface;
PFNEGLDESTROYCONTEXTPROC eglDestroyContext;
PFNEGLDESTROYSURFACEPROC eglDestroySurface;
PFNEGLGETCONFIGATTRIBPROC eglGetConfigAttrib;
PFNEGLGETCONFIGSPROC eglGetConfigs;
PFNEGLGETCURRENTDISPLAYPROC eglGetCurrentDisplay;
PFNEGLGETCURRENTSURFACEPROC eglGetCurrentSurface;
PFNEGLGETDISPLAYPROC eglGetDisplay;
PFNEGLGETERRORPROC eglGetError;
PFNEGLGETPROCADDRESSPROC eglGetProcAddress;
PFNEGLINITIALIZEPROC eglInitialize;
PFNEGLMAKECURRENTPROC eglMakeCurrent;
PFNEGLQUERYCONTEXTPROC eglQueryContext;
PFNEGLQUERYSTRINGPROC eglQueryString;
PFNEGLQUERYSURFACEPROC eglQuerySurface;
PFNEGLSWAPBUFFERSPROC eglSwapBuffers;
PFNEGLTERMINATEPROC eglTerminate;
PFNEGLWAITGLPROC eglWaitGL;
PFNEGLWAITNATIVEPROC eglWaitNative;

PFNEGLBINDTEXIMAGEPROC eglBindTexImage;
PFNEGLRELEASETEXIMAGEPROC eglReleaseTexImage;
PFNEGLSURFACEATTRIBPROC eglSurfaceAttrib;
PFNEGLSWAPINTERVALPROC eglSwapInterval;

PFNEGLBINDAPIPROC eglBindAPI;
PFNEGLQUERYAPIPROC eglQueryAPI;
PFNEGLCREATEPBUFFERFROMCLIENTBUFFERPROC eglCreatePbufferFromClientBuffer;
PFNEGLRELEASETHREADPROC eglReleaseThread;
PFNEGLWAITCLIENTPROC eglWaitClient;

PFNEGLGETCURRENTCONTEXTPROC eglGetCurrentContext;

PFNEGLCREATESYNCPROC eglCreateSync;
PFNEGLDESTROYSYNCPROC eglDestroySync;
PFNEGLCLIENTWAITSYNCPROC eglClientWaitSync;
PFNEGLGETSYNCATTRIBPROC eglGetSyncAttrib;
PFNEGLCREATEIMAGEPROC eglCreateImage;
PFNEGLDESTROYIMAGEPROC eglDestroyImage;
PFNEGLGETPLATFORMDISPLAYPROC eglGetPlatformDisplay;
PFNEGLCREATEPLATFORMWINDOWSURFACEPROC eglCreatePlatformWindowSurface;
PFNEGLCREATEPLATFORMPIXMAPSURFACEPROC eglCreatePlatformPixmapSurface;
PFNEGLWAITSYNCPROC eglWaitSync;

PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT;
PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC eglCreatePlatformWindowSurfaceEXT;
PFNEGLCREATEPLATFORMPIXMAPSURFACEEXTPROC eglCreatePlatformPixmapSurfaceEXT;