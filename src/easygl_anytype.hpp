//
//  Created by Pi on 2021/2/22.
//

#ifndef easygl_anytype_h
#define easygl_anytype_h

#include "easygl_types.hpp"
#include "easygl_uniform_setter.hpp"

NS_EASYGL_BEGIN

template<typename T, size_t C>
struct GLAny;

struct GLAnyType
{
    using Ptr = std::shared_ptr<GLAnyType>;
    template<typename T, size_t C>
    static std::shared_ptr<GLAny<T, C>> Create(const T v[C])
    {
        return std::make_shared<GLAny<T, C>>(v);
    }
    
    template<typename T>
    static std::shared_ptr<GLAny<T, 1>> Create(const T &v)
    {
        return Create<T, 1>(&v);
    }
    
    virtual ~GLAnyType() {}
    virtual void syncUniform(GLint locaion) = 0;
};

template<typename T, size_t C>
struct GLAny final : public GLAnyType
{
    static const size_t size = C;
    using my_type = GLAny<T, C>;
    using type = T;
    using value_type = typename GLTypeTraits<T>::value_type;

    GLAny(const type v[size])
    {
        for (auto i = 0; i < size; ++i)
        {
            value[i] = v[i];
        }
    }
    
    void syncUniform(GLint locaion) override
    {
        GLUniformSetter::Set<T, C>(locaion, value);
    }
             
    type value[size];
};

NS_EASYGL_END

#endif /* easygl_anytype_h */
