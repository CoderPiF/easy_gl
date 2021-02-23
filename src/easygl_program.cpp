//
//  Created by Pi on 2021/2/19.
//

#include "easygl_program.hpp"
#include <vector>

NS_EASYGL_BEGIN

ShaderProgram::operator GLuint() const { return _program; }

ShaderProgram::~ShaderProgram() { invalidate(); }
ShaderProgram::ShaderProgram(const Shader::Ptr &vsh, const Shader::Ptr &fsh,
                             const UniformDescriptor::Ptr &uniformDescr,
                             const VertexDescriptor::Ptr &vertexDescr)
: _vsh(vsh), _fsh(fsh), _uniformDescr(uniformDescr), _vertexDescr(vertexDescr)
{
}

void ShaderProgram::use() const
{
    EASYGL_AUTO_LOCK(_mutex);
    EASYGL_CHECK_CURCTX_DESCR("use program");
    if (_ctx && _ctx->isCurrent()) {
        EASYGL_CHECK_ERROR(glUseProgram(_program));
    }
}

bool ShaderProgram::validate(std::string *errMsg) const
{
    EASYGL_AUTO_LOCK(_mutex);
    EASYGL_CHECK_CURCTX_DESCR("validate program");
    if (!_ctx || !_ctx->isCurrent()) {
        return false;
    }
    EASYGL_CHECK_ERROR(glValidateProgram(_program));
    return GetAndCheckGLStatus(glGetProgramiv, _program, GL_VALIDATE_STATUS, glGetProgramInfoLog, errMsg);
}

void ShaderProgram::invalidate()
{
    EASYGL_AUTO_LOCK(_mutex);
    if (_program > 0) {
        EASYGL_ASSERT(_ctx, "context invalid but program has been created!");
        bool hasDelete = _ctx->resources()->tryDelete(Resources::Program, _program);
        EASYGL_ASSERT(hasDelete, "delete program when context is not current");
        _program = 0;
    }
}

bool ShaderProgram::setup(std::string *errMsg)
{
    EASYGL_AUTO_LOCK(_mutex);
    if (_program > 0) { return true; }
    
    if (!_vsh->compile(errMsg)) { return false; }
    if (!_fsh->compile(errMsg)) { return false; }
    
    _ctx = ContextMgr::CurrentCtx();
    EASYGL_ASSERT(_ctx, "current context is null!!");
    if (!_ctx) { return false; }
    
    _program = glCreateProgram();
    EASYGL_CHECK_ERROR_DESCR("glCreateProgram");
    EASYGL_ASSERT(_program > 0, "create program error!");
    if (_program == 0) { return false; }
    
    EASYGL_CHECK_ERROR(glAttachShader(_program, *_vsh));
    EASYGL_CHECK_ERROR(glAttachShader(_program, *_fsh));
    
    EASYGL_CHECK_ERROR(glLinkProgram(_program));
    if (!GetAndCheckGLStatus(glGetProgramiv, _program, GL_LINK_STATUS, glGetProgramInfoLog, errMsg))
    {
        // rollback
        EASYGL_CHECK_ERROR(glDeleteProgram(_program));
        _program = 0;
        return false;
    }
    
    _uniformDescr->bind(_program);
    _vertexDescr->bind(_program);
    return true;
}

const Context::Ptr & ShaderProgram::context() const
{
    return _ctx;
}


void ShaderProgram::syncUniforms(const uniform_data &data) const
{
    EASYGL_CHECK_CURCTX(_uniformDescr->sync(data));
}

void ShaderProgram::syncVertexs() const
{
    EASYGL_CHECK_CURCTX(_vertexDescr->sync());
}

void ShaderProgram::disableVertexs() const
{
    EASYGL_CHECK_CURCTX(_vertexDescr->disable());
}

ProgramSharedMgr::ProgramSharedMgr(Context::ID aCtxId, const ShaderSharedMgr::Ptr &shaderMgr)
: ctxId(aCtxId), _shaderMgr(shaderMgr)
{}

template<typename K1T,typename K2T>
static void s_moveAllPrograms(std::map<K1T, std::map<K2T, ProgramSharedMgr::ProgramWeakPtr>> &shareds,
                              std::vector<ShaderProgram::Ptr> &programs)
{
    for (auto &iter1 : shareds) {
        for (auto &iter2 : iter1.second) {
            if (auto tmp = iter2.second.lock()) {
                programs.push_back(tmp);
            }
        }
    }
    shareds.clear();
}

void ProgramSharedMgr::clear()
{
    std::vector<ProgramPtr> programs;
    {
        EASYGL_AUTO_LOCK(_mutex);
        s_moveAllPrograms(_fixedPrograms, programs);
        s_moveAllPrograms(_fixedVSHPrograms, programs);
        s_moveAllPrograms(_fixedFSHPrograms, programs);
        s_moveAllPrograms(_dynamicPrograms, programs);
    }
    for (auto &program : programs) {
        program->invalidate();
    }
}

NS_EASYGL_END
