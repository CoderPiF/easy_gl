//
//  Created by Pi on 2021/2/9.
//

#ifndef easygl_vertex_h
#define easygl_vertex_h

#include "easygl_types.hpp"
#include <vector>
#include <type_traits>

NS_EASYGL_BEGIN

struct VertexLocation
{
    using Ptr = std::shared_ptr<VertexLocation>;
    GLuint location = 0;
    virtual void bind(GLuint program) = 0;
    virtual void sync() const = 0;
    virtual void disable() const = 0;
    virtual ~VertexLocation() {}
};

struct VertexDescriptor
{
    using Ptr = std::shared_ptr<VertexDescriptor>;
    virtual void bind(GLuint program) = 0;
    virtual void sync() const = 0;
    virtual void disable() const = 0;
    virtual ~VertexDescriptor() {}
};

template<typename T>
struct VertexLocationBase : public VertexLocation
{
    void bind(GLuint program, const GLchar *name)
    {
        location = glGetAttribLocation(program, name);
        EASYGL_CHECK_ERROR_DESCR("glGetAttribLocation");
        EASYGL_ASSERT(location != -1, "can not found vertex.location : " + std::string(name));
    }
    
    void disable() const override
    {
        EASYGL_CHECK_ERROR(glDisableVertexAttribArray(location));
    }
    
    void sync() const override
    {
        EASYGL_CHECK_ERROR(glEnableVertexAttribArray(location));
        EASYGL_CHECK_ERROR(glVertexAttribPointer(location, T::Size, T::Type, T::Normalized, T::Stride, T::Pointer()));
    }
};

template<typename T>
struct VertexDescriptorBase;

template<typename T, typename M, M T::*MPtr, GLboolean N>
struct VertexDescriptorTraits
{
    static const GLint Size = GLTypeTraits<M>::size;
    static const GLenum Type = GLTypeTraits<M>::gl_type;
    static const GLboolean Normalized = N;
    static const GLsizei Stride = sizeof(T);
    static const GLvoid * Pointer()
    {
        return (void *)&((T *)nullptr->*MPtr);
    }
};

#define __DEF_VERTEX_DESCRIPTOR(_name) locations[_name] = type::Location_##_name::Make();
#define EXPAND_VERTEX_DESCRIPTOR(...) EASYGL_EXPAND_ARGS(__DEF_VERTEX_DESCRIPTOR, __VA_ARGS__)

#define DEF_VERTEX_DESCRIPTOR(_type, ...) \
template<>\
struct VertexDescriptorBase<_type> final : public VertexDescriptor { \
    using type = _type; \
    enum { __VA_ARGS__, vertex_size }; \
    VertexLocation::Ptr locations[vertex_size]; \
    void bind(GLuint program) override { \
        for (size_t i = 0; i < vertex_size; ++i) { \
            locations[i]->bind(program); \
        } \
    } \
    void sync() const override { \
        for (size_t i = 0; i < vertex_size; ++i) { \
            locations[i]->sync(); \
        } \
} \
    void disable() const override { \
        for (size_t i = 0; i < vertex_size; ++i) { \
            locations[i]->disable(); \
        } \
    } \
    VertexDescriptorBase() { \
        EXPAND_VERTEX_DESCRIPTOR(__VA_ARGS__) \
    } \
}; \

#define __DEF_VERTEX(_type, _name, _normalized) \
_type _name; \
using Descriptor_##_name = VertexDescriptorTraits<type, _type, &type::_name, _normalized>; \
struct Location_##_name final : public VertexLocationBase<Descriptor_##_name> { \
    using type = Location_##_name; \
    static VertexLocation::Ptr Make() { return std::make_shared<type>(); } \
    void bind(GLuint program) override { VertexLocationBase<Descriptor_##_name>::bind(program, #_name); } \
};
#define _DEF_VERTEX(_type, _name, _normalized) __DEF_VERTEX(_GLRealType<_type>::real_type, _name, _normalized)


#define DEF_VERTEX(_type, _name) _DEF_VERTEX(_type, _name, GL_FALSE)
#define DEF_NORMALIZED_VERTEX(_type, _name) _DEF_VERTEX(_type, _name, GL_TRUE)

#define DEF_VERTEX_BEGIN(_name) struct _name { using type = _name;
#define DEF_VERTEX_END                       };


NS_EASYGL_END

#endif /* easygl_vertex_h */
