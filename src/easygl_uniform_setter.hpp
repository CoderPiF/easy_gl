//
//  easygl_uniform_setter.hpp
//  EasyGLDemo
//
//  Created by 江派锋 on 2021/2/22.
//

#ifndef easygl_uniform_setter_h
#define easygl_uniform_setter_h

#include "easygl_types.hpp"

NS_EASYGL_BEGIN

struct GLUniformSetter
{
    template<typename T, size_t C>
    static void Set(GLint location, ...)
    {
        static_assert(C == 0, "NOT SUPPORT");
    }
    
    // custom type
    template<typename T, size_t C, typename T::real_type * = nullptr>
    static void Set(GLint location, const T v[C]) {
        using real_type = typename T::real_type;
        real_type tmp[C];
        for (size_t i = 0; i < C; ++i) {
            tmp[i] = v[i];
        }
        Set<real_type, C>(location, tmp);
    }
    
#define DEF_SCALAR_SETTER(_t, _f) \
    template<typename T, size_t C> \
    static void Set(GLint location, const _t v[C]) { \
        static_assert(C == 1, "NOT SUPPORT multi for this type"); \
        EASYGL_CHECK_ERROR(glUniform1##_f(location, v[0])); \
    }
    DEF_SCALAR_SETTER(GLbyte, i);
    DEF_SCALAR_SETTER(GLubyte, i);
    DEF_SCALAR_SETTER(GLshort, i);
    DEF_SCALAR_SETTER(GLushort, i);
    DEF_SCALAR_SETTER(GLdouble, f);
    DEF_SCALAR_SETTER(GLuint, i);
#undef DEF_SCALAR_SETTER
    
#define _DEF_SETTER_V(_n, _t, _f) \
    template<typename T, size_t C> \
    static void Set(GLint location, const _t v[C]) { \
        EASYGL_CHECK_ERROR(glUniform##_n##_f##v(location, C, GLTypeValuePtr::value_ptr<_t>(v[0]))); \
    }
    
#define DEF_SCALAR_SETTER_V(_t, _f) _DEF_SETTER_V(1, _t, _f)
    DEF_SCALAR_SETTER_V(GLint, i);
    DEF_SCALAR_SETTER_V(GLfloat, f);
#undef DEF_SCALAR_SETTER_V
    
#define __DEF_VEC_SETTER_V(_n, _t, _f) _DEF_SETTER_V(_n, glm::_t##_n, _f);
#define _DEF_IVEC_SETTER_V(_n) __DEF_VEC_SETTER_V(_n, ivec, i)
#define _DEF_FVEC_SETTER_V(_n) __DEF_VEC_SETTER_V(_n, vec, f)

#define _DEF_MATRIX_SETTER_V(_n) \
    template<typename T, size_t C> \
    static void Set(GLint location, const glm::mat##_n v[C]) { \
        EASYGL_CHECK_ERROR(glUniformMatrix##_n##fv(location, C, false, GLTypeValuePtr::value_ptr<glm::mat##_n>(v[0]))); \
    } \
    template<typename T, size_t C> \
    static void Set(GLint location, const TransposeMatrix<glm::mat##_n> v[C]) { \
        EASYGL_CHECK_ERROR(glUniformMatrix##_n##fv(location, C, false, GLTypeValuePtr::value_ptr<TransposeMatrix<glm::mat##_n>>(v[0]))); \
    }
    
#define DEF_GLM_SETTER_V(_macro) \
    _macro(2) \
    _macro(3) \
    _macro(4)
    
    DEF_GLM_SETTER_V(_DEF_IVEC_SETTER_V);
    DEF_GLM_SETTER_V(_DEF_FVEC_SETTER_V);
    DEF_GLM_SETTER_V(_DEF_MATRIX_SETTER_V);
    
#undef __DEF_VEC_SETTER_V
#undef _DEF_IVEC_SETTER_V
#undef _DEF_FVEC_SETTER_V
#undef _DEF_MATRIX_SETTER_V
#undef DEF_GLM_SETTER_V
    
};

NS_EASYGL_END

#endif /* easygl_uniform_setter_h */
