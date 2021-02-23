//
//  Created by Pi on 2021/2/9.
//

#ifndef easygl_program_hpp
#define easygl_program_hpp

#include "easygl_utils.hpp"
#include "easygl_shader.hpp"
#include "easygl_context.hpp"
#include "easygl_resource.hpp"
#include <string>
#include <memory>
#include <mutex>
#include <thread>

NS_EASYGL_BEGIN

class ShaderProgram
{
    EASYGL_DISABLE_COPY(ShaderProgram);
public:
    using Ptr = std::shared_ptr<ShaderProgram>;
    
    operator GLuint() const;
    bool setup(std::string *errMsg);
    const Context::Ptr & context() const;
    void syncUniforms(const uniform_data &data) const;
    void syncVertexs() const;
    void disableVertexs() const;

    void use() const;
    bool validate(std::string *errMsg) const;
    void invalidate();

    virtual ~ShaderProgram();
    ShaderProgram(const Shader::Ptr &vsh, const Shader::Ptr &fsh,
                  const UniformDescriptor::Ptr &uniformDescr,
                  const VertexDescriptor::Ptr &vertexDescr);

protected:
    mutable std::mutex _mutex;
    Context::Ptr _ctx;
    GLuint _program = 0;
    Shader::Ptr _fsh;
    Shader::Ptr _vsh;
    UniformDescriptor::Ptr _uniformDescr;
    VertexDescriptor::Ptr _vertexDescr;
};

template<typename VSHType, typename FSHType>
class ShaderProgramBase final : public ShaderProgram
{
public:
    using Ptr = std::shared_ptr<ShaderProgramBase>;
    using vsh_type = VSHType;
    using fsh_type = FSHType;
    using vsh_ptr = std::shared_ptr<vsh_type>;
    using fsh_ptr = std::shared_ptr<fsh_type>;
    
    using vertex_type = typename vsh_type::vertex_type;
    using vertex_descriptor = typename vsh_type::vertex_descriptor;
    struct uniform_type final : public vsh_type::uniform_type, public fsh_type::uniform_type {};
    struct uniform_descriptor final : public vsh_type::uniform_descriptor,
                                      public fsh_type::uniform_descriptor {};

    ShaderProgramBase(const Shader::Ptr &vsh, const Shader::Ptr &fsh)
    : ShaderProgram(vsh, fsh, std::make_shared<uniform_descriptor>(), std::make_shared<vertex_descriptor>())
    {}
};

class ProgramSharedMgr final
{
    EASYGL_DISABLE_COPY(ProgramSharedMgr);
public:
    using Ptr = std::shared_ptr<ProgramSharedMgr>;
    using ProgramWeakPtr = std::weak_ptr<ShaderProgram>;
    using ProgramPtr = std::shared_ptr<ShaderProgram>;
    const Context::ID ctxId;

    template<typename VSHType, const GLchar *VSHSource, typename FSHType, const GLchar *FSHSource>
    typename ShaderProgramBase<VSHType, FSHType>::Ptr makeFromFixed()
    {
        return make(FixedShaderTraits<VSHType, VSHSource>(),
                    FixedShaderTraits<FSHType, FSHSource>(),
                    _fixedPrograms);
    }

    template<typename VSHType, typename FSHType, const GLchar *FSHSource>
    typename ShaderProgramBase<VSHType, FSHType>::Ptr makeFromVSHSrc(const std::string &vshSrc)
    {
        return make(DynamicShaderTraits<VSHType>(vshSrc),
                    FixedShaderTraits<FSHType, FSHSource>(),
                    _fixedFSHPrograms);
    }

    template<typename VSHType, const GLchar *VSHSource, typename FSHType>
    typename ShaderProgramBase<VSHType, FSHType>::Ptr makeFromFSHSrc(const std::string &fshSrc)
    {
        return make(FixedShaderTraits<VSHType, VSHSource>(),
                    DynamicShaderTraits<FSHType>(fshSrc),
                    _fixedVSHPrograms);
    }

    template<typename VSHType, typename FSHType>
    typename ShaderProgramBase<VSHType, FSHType>::Ptr makeFromVSHSrcFSHSrc(const std::string &vshSrc, const std::string &fshSrc)
    {
        return make(DynamicShaderTraits<VSHType>(vshSrc),
                    DynamicShaderTraits<FSHType>(fshSrc),
                    _dynamicPrograms);
    }

private:
    friend class ResourcesImpl;
    ProgramSharedMgr(Context::ID aCtxId, const ShaderSharedMgr::Ptr &shaderMgr);
    void clear();
    
    template<typename K1T, typename K2T>
    ProgramPtr find(const std::map<K1T, std::map<K2T, ProgramWeakPtr>> &sharedPrograms,
                    const K1T &k1, const K2T &k2) const
    {
        EASYGL_AUTO_LOCK(_mutex);
        auto it1 = sharedPrograms.find(k1);
        if (it1 == sharedPrograms.end()) { return nullptr; }
        auto it2 = it1->second.find(k2);
        if (it2 == it1->second.end()) { return nullptr; }
        return it2->second.lock();
    }
    
    template<typename VSHTraits, typename FSHTraits>
    typename ShaderProgramBase<typename VSHTraits::shader_type, typename FSHTraits::shader_type>::Ptr
    make(const VSHTraits &vshTraits, const FSHTraits &fshTraits,
         std::map<typename VSHTraits::uid_type, std::map<typename FSHTraits::uid_type, ProgramWeakPtr>> &sharedPrograms)
    {
        EASYGL_AUTO_LOCK(_mutex);
        using program_type = ShaderProgramBase<typename VSHTraits::shader_type, typename FSHTraits::shader_type>;
        auto program = find(sharedPrograms, vshTraits.uid(), fshTraits.uid());
        if (!program) {
            program = std::make_shared<program_type>(_shaderMgr->makeShader(vshTraits),
                                                     _shaderMgr->makeShader(fshTraits));
            sharedPrograms[vshTraits.uid()][fshTraits.uid()] = program;
        }
        return std::static_pointer_cast<program_type>(program);
    }

private:
    mutable std::recursive_mutex _mutex;
    ShaderSharedMgr::Ptr _shaderMgr;
    std::map<size_t, std::map<size_t, ProgramWeakPtr>> _fixedPrograms;
    std::map<size_t, std::map<std::string, ProgramWeakPtr>> _fixedVSHPrograms;
    std::map<std::string, std::map<size_t, ProgramWeakPtr>> _fixedFSHPrograms;
    std::map<std::string, std::map<std::string, ProgramWeakPtr>> _dynamicPrograms;
};

NS_EASYGL_END

#endif /* easygl_program_hpp */
