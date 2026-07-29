#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "EGL/egl.h"
#include "glcorearb.h"

const EGLint config_attribs[] =
{
    EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
    EGL_NONE
};
const EGLint pbuffer_attribs[] =
{
    EGL_WIDTH, 1,
    EGL_HEIGHT, 1,
    EGL_NONE
};
const EGLint context_attribs[] =
{
    EGL_CONTEXT_MAJOR_VERSION, 3,
    EGL_CONTEXT_MINOR_VERSION, 3,
    EGL_NONE
};

void computation();

int main(void)
{
    srand(time(NULL));

    if(eglwInitialize()){
        fprintf(stderr, "eglwInitialize failed\n");return 1;
    }

    EGLDisplay display = eglGetPlatformDisplayEXT(EGL_PLATFORM_SURFACELESS_MESA, EGL_DEFAULT_DISPLAY, NULL);
    if (display == EGL_NO_DISPLAY){
        fprintf(stderr, "eglGetDisplay failed\n");return 1;
    }

    if (!eglInitialize(display, NULL, NULL)) {
        EGLint err = eglGetError();
        fprintf(stderr, "eglInitialize failed: 0x%04X\n", err);
        return 1;
    }

    eglBindAPI(EGL_OPENGL_API);
    
    EGLConfig config;
    EGLint num_configs;
    eglChooseConfig(display, config_attribs, &config, 1, &num_configs);

    EGLSurface surface = eglCreatePbufferSurface(display, config, pbuffer_attribs);    
    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attribs);
    if (context == EGL_NO_CONTEXT) {
        EGLint err = eglGetError();
        fprintf(stderr, "eglCreateContext failed: 0x%04X\n", err);
        return 1;
    }
    eglMakeCurrent(display, surface, surface, context);

    if(glwInitialize(0x402)) {
        fprintf(stderr, "glwInitialize failed\n");return 1;
    }

    computation();

    glwDestroy();
    eglDestroyContext(display, context);
    eglDestroySurface(display, surface);
    eglTerminate(display);
    eglwDestroy();
    return 0;
}

GLuint compile_shader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success){
        char log[4096];
        glGetShaderInfoLog(shader, sizeof(log), NULL, log);
        fprintf(stderr, "Shader compile error:\n%s\n", log);
        exit(1);
    }
    return shader;
}
GLuint create_program(const char* vertex_source, const char* fragment_source) {
    GLuint vs = compile_shader(GL_VERTEX_SHADER, vertex_source);
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fragment_source);
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success){
        char log[4096];
        glGetProgramInfoLog(program, sizeof(log), NULL, log);
        fprintf(stderr, "Program link error:\n%s\n", log);
        exit(1);
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

const char *fshader = "                    \
#version 330 core\n                        \
uniform samplerBuffer matrixA;             \
uniform samplerBuffer matrixB;             \
layout(location = 0) out float result;     \
void main() {                              \
    ivec2 cord = ivec2(gl_FragCoord.xy);   \
    int index = cord.x * 4 + cord.y;       \
    result = log(dot(texelFetch(matrixA, index),atan(texelFetch(matrixB, index))));\
}";
const char *vshader = "                    \
#version 330 core\n                        \
const vec2 verts[3] = vec2[](              \
    vec2(-2.0, -2.0),                      \
    vec2( 100.0, -2.0),                    \
    vec2(-2.0,  100.0)                     \
);                                         \
void main() {                              \
    gl_Position = vec4(verts[gl_VertexID%3], 0.0, 1.0);\
}";

float randomFloat() {
      return (float)rand() / (float)RAND_MAX;
}

#define GLCHECK {GLenum err = glGetError(); if(err){fprintf(stderr, "Error 0x%04X %s:%u\n", (int)err, __FILE__, __LINE__ );}}


void computation(){
    GLuint buffer[2];
    GLuint tbo[2];
    GLuint resultTex[1];
    GLuint fbo[1];
    GLuint vao[1];
    float* ptr[2];
    GLuint program;
    int dimention = 1024;

    program = create_program(vshader, fshader);
    glGenBuffers(2, buffer);
    glGenTextures(1, resultTex);
    glGenTextures(2, tbo);
    glGenFramebuffers(1, fbo);
    glGenVertexArrays(1, vao);

    glBindBuffer(GL_TEXTURE_BUFFER, buffer[0]);
    glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * 4 * dimention, NULL, GL_DYNAMIC_DRAW);
    ptr[0] = glMapBuffer(GL_TEXTURE_BUFFER, GL_WRITE_ONLY);
    glBindTexture(GL_TEXTURE_BUFFER, tbo[0]);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, buffer[0]);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);

    glBindBuffer(GL_TEXTURE_BUFFER, buffer[1]);
    glBufferData(GL_TEXTURE_BUFFER, sizeof(float) * 4 * dimention, NULL, GL_DYNAMIC_DRAW);
    ptr[1] = glMapBuffer(GL_TEXTURE_BUFFER, GL_WRITE_ONLY);
    glBindTexture(GL_TEXTURE_BUFFER, tbo[1]);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, buffer[1]);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);

    for(uint32_t i = 0;i < 4 * dimention;i++)
        ptr[0][i]=randomFloat();
    for(uint32_t i = 0;i < 4 * dimention;i++)
        ptr[1][i]=randomFloat();

    glBindBuffer(GL_TEXTURE_BUFFER, buffer[0]);
    glUnmapBuffer(GL_TEXTURE_BUFFER);
    glBindBuffer(GL_TEXTURE_BUFFER, buffer[1]);
    glUnmapBuffer(GL_TEXTURE_BUFFER);

    glBindTexture(GL_TEXTURE_2D, resultTex[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, dimention, dimention, 0, GL_RED, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo[0]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, resultTex[0], 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        abort();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    glBindVertexArray(vao[0]);
    //glBindBuffer(GL_ARRAY_BUFFER, vao[0]);
    //glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    //glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    GLCHECK


    glViewport(0, 0, dimention, dimention);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo[0]);
    glUseProgram(program);
    glBindVertexArray(vao[0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_BUFFER, tbo[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_BUFFER, tbo[1]);
    glUniform1i(glGetUniformLocation(program, "matrixA"), 0);
    glUniform1i(glGetUniformLocation(program, "matrixB"), 1);
    glDrawArrays(GL_TRIANGLES, 0, 3*100000);
    GLCHECK
    glFlush();
    if(1){
        dimention = 4;
        float result[dimention*dimention];
        glReadPixels(0, 0, dimention, dimention, GL_RED, GL_FLOAT, result);
        for (int y = 0; y < dimention; y++) {
            for (int x = 0; x < dimention; x++)
                printf("%f ", result[y * dimention + x]);
            printf("\n");
        }
    }

    glDeleteProgram(program);
    glDeleteBuffers(2, buffer);
    glDeleteTextures(1, resultTex);
    glDeleteTextures(2, tbo);
    glDeleteFramebuffers(1, fbo);
    glDeleteVertexArrays(1, vao);
}