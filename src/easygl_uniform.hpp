//
//  Created by Pi on 2021/2/9.
//

#ifndef easygl_uniform_h
#define easygl_uniform_h

#include "easygl_anytype.hpp"
#include <map>
#include <string>
#include <memory>

NS_EASYGL_BEGIN

using uniform_location = std::map<std::string, GLint>;
using uniform_data = std::map<std::string, GLAnyType::Ptr>;

class UniformDescriptor
{
    EASYGL_DISABLE_COPY(UniformDescriptor);
public:
    using Ptr = std::shared_ptr<UniformDescriptor>;
    virtual ~UniformDescriptor() {}
    UniformDescriptor(){}
    
    void bind(GLuint program)
    {
        for (auto &info : _locations)
        {
            info.second = glGetUniformLocation(program, info.first.c_str());
            EASYGL_CHECK_ERROR_DESCR("glGetUniformLocation");
            EASYGL_ASSERT(info.second != -1, "can not found uniform.location in program : " + info.first);
        }
    }
    
    void sync(const uniform_data &data) const
    {
        for (auto &info : data)
        {
            auto iter = _locations.find(info.first);
            EASYGL_ASSERT(iter != _locations.end(), "can not found uniform : " + info.first);
            info.second->syncUniform(iter->second);
        }
    }

protected:
    uniform_location _locations;
};

template<typename T>
struct UniformDescriptorBase;

class UniformBase
{
    EASYGL_DISABLE_COPY(UniformBase);
public:
    UniformBase(){}
    const uniform_data & getData() const { return _uniforms; }

    template<typename T, size_t C>
    void setUniform(const std::string &key, const T v[C])
    {
        _uniforms[key] = GLAnyType::Create<T, C>(v);
    }
    template<typename T>
    void setUniform(const std::string &key, const T &v)
    {
        setUniform<T, 1>(key, &v);
    }
    
    template<typename T, size_t C>
    bool getUniform(const std::string &key, T **vp) const
    {
        auto iter = _uniforms.find(key);
        if (iter == _uniforms.end())
        {
            return false;
        }
        
        const auto &tv = std::static_pointer_cast<GLAny<T, C>>(iter->second)->value;
        for (int i = 0; i < C; ++i)
        {
            *vp[i] = tv[i];
        }
        return true;
    }
    
    template<typename T>
    bool getUniform(const std::string &key, T *v) const
    {
        return getUniform<T, 1>(key, &v);
    }

private:
    uniform_data _uniforms;
};

#define DEF_UNIFORM_BEGIN(_name) \
struct _name : virtual public UniformBase {

#define DEF_UNIFORM(_type, _key) \
void _key(const _type &v) { setUniform<_type>(#_key, v); } \
_type _key() const { _type tmp; getUniform<_type>(#_key, &tmp); return tmp; }

#define DEF_UNIFORM_ARRAY(_type, _key, _n) \
void _key(const _type v[_n]) { setUniform<_type, _n>(#_key, v); } \

#define DEF_UNIFORM_END };


#define __DEF_UNIFORM_DESCRIPTOR(_name) _locations[#_name] = -1;
#define EXPAND_UNIFORM_DESCRIPTOR(...) EASYGL_EXPAND_ARGS(__DEF_UNIFORM_DESCRIPTOR, ##__VA_ARGS__)

#define DEF_UNIFORM_DESCRIPTOR(_type, ...) \
template<> \
struct UniformDescriptorBase<_type> : virtual public UniformDescriptor { \
    using type = _type; \
    UniformDescriptorBase() { \
        EXPAND_UNIFORM_DESCRIPTOR(__VA_ARGS__) \
    } \
};

DEF_UNIFORM_BEGIN(EmptyUniform)
DEF_UNIFORM_END
DEF_UNIFORM_DESCRIPTOR(EmptyUniform);

NS_EASYGL_END

#endif /* easygl_uniform_h */
