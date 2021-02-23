//
//  Created by Pi on 2021/2/18.
//

#include "easygl_resource.hpp"
#include "easygl_shader.hpp"
#include "easygl_program.hpp"
#include <map>
#include <set>
#include <mutex>
#include <memory>
#include <type_traits>

NS_EASYGL_BEGIN

class ResourcesImpl final : public Resources
{
public:
    using Ptr = std::shared_ptr<ResourcesImpl>;
    bool tryDelete(Type type, GLuint target) override
    {
        if (target == 0) {
            return true;
        }
        
        EASYGL_AUTO_LOCK(_mutex);
        _targets[type].insert(target);
        return tryClearDeleteQueue();
    }
    
    bool tryClearDeleteQueue() override
    {
        auto curCtx = ContextMgr::CurrentCtx();
        if (!curCtx || curCtx->ctxId() != ctxId) {
            return false;
        }
        
        EASYGL_AUTO_LOCK(_mutex);
        deleteNRes(Resources::VAO, glDeleteVertexArrays);
        deleteNRes(Resources::VBO, glDeleteBuffers);
        deleteNRes(Resources::Texture, glDeleteTextures);
        deleteNRes(Resources::FBO, glDeleteFramebuffers);
        deleteNRes(Resources::RBO, glDeleteRenderbuffers);
        deleteRes(Resources::Shader, glDeleteShader);
        deleteRes(Resources::Program, glDeleteProgram);
        return true;
    }
    
    bool clearAll() override
    {
        EASYGL_AUTO_LOCK(_mutex);
        if (_programMgr) {
            _programMgr->clear();
            _programMgr.reset();
        }
        if (_shaderMgr) {
            _shaderMgr->clear();
            _shaderMgr.reset();
        }
        return tryClearDeleteQueue();
    }
    
    ShaderSharedMgr::Ptr shaderMgr() override
    {
        EASYGL_AUTO_LOCK(_mutex);
        if (!_shaderMgr) {
            _shaderMgr.reset(new ShaderSharedMgr(ctxId));
        }
        return _shaderMgr;
    }
    
    ProgramSharedMgr::Ptr programMgr() override
    {
        EASYGL_AUTO_LOCK(_mutex);
        if (!_programMgr) {
            _programMgr.reset(new ProgramSharedMgr(ctxId, shaderMgr()));
        }
        return _programMgr;
    }
    
    ~ResourcesImpl() { clearAll(); }
    ResourcesImpl(Context::ID ctxId) : Resources(ctxId) {}
    
private:
    void _deleteRes(Resources::Type type, std::function<void(GLuint)> func)
    {
        for (auto &target : _targets[type]) {
            func(target);
        }
        _targets[type].clear();
    }
    
    using DeleteFunc = std::add_pointer<void(GLuint)>::type;
    void deleteRes(Resources::Type type, DeleteFunc delFunc)
    {
        _deleteRes(type, [&](GLuint target){
            EASYGL_CHECK_ERROR(delFunc(target));
        });
    }
    
    using DeleteNFunc = std::add_pointer<void(GLsizei, const GLuint *)>::type;
    void deleteNRes(Resources::Type type, DeleteNFunc delFunc)
    {
        _deleteRes(type, [&](GLuint target){
            EASYGL_CHECK_ERROR(delFunc(1, &target));
        });
    }
    
private:
    std::recursive_mutex _mutex;
    std::set<GLuint> _targets[Resources::MAX];
    ShaderSharedMgr::Ptr _shaderMgr;
    ProgramSharedMgr::Ptr _programMgr;
};

Resources::~Resources() {}
Resources::Resources(Context::ID aCtxId) : ctxId(aCtxId) {}
Resources::Ptr Resources::Create(Context::ID aCtxId)
{
    return std::make_shared<ResourcesImpl>(aCtxId);
}

NS_EASYGL_END
