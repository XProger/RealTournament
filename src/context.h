#ifndef H_CONTEXT
#define H_CONTEXT

#include "utils.h"

#include <gl/GL.h>
#include <gl/glext.h>

// textures
PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
// buffers
PFNGLGENBUFFERSPROC     glGenBuffers;
PFNGLDELETEBUFFERSPROC  glDeleteBuffers;
PFNGLBINDBUFFERPROC     glBindBuffer;
PFNGLBUFFERDATAPROC     glBufferData;
PFNGLBUFFERSUBDATAPROC  glBufferSubData;
// shaders
PFNGLCREATEPROGRAMPROC              glCreateProgram;
PFNGLCREATESHADERPROC               glCreateShader;
PFNGLSHADERSOURCEPROC               glShaderSource;
PFNGLATTACHSHADERPROC               glAttachShader;
PFNGLDELETESHADERPROC               glDeleteShader;
PFNGLDELETEPROGRAMPROC              glDeleteProgram;
PFNGLLINKPROGRAMPROC                glLinkProgram;
PFNGLCOMPILESHADERPROC              glCompileShader;
PFNGLGETSHADERINFOLOGPROC           glGetShaderInfoLog;
PFNGLGETPROGRAMINFOLOGPROC          glGetProgramInfoLog;
PFNGLUSEPROGRAMPROC                 glUseProgram;
PFNGLENABLEVERTEXATTRIBARRAYPROC    glEnableVertexAttribArray;
PFNGLDISABLEVERTEXATTRIBARRAYPROC   glDisableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC        glVertexAttribPointer;
PFNGLBINDATTRIBLOCATIONPROC         glBindAttribLocation;
PFNGLGETUNIFORMLOCATIONPROC         glGetUniformLocation;
PFNGLUNIFORM1IVPROC                 glUniform1iv;
PFNGLUNIFORM4FVPROC                 glUniform4fv;
PFNGLUNIFORMMATRIX4FVPROC           glUniformMatrix4fv;

typedef uint16 Index;

struct Vertex {
    float r, g, b;
    float s, t;
    float x, y;
};

namespace Context {
    int width, height;

    void* GetProc(const char *name) {
        return (void*)wglGetProcAddress(name);
    }

    #define GetProcGL(x) x = (decltype(x))GetProc(#x);

    void init() {
        LOG("Vendor   : %s\n", glGetString(GL_VENDOR));
        LOG("Renderer : %s\n", glGetString(GL_RENDERER));
        LOG("Version  : %s\n", glGetString(GL_VERSION));

        GetProcGL(glGenerateMipmap);

        GetProcGL(glGenBuffers);
        GetProcGL(glDeleteBuffers);
        GetProcGL(glBindBuffer);
        GetProcGL(glBufferData);
        GetProcGL(glBufferSubData);

        GetProcGL(glCreateProgram);
        GetProcGL(glCreateShader);
        GetProcGL(glShaderSource);
        GetProcGL(glAttachShader);
        GetProcGL(glDeleteShader);
        GetProcGL(glDeleteProgram);
        GetProcGL(glLinkProgram);
        GetProcGL(glCompileShader);
        GetProcGL(glGetShaderInfoLog);
        GetProcGL(glGetProgramInfoLog);
        GetProcGL(glUseProgram);
        GetProcGL(glEnableVertexAttribArray);
        GetProcGL(glDisableVertexAttribArray);
        GetProcGL(glVertexAttribPointer);
        GetProcGL(glBindAttribLocation);
        GetProcGL(glGetUniformLocation);
        GetProcGL(glUniform1iv);
        GetProcGL(glUniform4fv);
        GetProcGL(glUniformMatrix4fv);
    }

    void deinit() {
    
    }
}

#endif