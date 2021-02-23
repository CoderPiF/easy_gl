//
//  Created by Pi on 2021/2/10.
//

#ifndef easygl_shader_h
#define easygl_shader_h

#include "easygl_utils.hpp"
#include "easygl_uniform.hpp"
#include "easygl_vertex.hpp"
#include "easygl_resource.hpp"
#include <string>
#include <map>
#include <mutex>

NS_EASYGL_BEGIN

class Shader
{
    EASYGL_DISABLE_COPY(Shader);
public:
    using Ptr = std::shared_ptr<Shader>;
    operator GLuint () const;
    virtual bool compile(std::string *errMsg) = 0;
    void invalidate();

    virtual ~Shader();
    Shader();

protected:
    bool doCompile(const GLchar *source, GLenum shaderType, std::string *errMsg);
    
    std::mutex _mutex;
    Context::Ptr _ctx;
    GLuint _shader = 0;
};

template<typename VertexType, typename UniformType = EmptyUniform>
class VertexShader : public Shader
{
public:
    enum { gl_shader_type = GL_VERTEX_SHADER };
    virtual ~VertexShader() {}
    using vertex_type = VertexType;
    using vertex_descriptor = VertexDescriptorBase<VertexType>;
    struct uniform_type : virtual public UniformType {};
    struct uniform_descriptor : virtual public UniformDescriptorBase<UniformType> {};
};

template<typename UniformType>
class FragmentShader : public Shader
{
public:
    enum { gl_shader_type = GL_FRAGMENT_SHADER };
    virtual ~FragmentShader() {}
    struct uniform_type : virtual public UniformType {};
    struct uniform_descriptor : virtual public UniformDescriptorBase<UniformType> {};
};

template<typename ShaderType, const GLchar *ShaderSource>
class FixedShader final : public ShaderType,
public UIDEntry<FixedShader<ShaderType, ShaderSource>, UIDType_Shader>
{
public:
    using type = FixedShader<ShaderType, ShaderSource>;
    using shader_type = ShaderType;
    static std::shared_ptr<shader_type> Make()
    {
        return std::make_shared<type>();
    }

    bool compile(std::string *errMsg) override
    {
        return Shader::doCompile(ShaderSource, shader_type::gl_shader_type, errMsg);
    }
};

template<typename ShaderType>
class DynamicShader final : public ShaderType
{
public:
    static const size_t UID = 0;
    using type = DynamicShader<ShaderType>;
    using shader_type = ShaderType;
    static std::shared_ptr<shader_type> Make(const std::string &source)
    {
        return std::make_shared<type>(source);
    }
    
    DynamicShader(const std::string &source) : _source(source) {}
    bool compile(std::string *errMsg) override
    {
        if (Shader::_shader > 0) { return true; }
        return Shader::doCompile(_source.c_str(), shader_type::gl_shader_type, errMsg);
    }

private:
    const std::string _source;
};

template<typename ShaderType, const GLchar *ShaderSource>
struct FixedShaderTraits final
{
    using type = FixedShader<ShaderType, ShaderSource>;
    using shader_type = ShaderType;
    using shader_ptr = std::shared_ptr<shader_type>;
    using uid_type = size_t;
    
    uid_type uid() const { return type::UID; }
    shader_ptr make() const { return std::make_shared<type>(); }
};

template<typename ShaderType>
struct DynamicShaderTraits final
{
    using type = DynamicShader<ShaderType>;
    using shader_type = ShaderType;
    using shader_ptr = std::shared_ptr<shader_type>;
    using uid_type = std::string;

    const uid_type & uid() const { return source; }
    shader_ptr make() const { return std::make_shared<type>(source); }
    
    DynamicShaderTraits(const std::string &src) : source(src) {}
    const std::string &source;
};

class ShaderSharedMgr final
{
    EASYGL_DISABLE_COPY(ShaderSharedMgr);
public:
    using Ptr = std::shared_ptr<ShaderSharedMgr>;
    using ShaderWeakPtr = std::weak_ptr<Shader>;
    using ShaderPtr = std::shared_ptr<Shader>;
    const Context::ID ctxId;
    
    template<typename ShaderType, const GLchar *ShaderSource>
    ShaderPtr makeShader()
    {
        return make(FixedShaderTraits<ShaderType, ShaderSource>(), _fixShaders);
    }
    
    template<typename ShaderType>
    ShaderPtr makeShader(const std::string &source)
    {
        return make(DynamicShaderTraits<ShaderType>(source), _dynamicShaders);
    }
    
    template<typename ShaderTraits>
    ShaderPtr makeShader(const ShaderTraits &traits)
    {
        return make(traits, traits.uid());
    }

private:
    friend class ResourcesImpl;
    ShaderSharedMgr(Context::ID aCtxId);
    void clear();

    template<typename ShaderTraits>
    ShaderPtr make(const ShaderTraits &traits, std::map<typename ShaderTraits::uid_type, ShaderWeakPtr> &mgr)
    {
        EASYGL_AUTO_LOCK(_mutex);
        auto shader = mgr[traits.uid()].lock();
        if (!shader) {
            shader = traits.make();
            mgr[traits.uid()] = shader;
        }
        return shader;
    }
    
    template<typename ShaderTraits>
    ShaderPtr make(const ShaderTraits &traits, const std::string &)
    {
        return make(traits, _dynamicShaders);
    }
    template<typename ShaderTraits>
    ShaderPtr make(const ShaderTraits &traits, size_t)
    {
        return make(traits, _fixShaders);
    }

private:
    std::mutex _mutex;
    std::map<size_t, ShaderWeakPtr> _fixShaders;
    std::map<std::string, ShaderWeakPtr> _dynamicShaders;
};

NS_EASYGL_END

#endif /* easygl_shader_h */
