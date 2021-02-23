//
//  Created by Pi on 2021/2/18.
//

#include "easygl_utils.hpp"

#ifdef __ANDROID__
void glDummyBindVertexArrayOES (GLuint array) {}
void glDummyDeleteVertexArraysOES (GLsizei n, const GLuint *arrays) {}
void glDummyGenVertexArraysOES (GLsizei n, GLuint *arrays) {}
GLboolean glDummyIsVertexArrayOES (GLuint array) {return false;}

PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOES = &glDummyGenVertexArraysOES;
PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOES = &glDummyBindVertexArrayOES;
PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOES = &glDummyDeleteVertexArraysOES;
PFNGLISVERTEXARRAYOESPROC glIsVertexArrayOES = &glDummyIsVertexArrayOES;

void loadVAOFunctions()
{
#define __GET_PROC_ADDR(_func) \
    _func = (PFNGLGENVERTEXARRAYSOESPROC)eglGetProcAddress(#_func); \
    EASYGL_ASSERT(_func != nullptr, "device not support " #_func);
    
    __GET_PROC_ADDR(glGenVertexArraysOES)
    __GET_PROC_ADDR(glBindVertexArrayOES)
    __GET_PROC_ADDR(glDeleteVertexArraysOES)
    __GET_PROC_ADDR(glIsVertexArrayOES)
    
#undef __GET_PROC_ADDR
}
#endif

NS_EASYGL_BEGIN

bool GetAndCheckGLStatus(GetGLStatusFunc getStatus, GLuint target, GLenum type,
                         GetGLInfoLogFunc getLog, std::string *errMsg)
{
    GLint result = GL_FALSE;
    getStatus(target, type, &result);
    if (result != GL_TRUE)
    {
        char buf[1024];
        GLsizei len = 0;
        getLog(target, sizeof(buf), &len, buf);
        buf[len] = 0;
        if (errMsg) { *errMsg = std::string(buf); }
        return false;
    }
    return true;
}

const char * GetGLErrorString(GLenum error)
{
#define __DEF_GL_ERR_(_code, _msg) \
if (error == _code) { return _msg; }
    
#define __DEF_GL_ERR(_err) __DEF_GL_ERR_(_err, #_err)

    __DEF_GL_ERR(GL_NO_ERROR)
    __DEF_GL_ERR(GL_INVALID_ENUM)
    __DEF_GL_ERR(GL_INVALID_VALUE)
    __DEF_GL_ERR(GL_INVALID_OPERATION)
    __DEF_GL_ERR(GL_OUT_OF_MEMORY)
    __DEF_GL_ERR(GL_INVALID_FRAMEBUFFER_OPERATION)

#ifdef GL_STACK_OVERFLOW
    __DEF_GL_ERR(GL_STACK_OVERFLOW)
#else
    __DEF_GL_ERR_(0x0503, "GL_STACK_OVERFLOW")
#endif
    
#ifdef GL_STACK_UNDERFLOW
    __DEF_GL_ERR(GL_STACK_UNDERFLOW)
#else
    __DEF_GL_ERR_(0x0504, "GL_STACK_UNDERFLOW")
#endif
    
#ifdef GL_CONTEXT_LOST
    __DEF_GL_ERR(GL_CONTEXT_LOST)
#else
    __DEF_GL_ERR_(0x0507, "GL_CONTEXT_LOST")
#endif
    
#ifdef GL_TABLE_TOO_LARGE1
    __DEF_GL_ERR(GL_TABLE_TOO_LARGE1)
#else
    __DEF_GL_ERR_(0x8031, "GL_TABLE_TOO_LARGE1")
#endif

    return "(ERROR: Unknown Error Enum)";
}

NS_EASYGL_END
