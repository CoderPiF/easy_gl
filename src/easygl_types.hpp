//
//  Created by Pi on 2021/2/9.
//

#ifndef easygl_types_h
#define easygl_types_h

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include "easygl_utils.hpp"

NS_EASYGL_BEGIN

struct GLScalarType {};
struct GLComplexType {};

template<typename T>
struct _GLTypeTraitsImpl;

template<typename T>
struct _TypeFlag
{
    template<typename _T, typename std::decay<decltype(_GLTypeTraitsImpl<_T>::gl_type)>::type * = nullptr>
    static GLScalarType Check(GLScalarType);
    template<typename _T>
    static GLComplexType Check(...);
    using type_flag = typename std::decay<decltype(Check<T>(GLScalarType()))>::type;
};

template<typename T>
struct _GLRealType
{
    template<typename _T, typename _T::real_type * = nullptr>
    static typename _T::real_type Check(GLComplexType);
    template<typename _T>
    static _T Check(...);
    using real_type = typename std::decay<decltype(Check<T>(GLComplexType()))>::type;
};

template<typename T>
struct GLTypeTraits
{
    using type = typename _GLRealType<T>::real_type;
    using value_type = typename _GLTypeTraitsImpl<type>::value_type;
    using type_flag = typename _TypeFlag<type>::type_flag;
    static const GLenum gl_type = _GLTypeTraitsImpl<value_type>::gl_type;
    static const GLint size = sizeof(type) / sizeof(value_type);
};

#define DEF_SCALAR_GLTYPE(_t, _e) \
template<> \
struct _GLTypeTraitsImpl<_t> { \
    using value_type = _t; \
    static const GLenum gl_type = _e; \
}
/*
GLbyte    8   Signed, 2's complement binary integer   GL_BYTE
GLubyte   8   Unsigned binary integer                 GL_UNSIGNED_BYTE
GLshort   16  Signed, 2's complement binary integer   GL_SHORT
GLushort  16  Unsigned binary integer                 GL_UNSIGNED_SHORT
GLint     32  Signed, 2's complement binary integer   GL_INT
GLuint    32  Unsigned binary integer                 GL_UNSIGNED_INT
GLfixed   32  Signed, 2's complement 16.16 integer    GL_FIXED
GLhalf    16  An IEEE-754 floating-point value        GL_HALF_FLOAT
GLfloat   32  An IEEE-754 floating-point value        GL_FLOAT
GLdouble  64  An IEEE-754 floating-point value        GL_DOUBLE
*/
DEF_SCALAR_GLTYPE(GLbyte, GL_BYTE);
DEF_SCALAR_GLTYPE(GLubyte, GL_UNSIGNED_BYTE);
DEF_SCALAR_GLTYPE(GLshort, GL_SHORT);
DEF_SCALAR_GLTYPE(GLushort, GL_UNSIGNED_SHORT);
DEF_SCALAR_GLTYPE(GLint, GL_INT);
DEF_SCALAR_GLTYPE(GLuint, GL_UNSIGNED_INT);
DEF_SCALAR_GLTYPE(GLfloat, GL_FLOAT);
DEF_SCALAR_GLTYPE(GLdouble, GL_DOUBLE);
#undef DEF_SCALAR_GLTYPE

template<typename T>
struct _GLTypeTraitsImpl
{
    using value_type = typename T::value_type;
};

template <typename T>
struct TransposeMatrix : public T
{
    using value_type = typename T::value_type;
    static const value_type * value_ptr(const TransposeMatrix &value)
    {
        return glm::value_ptr(static_cast<const T &>(value));
    }
};

// type ptr
struct GLTypeValuePtr
{
    template<typename T>
    static const typename GLTypeTraits<T>::value_type * value_ptr(const T &value)
    {
        return value_ptr(value, (typename _TypeFlag<T>::type_flag){});
    }
    
    template<typename T>
    static const typename GLTypeTraits<T>::value_type * value_ptr(const T &value, GLScalarType)
    {
        return &value;
    }
    
    template<typename T>
    static const typename GLTypeTraits<T>::value_type * value_ptr(const T &value, GLComplexType)
    {
        return complex_value_ptr(value, GLComplexType());
    }
    
    template<typename T, typename std::decay<decltype(&T::value_ptr)>::type * = nullptr>
    static const typename GLTypeTraits<T>::value_type * complex_value_ptr(const T &value, GLComplexType)
    {
        return T::value_ptr(value);
    }
    
    template<typename T>
    static const typename GLTypeTraits<T>::value_type * complex_value_ptr(const T &value, ...)
    {
        return glm::value_ptr(value);
    }
};

NS_EASYGL_END

#endif /* easygl_types_h */
