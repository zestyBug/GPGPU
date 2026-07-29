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
	#define GLWLibraryName					"libGL.so.1"
	#define GLWHandle						void*
	#define GLWOpen(N)						dlopen(N, RTLD_NOW | RTLD_LOCAL)
	#define GLWObtain(HANDLE,SYMBOL)		dlsym(HANDLE,SYMBOL)
	#define GLWObtainLegacy(HANDLE,SYMBOL)	dlsym(HANDLE,SYMBOL)
	#define GLWClose(HANDLE)				dlclose(HANDLE)
	#define GLWGetProcAddress(name)			NULL
	#define GLWErrorReason()				dlerror()
	#define PROC_LEGACY(V)					retval|=(V=(__typeof__(V))GLWObtain(libgl,#V))==NULL
	#define PROC(V)							retval|=(V=(__typeof__(V))GLWObtain(libgl,#V))==NULL
	#define PROC_EXT(V)						if(V==NULL) V=(__typeof__(V))GLWObtain(libgl,#V "EXT");
	#define PROC_ARB(V)						if(V==NULL) V=(__typeof__(V))eglGetProcAddress(#V);
//#else#error "undefined compiler"
#else
#error undefined platform
#endif

#define DEBUG(...) printf(__VA_ARGS__)
static GLWHandle libgl=NULL;

int32_t glwDestroy()
{
	if(libgl)
		return GLWClose(libgl);
	return 0;
}
int32_t glwInitialize(int gl_current_version)
{
	int32_t retval=0;
	// makes an executable object file specified by file available to the calling program.
	if(!libgl)
		libgl = GLWOpen(GLWLibraryName);
	if(!libgl)
	{
		DEBUG("Unable to open %s: %s\n",GLWLibraryName,GLWErrorReason());
		return 1;
	}
	// obtain the address of a symbol defined within an object.
	PROC_LEGACY(glGetString);
	if(!glGetString)
	{
		DEBUG("Invalide library : %s, %s\n",GLWLibraryName,GLWErrorReason());
		return 1;
	}


	PROC_LEGACY(glCullFace);
	PROC_LEGACY(glFrontFace);
	PROC_LEGACY(glHint);
	PROC_LEGACY(glLineWidth);
	PROC_LEGACY(glPointSize);
	PROC_LEGACY(glScissor);
	PROC_LEGACY(glTexParameterf);
	PROC_LEGACY(glTexParameterfv);
	PROC_LEGACY(glTexParameteri);
	PROC_LEGACY(glTexParameteriv);
	PROC_LEGACY(glTexImage1D);
	PROC_LEGACY(glTexImage2D);
	PROC_LEGACY(glDrawBuffer);
	PROC_LEGACY(glClear);
	PROC_LEGACY(glClearColor);
	PROC_LEGACY(glClearStencil);
	PROC_LEGACY(glClearDepth);
	PROC_LEGACY(glStencilMask);
	PROC_LEGACY(glColorMask);
	PROC_LEGACY(glDepthMask);
	PROC_LEGACY(glDisable);
	PROC_LEGACY(glEnable);
	PROC_LEGACY(glFinish);
	PROC_LEGACY(glFlush);
	PROC_LEGACY(glBlendFunc);
	PROC_LEGACY(glLogicOp);
	PROC_LEGACY(glStencilFunc);
	PROC_LEGACY(glStencilOp);
	PROC_LEGACY(glDepthFunc);
	PROC_LEGACY(glPixelStorei);
	PROC_LEGACY(glReadBuffer);
	PROC_LEGACY(glReadPixels);
	PROC_LEGACY(glGetBooleanv);
	PROC_LEGACY(glGetDoublev);
	PROC_LEGACY(glGetError);
	PROC_LEGACY(glGetFloatv);
	PROC_LEGACY(glGetIntegerv);
	PROC_LEGACY(glGetTexImage);
	PROC_LEGACY(glGetTexParameterfv);
	PROC_LEGACY(glGetTexParameteriv);
	PROC_LEGACY(glGetTexLevelParameterfv);
	PROC_LEGACY(glGetTexLevelParameteriv);
	PROC_LEGACY(glIsEnabled);
	PROC_LEGACY(glDepthRange);
	PROC_LEGACY(glViewport);
	if(retval)
	{
		DEBUG("was unable to load some GL_VERSION_1_0 functions\n");
		return 1;
	}

	if(gl_current_version < 0x101)
	{
		goto EXT;
	}
	PROC_LEGACY(glBindTexture);
	PROC_LEGACY(glDrawArrays);
	PROC_LEGACY(glDrawElements);
	PROC_LEGACY(glCopyTexImage2D);
	PROC_LEGACY(glCopyTexSubImage2D);
	PROC_LEGACY(glDeleteTextures);
	PROC_LEGACY(glGenTextures);
	PROC_LEGACY(glGetPointerv);
	PROC_LEGACY(glIsTexture);
	PROC_LEGACY(glTexSubImage2D);
	if(retval)
	{
		DEBUG("was unable to load some GL_VERSION_1_1 functions\n");
		return 1;
	}

	if(gl_current_version < 0x102)
	{
		goto EXT;
	}
	PROC(glCopyTexSubImage3D);
	PROC(glDrawRangeElements);
	PROC(glTexImage3D);
	PROC(glTexSubImage3D);
	if(retval)
	{
		DEBUG("was unable to load some GL_VERSION_1_2 functions\n");
		return 1;
	}

	if(gl_current_version < 0x103)
	{
		goto EXT;
	}
	PROC(glActiveTexture);
	PROC(glCompressedTexImage2D);
	PROC(glCompressedTexImage3D);
	PROC(glCompressedTexSubImage2D);
	PROC(glCompressedTexSubImage3D);
	PROC(glGetCompressedTexImage);
	PROC(glSampleCoverage);
	if(retval)
	{
		DEBUG("was unable to load some GL_VERSION_1_3 functions\n");
		return 1;
	}

	if(gl_current_version < 0x104)
	{
		goto EXT;
	}
	PROC(glBlendColor);
	PROC(glBlendEquation);
	PROC(glBlendFuncSeparate);
	PROC(glMultiDrawArrays);
	PROC(glMultiDrawElements);
	PROC(glPointParameterf);
	PROC(glPointParameterfv);
	PROC(glPointParameteri);
	PROC(glPointParameteriv);
	if(retval)
	{
		DEBUG("was unable to load some GL_VERSION_1_4 functions\n");
		return 1;
	}

	if(gl_current_version < 0x105)
	{
		goto EXT;
	}
	PROC(glBeginQuery);
	PROC(glBindBuffer);
	PROC(glBufferData);
	PROC(glBufferSubData);
	PROC(glDeleteBuffers);
	PROC(glDeleteQueries);
	PROC(glEndQuery);
	PROC(glGenBuffers);
	PROC(glGenQueries);
	PROC(glGetBufferParameteriv);
	PROC(glGetBufferPointerv);
	PROC(glGetBufferSubData);
	PROC(glGetQueryObjectiv);
	PROC(glGetQueryObjectuiv);
	PROC(glGetQueryiv);
	PROC(glIsBuffer);
	PROC(glIsQuery);
	PROC(glMapBuffer);
	PROC(glUnmapBuffer);
	if(retval)
	{
		DEBUG("was unable to load some GL_VERSION_1_5 functions\n");
		return 1;
	}

	if(gl_current_version < 0x200)
	{
		goto EXT;
	}
	PROC(glAttachShader);
	PROC(glBindAttribLocation);
	PROC(glBlendEquationSeparate);
	PROC(glCompileShader);
	PROC(glCreateProgram);
	PROC(glCreateShader);
	PROC(glDeleteProgram);
	PROC(glDeleteShader);
	PROC(glDetachShader);
	PROC(glDisableVertexAttribArray);
	PROC(glDrawBuffers);
	PROC(glEnableVertexAttribArray);
	PROC(glGetActiveAttrib);
	PROC(glGetActiveUniform);
	PROC(glGetAttachedShaders);
	PROC(glGetAttribLocation);
	PROC(glGetProgramInfoLog);
	PROC(glGetProgramiv);
	PROC(glGetShaderInfoLog);
	PROC(glGetShaderSource);
	PROC(glGetShaderiv);
	PROC(glGetUniformLocation);
	PROC(glGetUniformfv);
	PROC(glGetUniformiv);
	PROC(glGetVertexAttribPointerv);
	PROC(glGetVertexAttribdv);
	PROC(glGetVertexAttribfv);
	PROC(glGetVertexAttribiv);
	PROC(glIsProgram);
	PROC(glIsShader);
	PROC(glLinkProgram);
	PROC(glShaderSource);
	PROC(glStencilFuncSeparate);
	PROC(glStencilMaskSeparate);
	PROC(glStencilOpSeparate);
	PROC(glUniform1f);
	PROC(glUniform1fv);
	PROC(glUniform1i);
	PROC(glUniform1iv);
	PROC(glUniform2f);
	PROC(glUniform2fv);
	PROC(glUniform2i);
	PROC(glUniform2iv);
	PROC(glUniform3f);
	PROC(glUniform3fv);
	PROC(glUniform3i);
	PROC(glUniform3iv);
	PROC(glUniform4f);
	PROC(glUniform4fv);
	PROC(glUniform4i);
	PROC(glUniform4iv);
	PROC(glUniformMatrix2fv);
	PROC(glUniformMatrix3fv);
	PROC(glUniformMatrix4fv);
	PROC(glUseProgram);
	PROC(glValidateProgram);
	PROC(glVertexAttrib1d);
	PROC(glVertexAttrib1dv);
	PROC(glVertexAttrib1f);
	PROC(glVertexAttrib1fv);
	PROC(glVertexAttrib1s);
	PROC(glVertexAttrib1sv);
	PROC(glVertexAttrib2d);
	PROC(glVertexAttrib2dv);
	PROC(glVertexAttrib2f);
	PROC(glVertexAttrib2fv);
	PROC(glVertexAttrib2s);
	PROC(glVertexAttrib2sv);
	PROC(glVertexAttrib3d);
	PROC(glVertexAttrib3dv);
	PROC(glVertexAttrib3f);
	PROC(glVertexAttrib3fv);
	PROC(glVertexAttrib3s);
	PROC(glVertexAttrib3sv);
	PROC(glVertexAttrib4Nbv);
	PROC(glVertexAttrib4Niv);
	PROC(glVertexAttrib4Nsv);
	PROC(glVertexAttrib4Nub);
	PROC(glVertexAttrib4Nubv);
	PROC(glVertexAttrib4Nuiv);
	PROC(glVertexAttrib4Nusv);
	PROC(glVertexAttrib4bv);
	PROC(glVertexAttrib4d);
	PROC(glVertexAttrib4dv);
	PROC(glVertexAttrib4f);
	PROC(glVertexAttrib4fv);
	PROC(glVertexAttrib4iv);
	PROC(glVertexAttrib4s);
	PROC(glVertexAttrib4sv);
	PROC(glVertexAttrib4ubv);
	PROC(glVertexAttrib4uiv);
	PROC(glVertexAttrib4usv);
	PROC(glVertexAttribPointer);
	if(retval)
	{
		DEBUG("was unable to load some GL_VERSION_2_0 functions\n");
		return 1;
	}

	if(gl_current_version < 0x201)
	{
		goto EXT;
	}
	PROC(glUniformMatrix2x3fv);
	PROC(glUniformMatrix3x2fv);
	PROC(glUniformMatrix2x4fv);
	PROC(glUniformMatrix4x2fv);
	PROC(glUniformMatrix3x4fv);
	PROC(glUniformMatrix4x3fv);
	if(retval)
	{
		DEBUG("was unable to load some GL_VERSION_2_1 functions\n");
		return 1;
	}

	if(gl_current_version < 0x300)
	{
		goto EXT;
	}
	PROC(glBeginConditionalRender);
	PROC(glBeginTransformFeedback);
	PROC(glBindFragDataLocation);
	PROC(glBindBufferRange);
	PROC(glBindBufferBase);
	PROC(glClampColor);
	PROC(glClearBufferfi);
	PROC(glClearBufferfv);
	PROC(glClearBufferiv);
	PROC(glClearBufferuiv);
	PROC(glColorMaski);
	PROC(glDisablei);
	PROC(glEnablei);
	PROC(glEndConditionalRender);
	PROC(glEndTransformFeedback);
	PROC(glGetBooleani_v);
	PROC(glGetIntegeri_v);
	PROC(glGetFragDataLocation);
	PROC(glGetStringi);
	PROC(glGetTexParameterIiv);
	PROC(glGetTexParameterIuiv);
	PROC(glGetTransformFeedbackVarying);
	PROC(glGetUniformuiv);
	PROC(glGetVertexAttribIiv);
	PROC(glGetVertexAttribIuiv);
	PROC(glIsEnabledi);
	PROC(glTexParameterIiv);
	PROC(glTexParameterIuiv);
	PROC(glTransformFeedbackVaryings);
	PROC(glUniform1ui);
	PROC(glUniform1uiv);
	PROC(glUniform2ui);
	PROC(glUniform2uiv);
	PROC(glUniform3ui);
	PROC(glUniform3uiv);
	PROC(glUniform4ui);
	PROC(glUniform4uiv);
	PROC(glVertexAttribI1i);
	PROC(glVertexAttribI1iv);
	PROC(glVertexAttribI1ui);
	PROC(glVertexAttribI1uiv);
	PROC(glVertexAttribI2i);
	PROC(glVertexAttribI2iv);
	PROC(glVertexAttribI2ui);
	PROC(glVertexAttribI2uiv);
	PROC(glVertexAttribI3i);
	PROC(glVertexAttribI3iv);
	PROC(glVertexAttribI3ui);
	PROC(glVertexAttribI3uiv);
	PROC(glVertexAttribI4bv);
	PROC(glVertexAttribI4i);
	PROC(glVertexAttribI4iv);
	PROC(glVertexAttribI4sv);
	PROC(glVertexAttribI4ubv);
	PROC(glVertexAttribI4ui);
	PROC(glVertexAttribI4uiv);
	PROC(glVertexAttribI4usv);
	PROC(glVertexAttribIPointer);
	PROC(glBindFramebuffer);
	PROC(glBindRenderbuffer);
	PROC(glBlitFramebuffer);
	PROC(glCheckFramebufferStatus);
	PROC(glDeleteFramebuffers);
	PROC(glDeleteRenderbuffers);
	PROC(glFramebufferRenderbuffer);
	PROC(glFramebufferTexture1D);
	PROC(glFramebufferTexture2D);
	PROC(glFramebufferTexture3D);
	PROC(glFramebufferTextureLayer);
	PROC(glGenFramebuffers);
	PROC(glGenRenderbuffers);
	PROC(glGenerateMipmap);
	PROC(glGetFramebufferAttachmentParameteriv);
	PROC(glGetRenderbufferParameteriv);
	PROC(glIsFramebuffer);
	PROC(glIsRenderbuffer);
	PROC(glRenderbufferStorage);
	PROC(glRenderbufferStorageMultisample);
	PROC(glFlushMappedBufferRange);
	PROC(glMapBufferRange);
	PROC(glBindVertexArray);
	PROC(glDeleteVertexArrays);
	PROC(glGenVertexArrays);
	PROC(glIsVertexArray);
	if(retval)
	{
		DEBUG("was unable to load some GL_VERSION_3_0 functions\n");
		return 1;
	}

	if(gl_current_version < 0x301)
	{
		goto EXT;
	}
	PROC(glDrawArraysInstanced);
	PROC(glDrawElementsInstanced);
	PROC(glPrimitiveRestartIndex);
	PROC(glTexBuffer);
	PROC(glGetActiveUniformBlockName);
	PROC(glGetActiveUniformBlockiv);
	PROC(glGetActiveUniformName);
	PROC(glGetActiveUniformsiv);
	PROC(glGetUniformBlockIndex);
	PROC(glGetUniformIndices);
	PROC(glUniformBlockBinding);
	PROC(glCopyBufferSubData);
	if(retval)
	{
		DEBUG("was unable to load some GL_VERSION_3_1 functions\n");
		return 1;
	}

	if(gl_current_version < 0x302)
	{
		goto EXT;
	}
	PROC(glFramebufferTexture);
	PROC(glGetBufferParameteri64v);
	PROC(glGetInteger64i_v);
	PROC(glDrawElementsBaseVertex);
	PROC(glDrawElementsInstancedBaseVertex);
	PROC(glDrawRangeElementsBaseVertex);
	PROC(glMultiDrawElementsBaseVertex);
	PROC(glProvokingVertex);
	PROC(glClientWaitSync);
	PROC(glDeleteSync);
	PROC(glFenceSync);
	PROC(glGetInteger64v);
	PROC(glGetSynciv);
	PROC(glIsSync);
	PROC(glWaitSync);
	PROC(glGetMultisamplefv);
	PROC(glSampleMaski);
	PROC(glTexImage2DMultisample);
	PROC(glTexImage3DMultisample);
	if(retval)
	{
		DEBUG("was unable to load some GL_VERSION_3_2 functions\n");
		return 1;
	}

	if(gl_current_version < 0x303)
	{
		goto EXT;
	}
	PROC(glVertexAttribDivisor);
	PROC(glBindFragDataLocationIndexed);
	PROC(glGetFragDataIndex);
	PROC(glBindSampler);
	PROC(glDeleteSamplers);
	PROC(glGenSamplers);
	PROC(glGetSamplerParameterIiv);
	PROC(glGetSamplerParameterIuiv);
	PROC(glGetSamplerParameterfv);
	PROC(glGetSamplerParameteriv);
	PROC(glIsSampler);
	PROC(glSamplerParameterIiv);
	PROC(glSamplerParameterIuiv);
	PROC(glSamplerParameterf);
	PROC(glSamplerParameterfv);
	PROC(glSamplerParameteri);
	PROC(glSamplerParameteriv);
	PROC(glVertexAttribP1ui);
	PROC(glVertexAttribP1uiv);
	PROC(glVertexAttribP2ui);
	PROC(glVertexAttribP2uiv);
	PROC(glVertexAttribP3ui);
	PROC(glVertexAttribP3uiv);
	PROC(glVertexAttribP4ui);
	PROC(glVertexAttribP4uiv);
	if(retval)
	{
		DEBUG("was unable to load some GL_VERSION_3_3 functions\n");
		return 1;
	}

	if(gl_current_version < 0x400)
	{
		goto EXT;
	}

	PROC_ARB(glGetQueryObjecti64v);
	PROC_ARB(glGetQueryObjectui64v);
	PROC_ARB(glQueryCounter);

	PROC_ARB(glBlendEquationSeparatei);
	PROC_ARB(glBlendEquationi);
	PROC_ARB(glBlendFuncSeparatei);
	PROC_ARB(glBlendFunci);
	PROC_ARB(glMinSampleShading);
	PROC_ARB(glDrawArraysIndirect);
	PROC_ARB(glDrawElementsIndirect);
	PROC_ARB(glGetUniformdv);
	PROC_ARB(glUniform1d);
	PROC_ARB(glUniform1dv);
	PROC_ARB(glUniform2d);
	PROC_ARB(glUniform2dv);
	PROC_ARB(glUniform3d);
	PROC_ARB(glUniform3dv);
	PROC_ARB(glUniform4d);
	PROC_ARB(glUniform4dv);
	PROC_ARB(glUniformMatrix2dv);
	PROC_ARB(glUniformMatrix2x3dv);
	PROC_ARB(glUniformMatrix2x4dv);
	PROC_ARB(glUniformMatrix3dv);
	PROC_ARB(glUniformMatrix3x2dv);
	PROC_ARB(glUniformMatrix3x4dv);
	PROC_ARB(glUniformMatrix4dv);
	PROC_ARB(glUniformMatrix4x2dv);
	PROC_ARB(glUniformMatrix4x3dv);
	PROC_ARB(glGetActiveSubroutineName);
	PROC_ARB(glGetActiveSubroutineUniformName);
	PROC_ARB(glGetActiveSubroutineUniformiv);
	PROC_ARB(glGetProgramStageiv);
	PROC_ARB(glGetSubroutineIndex);
	PROC_ARB(glGetSubroutineUniformLocation);
	PROC_ARB(glGetUniformSubroutineuiv);
	PROC_ARB(glUniformSubroutinesuiv);
	PROC_ARB(glPatchParameterfv);
	PROC_ARB(glPatchParameteri);
	PROC_ARB(glBindTransformFeedback);
	PROC_ARB(glDeleteTransformFeedbacks);
	PROC_ARB(glDrawTransformFeedback);
	PROC_ARB(glGenTransformFeedbacks);
	PROC_ARB(glIsTransformFeedback);
	PROC_ARB(glPauseTransformFeedback);
	PROC_ARB(glResumeTransformFeedback);
	PROC_ARB(glBeginQueryIndexed);
	PROC_ARB(glDrawTransformFeedbackStream);
	PROC_ARB(glEndQueryIndexed);
	PROC_ARB(glGetQueryIndexediv);
	if(retval)
	{
		DEBUG("was unable to load some GL_VERSION_4_0 functions\n");
		return 1;
	}

	if(gl_current_version < 0x401)
	{
		goto EXT;
	}
	PROC_ARB(glClearDepthf);
	PROC_ARB(glDepthRangef);
	PROC_ARB(glReleaseShaderCompiler);
	PROC_ARB(glGetShaderPrecisionFormat);
	PROC_ARB(glShaderBinary);
	PROC_ARB(glGetProgramBinary);
	PROC_ARB(glProgramBinary);
	PROC_ARB(glProgramParameteri);
	PROC_ARB(glActiveShaderProgram);
	PROC_ARB(glBindProgramPipeline);
	PROC_ARB(glCreateShaderProgramv);
	PROC_ARB(glDeleteProgramPipelines);
	PROC_ARB(glGenProgramPipelines);
	PROC_ARB(glGetProgramPipelineInfoLog);
	PROC_ARB(glGetProgramPipelineiv);
	PROC_ARB(glIsProgramPipeline);
	PROC_ARB(glProgramUniform1d);
	PROC_ARB(glProgramUniform1dv);
	PROC_ARB(glProgramUniform1f);
	PROC_ARB(glProgramUniform1fv);
	PROC_ARB(glProgramUniform1i);
	PROC_ARB(glProgramUniform1iv);
	PROC_ARB(glProgramUniform1ui);
	PROC_ARB(glProgramUniform1uiv);
	PROC_ARB(glProgramUniform2d);
	PROC_ARB(glProgramUniform2dv);
	PROC_ARB(glProgramUniform2f);
	PROC_ARB(glProgramUniform2fv);
	PROC_ARB(glProgramUniform2i);
	PROC_ARB(glProgramUniform2iv);
	PROC_ARB(glProgramUniform2ui);
	PROC_ARB(glProgramUniform2uiv);
	PROC_ARB(glProgramUniform3d);
	PROC_ARB(glProgramUniform3dv);
	PROC_ARB(glProgramUniform3f);
	PROC_ARB(glProgramUniform3fv);
	PROC_ARB(glProgramUniform3i);
	PROC_ARB(glProgramUniform3iv);
	PROC_ARB(glProgramUniform3ui);
	PROC_ARB(glProgramUniform3uiv);
	PROC_ARB(glProgramUniform4d);
	PROC_ARB(glProgramUniform4dv);
	PROC_ARB(glProgramUniform4f);
	PROC_ARB(glProgramUniform4fv);
	PROC_ARB(glProgramUniform4i);
	PROC_ARB(glProgramUniform4iv);
	PROC_ARB(glProgramUniform4ui);
	PROC_ARB(glProgramUniform4uiv);
	PROC_ARB(glProgramUniformMatrix2dv);
	PROC_ARB(glProgramUniformMatrix2fv);
	PROC_ARB(glProgramUniformMatrix2x3dv);
	PROC_ARB(glProgramUniformMatrix2x3fv);
	PROC_ARB(glProgramUniformMatrix2x4dv);
	PROC_ARB(glProgramUniformMatrix2x4fv);
	PROC_ARB(glProgramUniformMatrix3dv);
	PROC_ARB(glProgramUniformMatrix3fv);
	PROC_ARB(glProgramUniformMatrix3x2dv);
	PROC_ARB(glProgramUniformMatrix3x2fv);
	PROC_ARB(glProgramUniformMatrix3x4dv);
	PROC_ARB(glProgramUniformMatrix3x4fv);
	PROC_ARB(glProgramUniformMatrix4dv);
	PROC_ARB(glProgramUniformMatrix4fv);
	PROC_ARB(glProgramUniformMatrix4x2dv);
	PROC_ARB(glProgramUniformMatrix4x2fv);
	PROC_ARB(glProgramUniformMatrix4x3dv);
	PROC_ARB(glProgramUniformMatrix4x3fv);
	PROC_ARB(glUseProgramStages);
	PROC_ARB(glValidateProgramPipeline);
	PROC_ARB(glGetVertexAttribLdv);
	PROC_ARB(glVertexAttribL1d);
	PROC_ARB(glVertexAttribL1dv);
	PROC_ARB(glVertexAttribL2d);
	PROC_ARB(glVertexAttribL2dv);
	PROC_ARB(glVertexAttribL3d);
	PROC_ARB(glVertexAttribL3dv);
	PROC_ARB(glVertexAttribL4d);
	PROC_ARB(glVertexAttribL4dv);
	PROC_ARB(glVertexAttribLPointer);
	PROC_ARB(glDepthRangeArrayv);
	PROC_ARB(glDepthRangeIndexed);
	PROC_ARB(glGetDoublei_v);
	PROC_ARB(glGetFloati_v);
	PROC_ARB(glScissorArrayv);
	PROC_ARB(glScissorIndexed);
	PROC_ARB(glScissorIndexedv);
	PROC_ARB(glViewportArrayv);
	PROC_ARB(glViewportIndexedf);
	PROC_ARB(glViewportIndexedfv);
	if(retval)
	{
		DEBUG("was unable to load some GL_VERSION_4_1 functions\n");
		return 1;
	}

	if(gl_current_version < 0x402)
	{
		goto EXT;
	}
	PROC_ARB(glDrawArraysInstancedBaseInstance);
	PROC_ARB(glDrawElementsInstancedBaseInstance);
	PROC_ARB(glDrawElementsInstancedBaseVertexBaseInstance);
	PROC_ARB(glGetInternalformativ);
	PROC_ARB(glGetActiveAtomicCounterBufferiv);
	PROC_ARB(glBindImageTexture);
	PROC_ARB(glMemoryBarrier);
	PROC_ARB(glTexStorage1D);
	PROC_ARB(glTexStorage2D);
	PROC_ARB(glTexStorage3D);
	PROC_ARB(glDrawTransformFeedbackInstanced);
	PROC_ARB(glDrawTransformFeedbackStreamInstanced);
	if(retval)
	{
		DEBUG("was unable to load some GL_VERSION_4_2 functions\n");
		return 1;
	}

	if(gl_current_version < 0x403)
	{
		goto EXT;
	}
	PROC_ARB(glClearBufferData);
	PROC_ARB(glClearBufferSubData);
	PROC_ARB(glClearNamedBufferDataEXT);
	PROC_ARB(glClearNamedBufferSubDataEXT);
	PROC_ARB(glDispatchCompute);
	PROC_ARB(glDispatchComputeIndirect);
	PROC_ARB(glCopyImageSubData);
	PROC_ARB(glFramebufferParameteri);
	PROC_ARB(glGetFramebufferParameteriv);
	PROC_ARB(glGetNamedFramebufferParameterivEXT);
	PROC_ARB(glNamedFramebufferParameteriEXT);
	PROC_ARB(glGetInternalformati64v);
	PROC_ARB(glInvalidateBufferData);
	PROC_ARB(glInvalidateBufferSubData);
	PROC_ARB(glInvalidateFramebuffer);
	PROC_ARB(glInvalidateSubFramebuffer);
	PROC_ARB(glInvalidateTexImage);
	PROC_ARB(glInvalidateTexSubImage);
	PROC_ARB(glMultiDrawArraysIndirect);
	PROC_ARB(glMultiDrawElementsIndirect);
	PROC_ARB(glGetProgramInterfaceiv);
	PROC_ARB(glGetProgramResourceIndex);
	PROC_ARB(glGetProgramResourceLocation);
	PROC_ARB(glGetProgramResourceLocationIndex);
	PROC_ARB(glGetProgramResourceName);
	PROC_ARB(glGetProgramResourceiv);
	PROC_ARB(glShaderStorageBlockBinding);
	PROC_ARB(glTexBufferRange);
	PROC_ARB(glTextureBufferRangeEXT);
	PROC_ARB(glTexStorage2DMultisample);
	PROC_ARB(glTexStorage3DMultisample);
	PROC_ARB(glTextureStorage2DMultisampleEXT);
	PROC_ARB(glTextureStorage3DMultisampleEXT);
	PROC_ARB(glTextureView);
	PROC_ARB(glBindVertexBuffer);
	PROC_ARB(glVertexArrayBindVertexBufferEXT);
	PROC_ARB(glVertexArrayVertexAttribBindingEXT);
	PROC_ARB(glVertexArrayVertexAttribFormatEXT);
	PROC_ARB(glVertexArrayVertexAttribIFormatEXT);
	PROC_ARB(glVertexArrayVertexAttribLFormatEXT);
	PROC_ARB(glVertexArrayVertexBindingDivisorEXT);
	PROC_ARB(glVertexAttribBinding);
	PROC_ARB(glVertexAttribFormat);
	PROC_ARB(glVertexAttribIFormat);
	PROC_ARB(glVertexAttribLFormat);
	PROC_ARB(glVertexBindingDivisor);
	PROC_ARB(glDebugMessageCallback);
	PROC_ARB(glDebugMessageControl);
	PROC_ARB(glDebugMessageInsert);
	PROC_ARB(glGetDebugMessageLog);
	PROC_ARB(glGetObjectLabel);
	PROC_ARB(glGetObjectPtrLabel);
	PROC_ARB(glObjectLabel);
	PROC_ARB(glObjectPtrLabel);
	PROC_ARB(glPopDebugGroup);
	PROC_ARB(glPushDebugGroup);
	if(retval)
	{
		DEBUG("was unable to load some GL_VERSION_4_3 functions\n");
		return 1;
	}

	if(gl_current_version < 0x404)
	{
		goto EXT;
	}
	PROC_ARB(glBufferStorage);
	PROC_ARB(glClearTexImage);
	PROC_ARB(glClearTexSubImage);
	PROC_ARB(glBindBuffersBase);
	PROC_ARB(glBindBuffersRange);
	PROC_ARB(glBindImageTextures);
	PROC_ARB(glBindSamplers);
	PROC_ARB(glBindTextures);
	PROC_ARB(glBindVertexBuffers);
	if(retval)
	{
		DEBUG("was unable to load some GL_VERSION_4_4 functions\n");
		return 1;
	}

	if(gl_current_version < 0x405)
	{
		goto EXT;
	}
	PROC_ARB(glGetGraphicsResetStatus);
	PROC_ARB(glGetnCompressedTexImage);
	PROC_ARB(glGetnTexImage);
	PROC_ARB(glGetnUniformdv);
	PROC_ARB(glClipControl);
	PROC_ARB(glBindTextureUnit);
	PROC_ARB(glBlitNamedFramebuffer);
	PROC_ARB(glCheckNamedFramebufferStatus);
	PROC_ARB(glClearNamedBufferData);
	PROC_ARB(glClearNamedBufferSubData);
	PROC_ARB(glClearNamedFramebufferfi);
	PROC_ARB(glClearNamedFramebufferfv);
	PROC_ARB(glClearNamedFramebufferiv);
	PROC_ARB(glClearNamedFramebufferuiv);
	PROC_ARB(glCompressedTextureSubImage1D);
	PROC_ARB(glCompressedTextureSubImage2D);
	PROC_ARB(glCompressedTextureSubImage3D);
	PROC_ARB(glCopyNamedBufferSubData);
	PROC_ARB(glCopyTextureSubImage1D);
	PROC_ARB(glCopyTextureSubImage2D);
	PROC_ARB(glCopyTextureSubImage3D);
	PROC_ARB(glCreateBuffers);
	PROC_ARB(glCreateFramebuffers);
	PROC_ARB(glCreateProgramPipelines);
	PROC_ARB(glCreateQueries);
	PROC_ARB(glCreateRenderbuffers);
	PROC_ARB(glCreateSamplers);
	PROC_ARB(glCreateTextures);
	PROC_ARB(glCreateTransformFeedbacks);
	PROC_ARB(glCreateVertexArrays);
	PROC_ARB(glDisableVertexArrayAttrib);
	PROC_ARB(glEnableVertexArrayAttrib);
	PROC_ARB(glFlushMappedNamedBufferRange);
	PROC_ARB(glGenerateTextureMipmap);
	PROC_ARB(glGetCompressedTextureImage);
	PROC_ARB(glGetNamedBufferParameteri64v);
	PROC_ARB(glGetNamedBufferParameteriv);
	PROC_ARB(glGetNamedBufferPointerv);
	PROC_ARB(glGetNamedBufferSubData);
	PROC_ARB(glGetNamedFramebufferAttachmentParameteriv);
	PROC_ARB(glGetNamedFramebufferParameteriv);
	PROC_ARB(glGetNamedRenderbufferParameteriv);
	PROC_ARB(glGetQueryBufferObjecti64v);
	PROC_ARB(glGetQueryBufferObjectiv);
	PROC_ARB(glGetQueryBufferObjectui64v);
	PROC_ARB(glGetQueryBufferObjectuiv);
	PROC_ARB(glGetTextureImage);
	PROC_ARB(glGetTextureLevelParameterfv);
	PROC_ARB(glGetTextureLevelParameteriv);
	PROC_ARB(glGetTextureParameterIiv);
	PROC_ARB(glGetTextureParameterIuiv);
	PROC_ARB(glGetTextureParameterfv);
	PROC_ARB(glGetTextureParameteriv);
	PROC_ARB(glGetTransformFeedbacki64_v);
	PROC_ARB(glGetTransformFeedbacki_v);
	PROC_ARB(glGetTransformFeedbackiv);
	PROC_ARB(glGetVertexArrayIndexed64iv);
	PROC_ARB(glGetVertexArrayIndexediv);
	PROC_ARB(glGetVertexArrayiv);
	PROC_ARB(glInvalidateNamedFramebufferData);
	PROC_ARB(glInvalidateNamedFramebufferSubData);
	PROC_ARB(glMapNamedBuffer);
	PROC_ARB(glMapNamedBufferRange);
	PROC_ARB(glNamedBufferData);
	PROC_ARB(glNamedBufferStorage);
	PROC_ARB(glNamedBufferSubData);
	PROC_ARB(glNamedFramebufferDrawBuffer);
	PROC_ARB(glNamedFramebufferDrawBuffers);
	PROC_ARB(glNamedFramebufferParameteri);
	PROC_ARB(glNamedFramebufferReadBuffer);
	PROC_ARB(glNamedFramebufferRenderbuffer);
	PROC_ARB(glNamedFramebufferTexture);
	PROC_ARB(glNamedFramebufferTextureLayer);
	PROC_ARB(glNamedRenderbufferStorage);
	PROC_ARB(glNamedRenderbufferStorageMultisample);
	PROC_ARB(glTextureBuffer);
	PROC_ARB(glTextureBufferRange);
	PROC_ARB(glTextureParameterIiv);
	PROC_ARB(glTextureParameterIuiv);
	PROC_ARB(glTextureParameterf);
	PROC_ARB(glTextureParameterfv);
	PROC_ARB(glTextureParameteri);
	PROC_ARB(glTextureParameteriv);
	PROC_ARB(glTextureStorage1D);
	PROC_ARB(glTextureStorage2D);
	PROC_ARB(glTextureStorage2DMultisample);
	PROC_ARB(glTextureStorage3D);
	PROC_ARB(glTextureStorage3DMultisample);
	PROC_ARB(glTextureSubImage1D);
	PROC_ARB(glTextureSubImage2D);
	PROC_ARB(glTextureSubImage3D);
	PROC_ARB(glTransformFeedbackBufferBase);
	PROC_ARB(glTransformFeedbackBufferRange);
	PROC_ARB(glUnmapNamedBuffer);
	PROC_ARB(glVertexArrayAttribBinding);
	PROC_ARB(glVertexArrayAttribFormat);
	PROC_ARB(glVertexArrayAttribIFormat);
	PROC_ARB(glVertexArrayAttribLFormat);
	PROC_ARB(glVertexArrayBindingDivisor);
	PROC_ARB(glVertexArrayElementBuffer);
	PROC_ARB(glVertexArrayVertexBuffer);
	PROC_ARB(glVertexArrayVertexBuffers);
	PROC_ARB(glGetnUniformfv);
	PROC_ARB(glGetnUniformiv);
	PROC_ARB(glGetnUniformuiv);
	PROC_ARB(glReadnPixels);
	PROC_ARB(glTextureBarrier);
	PROC_ARB(glMemoryBarrierByRegion);
	PROC_ARB(glGetCompressedTextureSubImage);
	PROC_ARB(glGetTextureSubImage);
	if(retval)
	{
		DEBUG("was unable to load some GL_VERSION_4_5 functions\n");
		return 1;
	}

	if(gl_current_version < 0x406)
	{
		goto EXT;
	}
	PROC_ARB(glMultiDrawArraysIndirectCount);
	PROC_ARB(glMultiDrawElementsIndirectCount);
	PROC_ARB(glSpecializeShader);
	PROC_ARB(glPolygonOffsetClamp);
	PROC_ARB(glPrimitiveBoundingBoxARB);
	if(retval)
	{
		DEBUG("was unable to load some GL_VERSION_4_6 functions\n");
		return 1;
	}

	EXT:
	retval=0;
	PROC_EXT(glBindFramebuffer)
	PROC_EXT(glCheckFramebufferStatus)
	PROC_EXT(glDeleteFramebuffers)
	PROC_EXT(glFramebufferTexture1D)
	PROC_EXT(glFramebufferTexture2D)
	PROC_EXT(glFramebufferTexture3D)
	PROC_EXT(glFramebufferTextureLayer)
	PROC_EXT(glGenFramebuffers)
	PROC_EXT(glGenerateMipmap)
	PROC_EXT(glGetFramebufferAttachmentParameteriv)
	PROC_EXT(glIsFramebuffer)

	PROC_EXT(glBindRenderbuffer)
	PROC_EXT(glDeleteRenderbuffers)
	PROC_EXT(glFramebufferRenderbuffer)
	PROC_EXT(glGenRenderbuffers)
	PROC_EXT(glRenderbufferStorage)
	PROC_EXT(glGetRenderbufferParameteriv)
	PROC_EXT(glIsRenderbuffer)

	PROC_EXT(glRenderbufferStorageMultisample)
	if(!glRenderbufferStorageMultisample)
		glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)GLWObtain(libgl,"glRenderbufferStorageMultisampleANGLE");
	if(!glRenderbufferStorageMultisample)
		glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)GLWObtain(libgl,"glRenderbufferStorageMultisampleAPPLE");
	return 0;
}
/* ----------------------------- GL_VERSION_1_0 ---------------------------- \*/
PFNGLCULLFACEPROC glCullFace;
PFNGLFRONTFACEPROC glFrontFace;
PFNGLHINTPROC glHint;
PFNGLLINEWIDTHPROC glLineWidth;
PFNGLPOINTSIZEPROC glPointSize;
PFNGLSCISSORPROC glScissor;
PFNGLTEXPARAMETERFPROC glTexParameterf;
PFNGLTEXPARAMETERFVPROC glTexParameterfv;
PFNGLTEXPARAMETERIPROC glTexParameteri;
PFNGLTEXPARAMETERIVPROC glTexParameteriv;
PFNGLTEXIMAGE1DPROC glTexImage1D;
PFNGLTEXIMAGE2DPROC glTexImage2D;
PFNGLDRAWBUFFERPROC glDrawBuffer;
PFNGLCLEARPROC glClear;
PFNGLCLEARCOLORPROC glClearColor;
PFNGLCLEARSTENCILPROC glClearStencil;
PFNGLCLEARDEPTHPROC glClearDepth;
PFNGLSTENCILMASKPROC glStencilMask;
PFNGLCOLORMASKPROC glColorMask;
PFNGLDEPTHMASKPROC glDepthMask;
PFNGLDISABLEPROC glDisable;
PFNGLENABLEPROC glEnable;
PFNGLFINISHPROC glFinish;
PFNGLFLUSHPROC glFlush;
PFNGLBLENDFUNCPROC glBlendFunc;
PFNGLLOGICOPPROC glLogicOp;
PFNGLSTENCILFUNCPROC glStencilFunc;
PFNGLSTENCILOPPROC glStencilOp;
PFNGLDEPTHFUNCPROC glDepthFunc;
PFNGLPIXELSTOREIPROC glPixelStorei;
PFNGLREADBUFFERPROC glReadBuffer;
PFNGLREADPIXELSPROC glReadPixels;
PFNGLGETBOOLEANVPROC glGetBooleanv;
PFNGLGETDOUBLEVPROC glGetDoublev;
PFNGLGETERRORPROC glGetError;
PFNGLGETFLOATVPROC glGetFloatv;
PFNGLGETINTEGERVPROC glGetIntegerv;
PFNGLGETSTRINGPROC glGetString;
PFNGLGETTEXIMAGEPROC glGetTexImage;
PFNGLGETTEXPARAMETERFVPROC glGetTexParameterfv;
PFNGLGETTEXPARAMETERIVPROC glGetTexParameteriv;
PFNGLGETTEXLEVELPARAMETERFVPROC glGetTexLevelParameterfv;
PFNGLGETTEXLEVELPARAMETERIVPROC glGetTexLevelParameteriv;
PFNGLISENABLEDPROC glIsEnabled;
PFNGLDEPTHRANGEPROC glDepthRange;
PFNGLVIEWPORTPROC glViewport;
/* ----------------------------- GL_VERSION_1_1 ---------------------------- \*/
PFNGLBINDTEXTUREPROC glBindTexture;
PFNGLDRAWARRAYSPROC glDrawArrays;
PFNGLDRAWELEMENTSPROC glDrawElements;
PFNGLCOPYTEXIMAGE2DPROC glCopyTexImage2D;
PFNGLCOPYTEXSUBIMAGE2DPROC glCopyTexSubImage2D;
PFNGLDELETETEXTURESPROC glDeleteTextures;
PFNGLGENTEXTURESPROC glGenTextures;
PFNGLGETPOINTERVPROC glGetPointerv;
PFNGLISTEXTUREPROC glIsTexture;
PFNGLTEXSUBIMAGE2DPROC glTexSubImage2D;
/* ----------------------------- GL_VERSION_1_2 ---------------------------- \*/
PFNGLCOPYTEXSUBIMAGE3DPROC glCopyTexSubImage3D;
PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements;
PFNGLTEXIMAGE3DPROC glTexImage3D;
PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D;
/* ----------------------------- GL_VERSION_1_3 ---------------------------- \*/
PFNGLACTIVETEXTUREPROC glActiveTexture;
PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D;
PFNGLCOMPRESSEDTEXIMAGE3DPROC glCompressedTexImage3D;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC glCompressedTexSubImage3D;
PFNGLGETCOMPRESSEDTEXIMAGEPROC glGetCompressedTexImage;
PFNGLSAMPLECOVERAGEPROC glSampleCoverage;
/* ----------------------------- GL_VERSION_1_4 ---------------------------- \*/
PFNGLBLENDCOLORPROC glBlendColor;
PFNGLBLENDEQUATIONPROC glBlendEquation;
PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate;
PFNGLMULTIDRAWARRAYSPROC glMultiDrawArrays;
PFNGLMULTIDRAWELEMENTSPROC glMultiDrawElements;
PFNGLPOINTPARAMETERFPROC glPointParameterf;
PFNGLPOINTPARAMETERFVPROC glPointParameterfv;
PFNGLPOINTPARAMETERIPROC glPointParameteri;
PFNGLPOINTPARAMETERIVPROC glPointParameteriv;
/* ----------------------------- GL_VERSION_1_5 ---------------------------- \*/
PFNGLBEGINQUERYPROC glBeginQuery;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLBUFFERSUBDATAPROC glBufferSubData;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLDELETEQUERIESPROC glDeleteQueries;
PFNGLENDQUERYPROC glEndQuery;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLGENQUERIESPROC glGenQueries;
PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv;
PFNGLGETBUFFERPOINTERVPROC glGetBufferPointerv;
PFNGLGETBUFFERSUBDATAPROC glGetBufferSubData;
PFNGLGETQUERYOBJECTIVPROC glGetQueryObjectiv;
PFNGLGETQUERYOBJECTUIVPROC glGetQueryObjectuiv;
PFNGLGETQUERYIVPROC glGetQueryiv;
PFNGLISBUFFERPROC glIsBuffer;
PFNGLISQUERYPROC glIsQuery;
PFNGLMAPBUFFERPROC glMapBuffer;
PFNGLUNMAPBUFFERPROC glUnmapBuffer;
/* ----------------------------- GL_VERSION_2_0 ---------------------------- \*/
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
PFNGLBLENDEQUATIONSEPARATEPROC glBlendEquationSeparate;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLDETACHSHADERPROC glDetachShader;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
PFNGLDRAWBUFFERSPROC glDrawBuffers;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib;
PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform;
PFNGLGETATTACHEDSHADERSPROC glGetAttachedShaders;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLGETSHADERSOURCEPROC glGetShaderSource;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLGETUNIFORMFVPROC glGetUniformfv;
PFNGLGETUNIFORMIVPROC glGetUniformiv;
PFNGLGETVERTEXATTRIBPOINTERVPROC glGetVertexAttribPointerv;
PFNGLGETVERTEXATTRIBDVPROC glGetVertexAttribdv;
PFNGLGETVERTEXATTRIBFVPROC glGetVertexAttribfv;
PFNGLGETVERTEXATTRIBIVPROC glGetVertexAttribiv;
PFNGLISPROGRAMPROC glIsProgram;
PFNGLISSHADERPROC glIsShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate;
PFNGLSTENCILMASKSEPARATEPROC glStencilMaskSeparate;
PFNGLSTENCILOPSEPARATEPROC glStencilOpSeparate;
PFNGLUNIFORM1FPROC glUniform1f;
PFNGLUNIFORM1FVPROC glUniform1fv;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLUNIFORM1IVPROC glUniform1iv;
PFNGLUNIFORM2FPROC glUniform2f;
PFNGLUNIFORM2FVPROC glUniform2fv;
PFNGLUNIFORM2IPROC glUniform2i;
PFNGLUNIFORM2IVPROC glUniform2iv;
PFNGLUNIFORM3FPROC glUniform3f;
PFNGLUNIFORM3FVPROC glUniform3fv;
PFNGLUNIFORM3IPROC glUniform3i;
PFNGLUNIFORM3IVPROC glUniform3iv;
PFNGLUNIFORM4FPROC glUniform4f;
PFNGLUNIFORM4FVPROC glUniform4fv;
PFNGLUNIFORM4IPROC glUniform4i;
PFNGLUNIFORM4IVPROC glUniform4iv;
PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv;
PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLVALIDATEPROGRAMPROC glValidateProgram;
PFNGLVERTEXATTRIB1DPROC glVertexAttrib1d;
PFNGLVERTEXATTRIB1DVPROC glVertexAttrib1dv;
PFNGLVERTEXATTRIB1FPROC glVertexAttrib1f;
PFNGLVERTEXATTRIB1FVPROC glVertexAttrib1fv;
PFNGLVERTEXATTRIB1SPROC glVertexAttrib1s;
PFNGLVERTEXATTRIB1SVPROC glVertexAttrib1sv;
PFNGLVERTEXATTRIB2DPROC glVertexAttrib2d;
PFNGLVERTEXATTRIB2DVPROC glVertexAttrib2dv;
PFNGLVERTEXATTRIB2FPROC glVertexAttrib2f;
PFNGLVERTEXATTRIB2FVPROC glVertexAttrib2fv;
PFNGLVERTEXATTRIB2SPROC glVertexAttrib2s;
PFNGLVERTEXATTRIB2SVPROC glVertexAttrib2sv;
PFNGLVERTEXATTRIB3DPROC glVertexAttrib3d;
PFNGLVERTEXATTRIB3DVPROC glVertexAttrib3dv;
PFNGLVERTEXATTRIB3FPROC glVertexAttrib3f;
PFNGLVERTEXATTRIB3FVPROC glVertexAttrib3fv;
PFNGLVERTEXATTRIB3SPROC glVertexAttrib3s;
PFNGLVERTEXATTRIB3SVPROC glVertexAttrib3sv;
PFNGLVERTEXATTRIB4NBVPROC glVertexAttrib4Nbv;
PFNGLVERTEXATTRIB4NIVPROC glVertexAttrib4Niv;
PFNGLVERTEXATTRIB4NSVPROC glVertexAttrib4Nsv;
PFNGLVERTEXATTRIB4NUBPROC glVertexAttrib4Nub;
PFNGLVERTEXATTRIB4NUBVPROC glVertexAttrib4Nubv;
PFNGLVERTEXATTRIB4NUIVPROC glVertexAttrib4Nuiv;
PFNGLVERTEXATTRIB4NUSVPROC glVertexAttrib4Nusv;
PFNGLVERTEXATTRIB4BVPROC glVertexAttrib4bv;
PFNGLVERTEXATTRIB4DPROC glVertexAttrib4d;
PFNGLVERTEXATTRIB4DVPROC glVertexAttrib4dv;
PFNGLVERTEXATTRIB4FPROC glVertexAttrib4f;
PFNGLVERTEXATTRIB4FVPROC glVertexAttrib4fv;
PFNGLVERTEXATTRIB4IVPROC glVertexAttrib4iv;
PFNGLVERTEXATTRIB4SPROC glVertexAttrib4s;
PFNGLVERTEXATTRIB4SVPROC glVertexAttrib4sv;
PFNGLVERTEXATTRIB4UBVPROC glVertexAttrib4ubv;
PFNGLVERTEXATTRIB4UIVPROC glVertexAttrib4uiv;
PFNGLVERTEXATTRIB4USVPROC glVertexAttrib4usv;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
/* ----------------------------- GL_VERSION_2_1 ---------------------------- \*/
PFNGLUNIFORMMATRIX2X3FVPROC glUniformMatrix2x3fv;
PFNGLUNIFORMMATRIX2X3FVPROC glUniformMatrix3x2fv;
PFNGLUNIFORMMATRIX2X3FVPROC glUniformMatrix2x4fv;
PFNGLUNIFORMMATRIX2X3FVPROC glUniformMatrix4x2fv;
PFNGLUNIFORMMATRIX2X3FVPROC glUniformMatrix3x4fv;
PFNGLUNIFORMMATRIX2X3FVPROC glUniformMatrix4x3fv;
/* ----------------------------- GL_VERSION_3_0 ---------------------------- \*/
PFNGLBEGINCONDITIONALRENDERPROC glBeginConditionalRender;
PFNGLBEGINTRANSFORMFEEDBACKPROC glBeginTransformFeedback;
PFNGLBINDFRAGDATALOCATIONPROC glBindFragDataLocation;
PFNGLBINDBUFFERRANGEPROC glBindBufferRange;
PFNGLBINDBUFFERBASEPROC glBindBufferBase;
PFNGLCLAMPCOLORPROC glClampColor;
PFNGLCLEARBUFFERFIPROC glClearBufferfi;
PFNGLCLEARBUFFERFVPROC glClearBufferfv;
PFNGLCLEARBUFFERIVPROC glClearBufferiv;
PFNGLCLEARBUFFERUIVPROC glClearBufferuiv;
PFNGLCOLORMASKIPROC glColorMaski;
PFNGLDISABLEIPROC glDisablei;
PFNGLENABLEIPROC glEnablei;
PFNGLENDCONDITIONALRENDERPROC glEndConditionalRender;
PFNGLENDTRANSFORMFEEDBACKPROC glEndTransformFeedback;
PFNGLGETBOOLEANI_VPROC glGetBooleani_v;
PFNGLGETINTEGERI_VPROC glGetIntegeri_v;
PFNGLGETFRAGDATALOCATIONPROC glGetFragDataLocation;
PFNGLGETSTRINGIPROC glGetStringi;
PFNGLGETTEXPARAMETERIIVPROC glGetTexParameterIiv;
PFNGLGETTEXPARAMETERIUIVPROC glGetTexParameterIuiv;
PFNGLGETTRANSFORMFEEDBACKVARYINGPROC glGetTransformFeedbackVarying;
PFNGLGETUNIFORMUIVPROC glGetUniformuiv;
PFNGLGETVERTEXATTRIBIIVPROC glGetVertexAttribIiv;
PFNGLGETVERTEXATTRIBIUIVPROC glGetVertexAttribIuiv;
PFNGLISENABLEDIPROC glIsEnabledi;
PFNGLTEXPARAMETERIIVPROC glTexParameterIiv;
PFNGLTEXPARAMETERIUIVPROC glTexParameterIuiv;
PFNGLTRANSFORMFEEDBACKVARYINGSPROC glTransformFeedbackVaryings;
PFNGLUNIFORM1UIPROC glUniform1ui;
PFNGLUNIFORM1UIVPROC glUniform1uiv;
PFNGLUNIFORM2UIPROC glUniform2ui;
PFNGLUNIFORM2UIVPROC glUniform2uiv;
PFNGLUNIFORM3UIPROC glUniform3ui;
PFNGLUNIFORM3UIVPROC glUniform3uiv;
PFNGLUNIFORM4UIPROC glUniform4ui;
PFNGLUNIFORM4UIVPROC glUniform4uiv;
PFNGLVERTEXATTRIBI1IPROC glVertexAttribI1i;
PFNGLVERTEXATTRIBI1IVPROC glVertexAttribI1iv;
PFNGLVERTEXATTRIBI1UIPROC glVertexAttribI1ui;
PFNGLVERTEXATTRIBI1UIVPROC glVertexAttribI1uiv;
PFNGLVERTEXATTRIBI2IPROC glVertexAttribI2i;
PFNGLVERTEXATTRIBI2IVPROC glVertexAttribI2iv;
PFNGLVERTEXATTRIBI2UIPROC glVertexAttribI2ui;
PFNGLVERTEXATTRIBI2UIVPROC glVertexAttribI2uiv;
PFNGLVERTEXATTRIBI3IPROC glVertexAttribI3i;
PFNGLVERTEXATTRIBI3IVPROC glVertexAttribI3iv;
PFNGLVERTEXATTRIBI3UIPROC glVertexAttribI3ui;
PFNGLVERTEXATTRIBI3UIVPROC glVertexAttribI3uiv;
PFNGLVERTEXATTRIBI4BVPROC glVertexAttribI4bv;
PFNGLVERTEXATTRIBI4IPROC glVertexAttribI4i;
PFNGLVERTEXATTRIBI4IVPROC glVertexAttribI4iv;
PFNGLVERTEXATTRIBI4SVPROC glVertexAttribI4sv;
PFNGLVERTEXATTRIBI4UBVPROC glVertexAttribI4ubv;
PFNGLVERTEXATTRIBI4UIPROC glVertexAttribI4ui;
PFNGLVERTEXATTRIBI4UIVPROC glVertexAttribI4uiv;
PFNGLVERTEXATTRIBI4USVPROC glVertexAttribI4usv;
PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer;
PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
PFNGLFRAMEBUFFERTEXTURE1DPROC glFramebufferTexture1D;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
PFNGLFRAMEBUFFERTEXTURE3DPROC glFramebufferTexture3D;
PFNGLFRAMEBUFFERTEXTURELAYERPROC glFramebufferTextureLayer;
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv;
PFNGLGETRENDERBUFFERPARAMETERIVPROC glGetRenderbufferParameteriv;
PFNGLISFRAMEBUFFERPROC glIsFramebuffer;
PFNGLISRENDERBUFFERPROC glIsRenderbuffer;
PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC glRenderbufferStorageMultisample;
PFNGLFLUSHMAPPEDBUFFERRANGEPROC glFlushMappedBufferRange;
PFNGLMAPBUFFERRANGEPROC glMapBufferRange;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLISVERTEXARRAYPROC glIsVertexArray;
/* ----------------------------- GL_VERSION_3_1 ---------------------------- \*/
PFNGLDRAWARRAYSINSTANCEDPROC glDrawArraysInstanced;
PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced;
PFNGLPRIMITIVERESTARTINDEXPROC glPrimitiveRestartIndex;
PFNGLTEXBUFFERPROC glTexBuffer;
PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC glGetActiveUniformBlockName;
PFNGLGETACTIVEUNIFORMBLOCKIVPROC glGetActiveUniformBlockiv;
PFNGLGETACTIVEUNIFORMNAMEPROC glGetActiveUniformName;
PFNGLGETACTIVEUNIFORMSIVPROC glGetActiveUniformsiv;
PFNGLGETUNIFORMBLOCKINDEXPROC glGetUniformBlockIndex;
PFNGLGETUNIFORMINDICESPROC glGetUniformIndices;
PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding;
PFNGLCOPYBUFFERSUBDATAPROC glCopyBufferSubData;
/* ----------------------------- GL_VERSION_3_2 ---------------------------- \*/
PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture;
PFNGLGETBUFFERPARAMETERI64VPROC glGetBufferParameteri64v;
PFNGLGETINTEGER64I_VPROC glGetInteger64i_v;
PFNGLDRAWELEMENTSBASEVERTEXPROC glDrawElementsBaseVertex;
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC glDrawElementsInstancedBaseVertex;
PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC glDrawRangeElementsBaseVertex;
PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC glMultiDrawElementsBaseVertex;
PFNGLPROVOKINGVERTEXPROC glProvokingVertex;
PFNGLCLIENTWAITSYNCPROC glClientWaitSync;
PFNGLDELETESYNCPROC glDeleteSync;
PFNGLFENCESYNCPROC glFenceSync;
PFNGLGETINTEGER64VPROC glGetInteger64v;
PFNGLGETSYNCIVPROC glGetSynciv;
PFNGLISSYNCPROC glIsSync;
PFNGLWAITSYNCPROC glWaitSync;
PFNGLGETMULTISAMPLEFVPROC glGetMultisamplefv;
PFNGLSAMPLEMASKIPROC glSampleMaski;
PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample;
PFNGLTEXIMAGE3DMULTISAMPLEPROC glTexImage3DMultisample;
/* ----------------------------- GL_VERSION_3_3 ---------------------------- \*/
PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisor;
PFNGLBINDFRAGDATALOCATIONINDEXEDPROC glBindFragDataLocationIndexed;
PFNGLGETFRAGDATAINDEXPROC glGetFragDataIndex;
PFNGLBINDSAMPLERPROC glBindSampler;
PFNGLDELETESAMPLERSPROC glDeleteSamplers;
PFNGLGENSAMPLERSPROC glGenSamplers;
PFNGLGETSAMPLERPARAMETERIIVPROC glGetSamplerParameterIiv;
PFNGLGETSAMPLERPARAMETERIUIVPROC glGetSamplerParameterIuiv;
PFNGLGETSAMPLERPARAMETERFVPROC glGetSamplerParameterfv;
PFNGLGETSAMPLERPARAMETERIVPROC glGetSamplerParameteriv;
PFNGLISSAMPLERPROC glIsSampler;
PFNGLSAMPLERPARAMETERIIVPROC glSamplerParameterIiv;
PFNGLSAMPLERPARAMETERIUIVPROC glSamplerParameterIuiv;
PFNGLSAMPLERPARAMETERFPROC glSamplerParameterf;
PFNGLSAMPLERPARAMETERFVPROC glSamplerParameterfv;
PFNGLSAMPLERPARAMETERIPROC glSamplerParameteri;
PFNGLSAMPLERPARAMETERIVPROC glSamplerParameteriv;
PFNGLGETQUERYOBJECTI64VPROC glGetQueryObjecti64v;
PFNGLGETQUERYOBJECTUI64VPROC glGetQueryObjectui64v;
PFNGLQUERYCOUNTERPROC glQueryCounter;
PFNGLVERTEXATTRIBP1UIPROC glVertexAttribP1ui;
PFNGLVERTEXATTRIBP1UIVPROC glVertexAttribP1uiv;
PFNGLVERTEXATTRIBP2UIPROC glVertexAttribP2ui;
PFNGLVERTEXATTRIBP2UIVPROC glVertexAttribP2uiv;
PFNGLVERTEXATTRIBP3UIPROC glVertexAttribP3ui;
PFNGLVERTEXATTRIBP3UIVPROC glVertexAttribP3uiv;
PFNGLVERTEXATTRIBP4UIPROC glVertexAttribP4ui;
PFNGLVERTEXATTRIBP4UIVPROC glVertexAttribP4uiv;
/* ----------------------------- GL_VERSION_4_0 ---------------------------- \*/
PFNGLBLENDEQUATIONSEPARATEIPROC glBlendEquationSeparatei;
PFNGLBLENDEQUATIONIPROC glBlendEquationi;
PFNGLBLENDFUNCSEPARATEIPROC glBlendFuncSeparatei;
PFNGLBLENDFUNCIPROC glBlendFunci;
PFNGLMINSAMPLESHADINGPROC glMinSampleShading;
PFNGLDRAWARRAYSINDIRECTPROC glDrawArraysIndirect;
PFNGLDRAWELEMENTSINDIRECTPROC glDrawElementsIndirect;
PFNGLGETUNIFORMDVPROC glGetUniformdv;
PFNGLUNIFORM1DPROC glUniform1d;
PFNGLUNIFORM1DVPROC glUniform1dv;
PFNGLUNIFORM2DPROC glUniform2d;
PFNGLUNIFORM2DVPROC glUniform2dv;
PFNGLUNIFORM3DPROC glUniform3d;
PFNGLUNIFORM3DVPROC glUniform3dv;
PFNGLUNIFORM4DPROC glUniform4d;
PFNGLUNIFORM4DVPROC glUniform4dv;
PFNGLUNIFORMMATRIX2DVPROC glUniformMatrix2dv;
PFNGLUNIFORMMATRIX2X3DVPROC glUniformMatrix2x3dv;
PFNGLUNIFORMMATRIX2X4DVPROC glUniformMatrix2x4dv;
PFNGLUNIFORMMATRIX3DVPROC glUniformMatrix3dv;
PFNGLUNIFORMMATRIX3X2DVPROC glUniformMatrix3x2dv;
PFNGLUNIFORMMATRIX3X4DVPROC glUniformMatrix3x4dv;
PFNGLUNIFORMMATRIX4DVPROC glUniformMatrix4dv;
PFNGLUNIFORMMATRIX4X2DVPROC glUniformMatrix4x2dv;
PFNGLUNIFORMMATRIX4X3DVPROC glUniformMatrix4x3dv;
PFNGLGETACTIVESUBROUTINENAMEPROC glGetActiveSubroutineName;
PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC glGetActiveSubroutineUniformName;
PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC glGetActiveSubroutineUniformiv;
PFNGLGETPROGRAMSTAGEIVPROC glGetProgramStageiv;
PFNGLGETSUBROUTINEINDEXPROC glGetSubroutineIndex;
PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC glGetSubroutineUniformLocation;
PFNGLGETUNIFORMSUBROUTINEUIVPROC glGetUniformSubroutineuiv;
PFNGLUNIFORMSUBROUTINESUIVPROC glUniformSubroutinesuiv;
PFNGLPATCHPARAMETERFVPROC glPatchParameterfv;
PFNGLPATCHPARAMETERIPROC glPatchParameteri;
PFNGLBINDTRANSFORMFEEDBACKPROC glBindTransformFeedback;
PFNGLDELETETRANSFORMFEEDBACKSPROC glDeleteTransformFeedbacks;
PFNGLDRAWTRANSFORMFEEDBACKPROC glDrawTransformFeedback;
PFNGLGENTRANSFORMFEEDBACKSPROC glGenTransformFeedbacks;
PFNGLISTRANSFORMFEEDBACKPROC glIsTransformFeedback;
PFNGLPAUSETRANSFORMFEEDBACKPROC glPauseTransformFeedback;
PFNGLRESUMETRANSFORMFEEDBACKPROC glResumeTransformFeedback;
PFNGLBEGINQUERYINDEXEDPROC glBeginQueryIndexed;
PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC glDrawTransformFeedbackStream;
PFNGLENDQUERYINDEXEDPROC glEndQueryIndexed;
PFNGLGETQUERYINDEXEDIVPROC glGetQueryIndexediv;
/* ----------------------------- GL_VERSION_4_1 ---------------------------- */
PFNGLCLEARDEPTHFPROC glClearDepthf;
PFNGLDEPTHRANGEFPROC glDepthRangef;
PFNGLRELEASESHADERCOMPILERPROC glReleaseShaderCompiler;
PFNGLGETSHADERPRECISIONFORMATPROC glGetShaderPrecisionFormat;
PFNGLSHADERBINARYPROC glShaderBinary;
PFNGLGETPROGRAMBINARYPROC glGetProgramBinary;
PFNGLPROGRAMBINARYPROC glProgramBinary;
PFNGLPROGRAMPARAMETERIPROC glProgramParameteri;
PFNGLACTIVESHADERPROGRAMPROC glActiveShaderProgram;
PFNGLBINDPROGRAMPIPELINEPROC glBindProgramPipeline;
PFNGLCREATESHADERPROGRAMVPROC glCreateShaderProgramv;
PFNGLDELETEPROGRAMPIPELINESPROC glDeleteProgramPipelines;
PFNGLGENPROGRAMPIPELINESPROC glGenProgramPipelines;
PFNGLGETPROGRAMPIPELINEINFOLOGPROC glGetProgramPipelineInfoLog;
PFNGLGETPROGRAMPIPELINEIVPROC glGetProgramPipelineiv;
PFNGLISPROGRAMPIPELINEPROC glIsProgramPipeline;
PFNGLPROGRAMUNIFORM1DPROC glProgramUniform1d;
PFNGLPROGRAMUNIFORM1DVPROC glProgramUniform1dv;
PFNGLPROGRAMUNIFORM1FPROC glProgramUniform1f;
PFNGLPROGRAMUNIFORM1FVPROC glProgramUniform1fv;
PFNGLPROGRAMUNIFORM1IPROC glProgramUniform1i;
PFNGLPROGRAMUNIFORM1IVPROC glProgramUniform1iv;
PFNGLPROGRAMUNIFORM1UIPROC glProgramUniform1ui;
PFNGLPROGRAMUNIFORM1UIVPROC glProgramUniform1uiv;
PFNGLPROGRAMUNIFORM2DPROC glProgramUniform2d;
PFNGLPROGRAMUNIFORM2DVPROC glProgramUniform2dv;
PFNGLPROGRAMUNIFORM2FPROC glProgramUniform2f;
PFNGLPROGRAMUNIFORM2FVPROC glProgramUniform2fv;
PFNGLPROGRAMUNIFORM2IPROC glProgramUniform2i;
PFNGLPROGRAMUNIFORM2IVPROC glProgramUniform2iv;
PFNGLPROGRAMUNIFORM2UIPROC glProgramUniform2ui;
PFNGLPROGRAMUNIFORM2UIVPROC glProgramUniform2uiv;
PFNGLPROGRAMUNIFORM3DPROC glProgramUniform3d;
PFNGLPROGRAMUNIFORM3DVPROC glProgramUniform3dv;
PFNGLPROGRAMUNIFORM3FPROC glProgramUniform3f;
PFNGLPROGRAMUNIFORM3FVPROC glProgramUniform3fv;
PFNGLPROGRAMUNIFORM3IPROC glProgramUniform3i;
PFNGLPROGRAMUNIFORM3IVPROC glProgramUniform3iv;
PFNGLPROGRAMUNIFORM3UIPROC glProgramUniform3ui;
PFNGLPROGRAMUNIFORM3UIVPROC glProgramUniform3uiv;
PFNGLPROGRAMUNIFORM4DPROC glProgramUniform4d;
PFNGLPROGRAMUNIFORM4DVPROC glProgramUniform4dv;
PFNGLPROGRAMUNIFORM4FPROC glProgramUniform4f;
PFNGLPROGRAMUNIFORM4FVPROC glProgramUniform4fv;
PFNGLPROGRAMUNIFORM4IPROC glProgramUniform4i;
PFNGLPROGRAMUNIFORM4IVPROC glProgramUniform4iv;
PFNGLPROGRAMUNIFORM4UIPROC glProgramUniform4ui;
PFNGLPROGRAMUNIFORM4UIVPROC glProgramUniform4uiv;
PFNGLPROGRAMUNIFORMMATRIX2DVPROC glProgramUniformMatrix2dv;
PFNGLPROGRAMUNIFORMMATRIX2FVPROC glProgramUniformMatrix2fv;
PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC glProgramUniformMatrix2x3dv;
PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC glProgramUniformMatrix2x3fv;
PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC glProgramUniformMatrix2x4dv;
PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC glProgramUniformMatrix2x4fv;
PFNGLPROGRAMUNIFORMMATRIX3DVPROC glProgramUniformMatrix3dv;
PFNGLPROGRAMUNIFORMMATRIX3FVPROC glProgramUniformMatrix3fv;
PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC glProgramUniformMatrix3x2dv;
PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC glProgramUniformMatrix3x2fv;
PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC glProgramUniformMatrix3x4dv;
PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC glProgramUniformMatrix3x4fv;
PFNGLPROGRAMUNIFORMMATRIX4DVPROC glProgramUniformMatrix4dv;
PFNGLPROGRAMUNIFORMMATRIX4FVPROC glProgramUniformMatrix4fv;
PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC glProgramUniformMatrix4x2dv;
PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC glProgramUniformMatrix4x2fv;
PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC glProgramUniformMatrix4x3dv;
PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC glProgramUniformMatrix4x3fv;
PFNGLUSEPROGRAMSTAGESPROC glUseProgramStages;
PFNGLVALIDATEPROGRAMPIPELINEPROC glValidateProgramPipeline;
PFNGLGETVERTEXATTRIBLDVPROC glGetVertexAttribLdv;
PFNGLVERTEXATTRIBL1DPROC glVertexAttribL1d;
PFNGLVERTEXATTRIBL1DVPROC glVertexAttribL1dv;
PFNGLVERTEXATTRIBL2DPROC glVertexAttribL2d;
PFNGLVERTEXATTRIBL2DVPROC glVertexAttribL2dv;
PFNGLVERTEXATTRIBL3DPROC glVertexAttribL3d;
PFNGLVERTEXATTRIBL3DVPROC glVertexAttribL3dv;
PFNGLVERTEXATTRIBL4DPROC glVertexAttribL4d;
PFNGLVERTEXATTRIBL4DVPROC glVertexAttribL4dv;
PFNGLVERTEXATTRIBLPOINTERPROC glVertexAttribLPointer;
PFNGLDEPTHRANGEARRAYVPROC glDepthRangeArrayv;
PFNGLDEPTHRANGEINDEXEDPROC glDepthRangeIndexed;
PFNGLGETDOUBLEI_VPROC glGetDoublei_v;
PFNGLGETFLOATI_VPROC glGetFloati_v;
PFNGLSCISSORARRAYVPROC glScissorArrayv;
PFNGLSCISSORINDEXEDPROC glScissorIndexed;
PFNGLSCISSORINDEXEDVPROC glScissorIndexedv;
PFNGLVIEWPORTARRAYVPROC glViewportArrayv;
PFNGLVIEWPORTINDEXEDFPROC glViewportIndexedf;
PFNGLVIEWPORTINDEXEDFVPROC glViewportIndexedfv;
/* ----------------------------- GL_VERSION_4_2 ---------------------------- */
PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC glDrawArraysInstancedBaseInstance;
PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC glDrawElementsInstancedBaseInstance;
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC glDrawElementsInstancedBaseVertexBaseInstance;
PFNGLGETINTERNALFORMATIVPROC glGetInternalformativ;
PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC glGetActiveAtomicCounterBufferiv;
PFNGLBINDIMAGETEXTUREPROC glBindImageTexture;
PFNGLMEMORYBARRIERPROC glMemoryBarrier;
PFNGLTEXSTORAGE1DPROC glTexStorage1D;
PFNGLTEXSTORAGE2DPROC glTexStorage2D;
PFNGLTEXSTORAGE3DPROC glTexStorage3D;
PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC glDrawTransformFeedbackInstanced;
PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC glDrawTransformFeedbackStreamInstanced;
/* ----------------------------- GL_VERSION_4_3 ---------------------------- */
PFNGLCLEARBUFFERDATAPROC glClearBufferData;
PFNGLCLEARBUFFERSUBDATAPROC glClearBufferSubData;
PFNGLCLEARNAMEDBUFFERDATAEXTPROC glClearNamedBufferDataEXT;
PFNGLCLEARNAMEDBUFFERSUBDATAEXTPROC glClearNamedBufferSubDataEXT;
PFNGLDISPATCHCOMPUTEPROC glDispatchCompute;
PFNGLDISPATCHCOMPUTEINDIRECTPROC glDispatchComputeIndirect;
PFNGLCOPYIMAGESUBDATAPROC glCopyImageSubData;
PFNGLFRAMEBUFFERPARAMETERIPROC glFramebufferParameteri;
PFNGLGETFRAMEBUFFERPARAMETERIVPROC glGetFramebufferParameteriv;
PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVEXTPROC glGetNamedFramebufferParameterivEXT;
PFNGLNAMEDFRAMEBUFFERPARAMETERIEXTPROC glNamedFramebufferParameteriEXT;
PFNGLGETINTERNALFORMATI64VPROC glGetInternalformati64v;
PFNGLINVALIDATEBUFFERDATAPROC glInvalidateBufferData;
PFNGLINVALIDATEBUFFERSUBDATAPROC glInvalidateBufferSubData;
PFNGLINVALIDATEFRAMEBUFFERPROC glInvalidateFramebuffer;
PFNGLINVALIDATESUBFRAMEBUFFERPROC glInvalidateSubFramebuffer;
PFNGLINVALIDATETEXIMAGEPROC glInvalidateTexImage;
PFNGLINVALIDATETEXSUBIMAGEPROC glInvalidateTexSubImage;
PFNGLMULTIDRAWARRAYSINDIRECTPROC glMultiDrawArraysIndirect;
PFNGLMULTIDRAWELEMENTSINDIRECTPROC glMultiDrawElementsIndirect;
PFNGLGETPROGRAMINTERFACEIVPROC glGetProgramInterfaceiv;
PFNGLGETPROGRAMRESOURCEINDEXPROC glGetProgramResourceIndex;
PFNGLGETPROGRAMRESOURCELOCATIONPROC glGetProgramResourceLocation;
PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC glGetProgramResourceLocationIndex;
PFNGLGETPROGRAMRESOURCENAMEPROC glGetProgramResourceName;
PFNGLGETPROGRAMRESOURCEIVPROC glGetProgramResourceiv;
PFNGLSHADERSTORAGEBLOCKBINDINGPROC glShaderStorageBlockBinding;
PFNGLTEXBUFFERRANGEPROC glTexBufferRange;
PFNGLTEXTUREBUFFERRANGEEXTPROC glTextureBufferRangeEXT;
PFNGLTEXSTORAGE2DMULTISAMPLEPROC glTexStorage2DMultisample;
PFNGLTEXSTORAGE3DMULTISAMPLEPROC glTexStorage3DMultisample;
PFNGLTEXTURESTORAGE2DMULTISAMPLEEXTPROC glTextureStorage2DMultisampleEXT;
PFNGLTEXTURESTORAGE3DMULTISAMPLEEXTPROC glTextureStorage3DMultisampleEXT;
PFNGLTEXTUREVIEWPROC glTextureView;
PFNGLBINDVERTEXBUFFERPROC glBindVertexBuffer;
PFNGLVERTEXARRAYBINDVERTEXBUFFEREXTPROC glVertexArrayBindVertexBufferEXT;
PFNGLVERTEXARRAYVERTEXATTRIBBINDINGEXTPROC glVertexArrayVertexAttribBindingEXT;
PFNGLVERTEXARRAYVERTEXATTRIBFORMATEXTPROC glVertexArrayVertexAttribFormatEXT;
PFNGLVERTEXARRAYVERTEXATTRIBIFORMATEXTPROC glVertexArrayVertexAttribIFormatEXT;
PFNGLVERTEXARRAYVERTEXATTRIBLFORMATEXTPROC glVertexArrayVertexAttribLFormatEXT;
PFNGLVERTEXARRAYVERTEXBINDINGDIVISOREXTPROC glVertexArrayVertexBindingDivisorEXT;
PFNGLVERTEXATTRIBBINDINGPROC glVertexAttribBinding;
PFNGLVERTEXATTRIBFORMATPROC glVertexAttribFormat;
PFNGLVERTEXATTRIBIFORMATPROC glVertexAttribIFormat;
PFNGLVERTEXATTRIBLFORMATPROC glVertexAttribLFormat;
PFNGLVERTEXBINDINGDIVISORPROC glVertexBindingDivisor;
PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback;
PFNGLDEBUGMESSAGECONTROLPROC glDebugMessageControl;
PFNGLDEBUGMESSAGEINSERTPROC glDebugMessageInsert;
PFNGLGETDEBUGMESSAGELOGPROC glGetDebugMessageLog;
PFNGLGETOBJECTLABELPROC glGetObjectLabel;
PFNGLGETOBJECTPTRLABELPROC glGetObjectPtrLabel;
PFNGLOBJECTLABELPROC glObjectLabel;
PFNGLOBJECTPTRLABELPROC glObjectPtrLabel;
PFNGLPOPDEBUGGROUPPROC glPopDebugGroup;
PFNGLPUSHDEBUGGROUPPROC glPushDebugGroup;
/* ----------------------------- GL_VERSION_4_4 ---------------------------- */
PFNGLBUFFERSTORAGEPROC glBufferStorage;
PFNGLCLEARTEXIMAGEPROC glClearTexImage;
PFNGLCLEARTEXSUBIMAGEPROC glClearTexSubImage;
PFNGLBINDBUFFERSBASEPROC glBindBuffersBase;
PFNGLBINDBUFFERSRANGEPROC glBindBuffersRange;
PFNGLBINDIMAGETEXTURESPROC glBindImageTextures;
PFNGLBINDSAMPLERSPROC glBindSamplers;
PFNGLBINDTEXTURESPROC glBindTextures;
PFNGLBINDVERTEXBUFFERSPROC glBindVertexBuffers;
/* ----------------------------- GL_VERSION_4_5 ---------------------------- */
PFNGLGETGRAPHICSRESETSTATUSPROC glGetGraphicsResetStatus;
PFNGLGETNCOMPRESSEDTEXIMAGEPROC glGetnCompressedTexImage;
PFNGLGETNTEXIMAGEPROC glGetnTexImage;
PFNGLGETNUNIFORMDVPROC glGetnUniformdv;
PFNGLCLIPCONTROLPROC glClipControl;
PFNGLBINDTEXTUREUNITPROC glBindTextureUnit;
PFNGLBLITNAMEDFRAMEBUFFERPROC glBlitNamedFramebuffer;
PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC glCheckNamedFramebufferStatus;
PFNGLCLEARNAMEDBUFFERDATAPROC glClearNamedBufferData;
PFNGLCLEARNAMEDBUFFERSUBDATAPROC glClearNamedBufferSubData;
PFNGLCLEARNAMEDFRAMEBUFFERFIPROC glClearNamedFramebufferfi;
PFNGLCLEARNAMEDFRAMEBUFFERFVPROC glClearNamedFramebufferfv;
PFNGLCLEARNAMEDFRAMEBUFFERIVPROC glClearNamedFramebufferiv;
PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC glClearNamedFramebufferuiv;
PFNGLCOMPRESSEDTEXTURESUBIMAGE1DPROC glCompressedTextureSubImage1D;
PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC glCompressedTextureSubImage2D;
PFNGLCOMPRESSEDTEXTURESUBIMAGE3DPROC glCompressedTextureSubImage3D;
PFNGLCOPYNAMEDBUFFERSUBDATAPROC glCopyNamedBufferSubData;
PFNGLCOPYTEXTURESUBIMAGE1DPROC glCopyTextureSubImage1D;
PFNGLCOPYTEXTURESUBIMAGE2DPROC glCopyTextureSubImage2D;
PFNGLCOPYTEXTURESUBIMAGE3DPROC glCopyTextureSubImage3D;
PFNGLCREATEBUFFERSPROC glCreateBuffers;
PFNGLCREATEFRAMEBUFFERSPROC glCreateFramebuffers;
PFNGLCREATEPROGRAMPIPELINESPROC glCreateProgramPipelines;
PFNGLCREATEQUERIESPROC glCreateQueries;
PFNGLCREATERENDERBUFFERSPROC glCreateRenderbuffers;
PFNGLCREATESAMPLERSPROC glCreateSamplers;
PFNGLCREATETEXTURESPROC glCreateTextures;
PFNGLCREATETRANSFORMFEEDBACKSPROC glCreateTransformFeedbacks;
PFNGLCREATEVERTEXARRAYSPROC glCreateVertexArrays;
PFNGLDISABLEVERTEXARRAYATTRIBPROC glDisableVertexArrayAttrib;
PFNGLENABLEVERTEXARRAYATTRIBPROC glEnableVertexArrayAttrib;
PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC glFlushMappedNamedBufferRange;
PFNGLGENERATETEXTUREMIPMAPPROC glGenerateTextureMipmap;
PFNGLGETCOMPRESSEDTEXTUREIMAGEPROC glGetCompressedTextureImage;
PFNGLGETNAMEDBUFFERPARAMETERI64VPROC glGetNamedBufferParameteri64v;
PFNGLGETNAMEDBUFFERPARAMETERIVPROC glGetNamedBufferParameteriv;
PFNGLGETNAMEDBUFFERPOINTERVPROC glGetNamedBufferPointerv;
PFNGLGETNAMEDBUFFERSUBDATAPROC glGetNamedBufferSubData;
PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetNamedFramebufferAttachmentParameteriv;
PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVPROC glGetNamedFramebufferParameteriv;
PFNGLGETNAMEDRENDERBUFFERPARAMETERIVPROC glGetNamedRenderbufferParameteriv;
PFNGLGETQUERYBUFFEROBJECTI64VPROC glGetQueryBufferObjecti64v;
PFNGLGETQUERYBUFFEROBJECTIVPROC glGetQueryBufferObjectiv;
PFNGLGETQUERYBUFFEROBJECTUI64VPROC glGetQueryBufferObjectui64v;
PFNGLGETQUERYBUFFEROBJECTUIVPROC glGetQueryBufferObjectuiv;
PFNGLGETTEXTUREIMAGEPROC glGetTextureImage;
PFNGLGETTEXTURELEVELPARAMETERFVPROC glGetTextureLevelParameterfv;
PFNGLGETTEXTURELEVELPARAMETERIVPROC glGetTextureLevelParameteriv;
PFNGLGETTEXTUREPARAMETERIIVPROC glGetTextureParameterIiv;
PFNGLGETTEXTUREPARAMETERIUIVPROC glGetTextureParameterIuiv;
PFNGLGETTEXTUREPARAMETERFVPROC glGetTextureParameterfv;
PFNGLGETTEXTUREPARAMETERIVPROC glGetTextureParameteriv;
PFNGLGETTRANSFORMFEEDBACKI64_VPROC glGetTransformFeedbacki64_v;
PFNGLGETTRANSFORMFEEDBACKI_VPROC glGetTransformFeedbacki_v;
PFNGLGETTRANSFORMFEEDBACKIVPROC glGetTransformFeedbackiv;
PFNGLGETVERTEXARRAYINDEXED64IVPROC glGetVertexArrayIndexed64iv;
PFNGLGETVERTEXARRAYINDEXEDIVPROC glGetVertexArrayIndexediv;
PFNGLGETVERTEXARRAYIVPROC glGetVertexArrayiv;
PFNGLINVALIDATENAMEDFRAMEBUFFERDATAPROC glInvalidateNamedFramebufferData;
PFNGLINVALIDATENAMEDFRAMEBUFFERSUBDATAPROC glInvalidateNamedFramebufferSubData;
PFNGLMAPNAMEDBUFFERPROC glMapNamedBuffer;
PFNGLMAPNAMEDBUFFERRANGEPROC glMapNamedBufferRange;
PFNGLNAMEDBUFFERDATAPROC glNamedBufferData;
PFNGLNAMEDBUFFERSTORAGEPROC glNamedBufferStorage;
PFNGLNAMEDBUFFERSUBDATAPROC glNamedBufferSubData;
PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC glNamedFramebufferDrawBuffer;
PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC glNamedFramebufferDrawBuffers;
PFNGLNAMEDFRAMEBUFFERPARAMETERIPROC glNamedFramebufferParameteri;
PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC glNamedFramebufferReadBuffer;
PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC glNamedFramebufferRenderbuffer;
PFNGLNAMEDFRAMEBUFFERTEXTUREPROC glNamedFramebufferTexture;
PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC glNamedFramebufferTextureLayer;
PFNGLNAMEDRENDERBUFFERSTORAGEPROC glNamedRenderbufferStorage;
PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEPROC glNamedRenderbufferStorageMultisample;
PFNGLTEXTUREBUFFERPROC glTextureBuffer;
PFNGLTEXTUREBUFFERRANGEPROC glTextureBufferRange;
PFNGLTEXTUREPARAMETERIIVPROC glTextureParameterIiv;
PFNGLTEXTUREPARAMETERIUIVPROC glTextureParameterIuiv;
PFNGLTEXTUREPARAMETERFPROC glTextureParameterf;
PFNGLTEXTUREPARAMETERFVPROC glTextureParameterfv;
PFNGLTEXTUREPARAMETERIPROC glTextureParameteri;
PFNGLTEXTUREPARAMETERIVPROC glTextureParameteriv;
PFNGLTEXTURESTORAGE1DPROC glTextureStorage1D;
PFNGLTEXTURESTORAGE2DPROC glTextureStorage2D;
PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC glTextureStorage2DMultisample;
PFNGLTEXTURESTORAGE3DPROC glTextureStorage3D;
PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC glTextureStorage3DMultisample;
PFNGLTEXTURESUBIMAGE1DPROC glTextureSubImage1D;
PFNGLTEXTURESUBIMAGE2DPROC glTextureSubImage2D;
PFNGLTEXTURESUBIMAGE3DPROC glTextureSubImage3D;
PFNGLTRANSFORMFEEDBACKBUFFERBASEPROC glTransformFeedbackBufferBase;
PFNGLTRANSFORMFEEDBACKBUFFERRANGEPROC glTransformFeedbackBufferRange;
PFNGLUNMAPNAMEDBUFFERPROC glUnmapNamedBuffer;
PFNGLVERTEXARRAYATTRIBBINDINGPROC glVertexArrayAttribBinding;
PFNGLVERTEXARRAYATTRIBFORMATPROC glVertexArrayAttribFormat;
PFNGLVERTEXARRAYATTRIBIFORMATPROC glVertexArrayAttribIFormat;
PFNGLVERTEXARRAYATTRIBLFORMATPROC glVertexArrayAttribLFormat;
PFNGLVERTEXARRAYBINDINGDIVISORPROC glVertexArrayBindingDivisor;
PFNGLVERTEXARRAYELEMENTBUFFERPROC glVertexArrayElementBuffer;
PFNGLVERTEXARRAYVERTEXBUFFERPROC glVertexArrayVertexBuffer;
PFNGLVERTEXARRAYVERTEXBUFFERSPROC glVertexArrayVertexBuffers;
PFNGLGETNUNIFORMFVPROC glGetnUniformfv;
PFNGLGETNUNIFORMIVPROC glGetnUniformiv;
PFNGLGETNUNIFORMUIVPROC glGetnUniformuiv;
PFNGLREADNPIXELSPROC glReadnPixels;
PFNGLTEXTUREBARRIERPROC glTextureBarrier;
PFNGLMEMORYBARRIERBYREGIONPROC glMemoryBarrierByRegion;
PFNGLGETCOMPRESSEDTEXTURESUBIMAGEPROC glGetCompressedTextureSubImage;
PFNGLGETTEXTURESUBIMAGEPROC glGetTextureSubImage;
/* ----------------------------- GL_VERSION_4_6 ---------------------------- */
PFNGLMULTIDRAWARRAYSINDIRECTCOUNTPROC glMultiDrawArraysIndirectCount;
PFNGLMULTIDRAWELEMENTSINDIRECTCOUNTPROC glMultiDrawElementsIndirectCount;
PFNGLSPECIALIZESHADERPROC glSpecializeShader;
PFNGLPOLYGONOFFSETCLAMPPROC glPolygonOffsetClamp;
PFNGLPRIMITIVEBOUNDINGBOXARBPROC glPrimitiveBoundingBoxARB;