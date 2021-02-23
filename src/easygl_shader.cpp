//
//  Created by Pi on 2021/2/19.
//

#include "easygl_shader.hpp"
#include <vector>

NS_EASYGL_BEGIN

Shader::operator GLuint () const { return _shader; }

Shader::Shader() {}
Shader::~Shader() { invalidate(); }
void Shader::invalidate()
{
    EASYGL_AUTO_LOCK(_mutex);
    if (_shader > 0)
    {
        EASYGL_ASSERT(_ctx, "context invalid but shader has been created!");
        bool hasDelete = _ctx->resources()->tryDelete(Resources::Shader, _shader);
        EASYGL_ASSERT(hasDelete, "delete shader when context is not current");
        _shader = 0;
    }
}

bool Shader::doCompile(const GLchar *source, GLenum shaderType, std::string *errMsg)
{
    EASYGL_AUTO_LOCK(_mutex);
    if (_shader > 0) { return true; }
    _ctx = ContextMgr::CurrentCtx();
    EASYGL_ASSERT(_ctx, "current context is null!!");
    if (!_ctx) { return false; }

    _shader = glCreateShader(shaderType);
    EASYGL_CHECK_ERROR_DESCR("glCreateShader");
    EASYGL_ASSERT(_shader > 0, "create shader error!");
    if (_shader == 0) { return false; }
    
    EASYGL_CHECK_ERROR(glShaderSource(_shader, 1, &source, NULL));
    EASYGL_CHECK_ERROR(glCompileShader(_shader));
    if (!GetAndCheckGLStatus(glGetShaderiv, _shader, GL_COMPILE_STATUS,
                             glGetShaderInfoLog, errMsg))
    {
        // rollback
        EASYGL_CHECK_ERROR(glDeleteShader(_shader));
        _shader = 0;
        return false;
    }
    return true;
}

ShaderSharedMgr::ShaderSharedMgr(Context::ID aCtxId) : ctxId(aCtxId) {}

void ShaderSharedMgr::clear()
{
    std::vector<ShaderPtr> shaders;
    {
        EASYGL_AUTO_LOCK(_mutex);
        for (auto &iter : _fixShaders) {
            if (auto tmp = iter.second.lock()) {
                shaders.push_back(tmp);
            }
        }
        _fixShaders.clear();
        for (auto &iter : _dynamicShaders) {
            if (auto tmp = iter.second.lock()) {
                shaders.push_back(tmp);
            }
        }
        _dynamicShaders.clear();
    }
    for (auto &shader : shaders) {
        shader->invalidate();
    }
}

NS_EASYGL_END
