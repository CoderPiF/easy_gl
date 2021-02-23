//
//  Created by Pi on 2021/2/9.
//

#ifndef easygl_utils_h
#define easygl_utils_h

#include "easygl_macro_helper.hpp"
#include <string>
#include <type_traits>
#include <atomic>

#ifdef __APPLE__
#include <TargetConditionals.h>

#if TARGET_OS_IPHONE != 1 // begin of macos
#define USING_OPENGL_3
#include <OpenGL/gl3.h>

#else // end of macos; begin of iOS
#define USING_OPENGL_ES
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#endif // end of iOS

#elif defined(__ANDROID__) // end of __APPLE__; begin of android
#define USING_OPENGL_ES
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <android/native_window.h> // requires ndk r5 or newer
#include <android/native_window_jni.h> // requires ndk r5 or newer

#define GLhalf GLshort

//VAO OES extensions must be load with eglGetProcAddress
extern PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOES;
extern PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOES;
extern PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOES;
extern PFNGLISVERTEXARRAYOESPROC glIsVertexArrayOES;

void loadVAOFunctions();

#else // end of android; begin of ubuntu

#define GL_GLEXT_PROTOTYPES
#include "GL/gl.h"
#include "GL/glext.h"
#include "GL/osmesa.h"

#endif // end of ubuntu

#ifdef USING_OPENGL_ES
#define GLdouble double
#define GL_DOUBLE GL_FLOAT
#define GL_RED GL_LUMINANCE

#define GL_DEPTH24_STENCIL8 GL_DEPTH24_STENCIL8_OES
#define glGenVertexArrays glGenVertexArraysOES
#define glBindVertexArray glBindVertexArrayOES
#define glDeleteVertexArrays glDeleteVertexArraysOES
#define glIsVertexArray glIsVertexArrayOES
#endif // end of opengl es

#ifdef EASYGL_DEBUG
#define EASYGL_CHECK_ERROR_(_x, _s) \
do { \
    _x;\
    GLenum errCode = glGetError(); \
    if (errCode != GL_NO_ERROR) { \
        std::cerr << "glCheckError : " << GetGLErrorString(errCode) << "\n" \
            << "Description: " << _s << "\n" \
            << "Source: " << __FILE__ << ":" << __LINE__ \
            << std::endl; \
        abort(); \
    } \
} while(0)
#define EASYGL_CHECK_ERROR(_x) EASYGL_CHECK_ERROR_(_x, #_x)
#define EASYGL_CHECK_ERROR_DESCR(_s) EASYGL_CHECK_ERROR_(, _s)

#else // end EASYGL_DEBUG
#define EASYGL_CHECK_ERROR(_x) _x
#define EASYGL_CHECK_ERROR_DESCR(_s)
#endif // end NOT EASYGL_DEBUG

NS_EASYGL_BEGIN

template<size_t Type>
class UIDGenerator
{
public:
    static size_t Current() { return _Current; }
    static size_t Next() { return ++_Current; }
    
private:
    static std::atomic<size_t> _Current;
};
template<size_t Type>
std::atomic<size_t> UIDGenerator<Type>::_Current(0);

template<typename T, size_t Type>
struct UIDEntry
{
    static const size_t UID;
};
template<typename T, size_t Type>
const size_t UIDEntry<T, Type>::UID = UIDGenerator<Type>::Next();

enum UIDType
{
    UIDType_Shader = 1,
};


using GetGLStatusFunc = std::add_pointer<void(GLuint, GLenum, GLint *)>::type;
using GetGLInfoLogFunc = std::add_pointer<void(GLuint, GLsizei, GLsizei *, GLchar *)>::type;
bool GetAndCheckGLStatus(GetGLStatusFunc getStatus, GLuint target, GLenum type,
                         GetGLInfoLogFunc getLog, std::string *errMsg);
const char * GetGLErrorString(GLenum errCode);

NS_EASYGL_END

#endif /* easygl_utils_h */
