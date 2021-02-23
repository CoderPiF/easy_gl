//
//  Created by Pi on 2021/2/19.
//

#include "easygl_shape.hpp"

NS_EASYGL_BEGIN

Shape::Shape(const ShaderProgram::Ptr &program, GLenum mode)
: _program(program), _shapeMode(mode)
{}

Shape::~Shape()
{
    release();
    invalidate();
}

void Shape::updateVertexs(size_t vertexCount, GLenum usage)
{
    EASYGL_AUTO_LOCK(_mutex);
    if (vertexCount == 0) {
        _vertexCount = 0;
        resetVertexs(0);
        _isVertexsDirty = false;
        return;
    }
    
    _vertexUsage = usage;
    if (_vertexCount != vertexCount) {
        _vertexCount = vertexCount;
        resetVertexs(vertexCount);
        _isVertexsDirty = true;
    }
}

bool Shape::draw()
{
    EASYGL_AUTO_LOCK(_mutex);
    EASYGL_ASSERT(_vertexCount > 0, "can not draw with no vertexs");
    if (_vertexCount == 0) { return false; }

    std::string errMsg;
    if (!_program->setup(&errMsg)) {
        EASYGL_ASSERT(false, "setup program error: " + errMsg);
        return false;
    }
    
    if (!_ctx) { _ctx = _program->context(); }

    EASYGL_ASSERT(_ctx && _ctx->isCurrent(), "context is not current when draw");
    if (!_ctx || !_ctx->isCurrent()) { return false; }

    _program->use();
    EASYGL_ON_SCOPE_EXIT
    {
        if (_vao == 0) {
            _program->disableVertexs();
        } else {
            EASYGL_CHECK_ERROR(glBindVertexArray(0));
        }
        EASYGL_CHECK_ERROR(glUseProgram(0));
    };
    
    if (_vao == 0) {
        EASYGL_CHECK_ERROR(glGenVertexArrays(1, &_vao));
        _isVertexsDirty = true;
    }
    glBindVertexArray(_vao);
    
    if (_vbo == 0) {
        EASYGL_CHECK_ERROR(glGenBuffers(1, &_vbo));
        _isVertexsDirty = true;
    }
    if (_isVertexsDirty) {
        EASYGL_CHECK_ERROR(glBindBuffer(GL_ARRAY_BUFFER, _vbo));
        EASYGL_CHECK_ERROR(glBufferData(GL_ARRAY_BUFFER, vertexSize() * _vertexCount, vertexsData(), _vertexUsage));
        _program->syncVertexs();
        _isVertexsDirty = false;
    }
    
    _program->syncUniforms(uniformData());
    if (!_program->validate(&errMsg)) {
        EASYGL_ASSERT(false, "program is invalid : " + errMsg);
        return false;
    }
    
    EASYGL_CHECK_ERROR(glDrawArrays(_shapeMode, 0, static_cast<GLsizei>(_vertexCount)));
    return true;
}

void Shape::release()
{
    EASYGL_AUTO_LOCK(_mutex);
    _vertexCount = 0;
    _isVertexsDirty = false;
    resetVertexs(0);
}

void Shape::invalidate()
{
    EASYGL_AUTO_LOCK(_mutex);
    if (_vao > 0) {
        EASYGL_ASSERT(_ctx, "context invalid but vao has been created!");
        auto hasDelete = _ctx->resources()->tryDelete(Resources::VAO, _vao);
        EASYGL_ASSERT(hasDelete, "delete vao when context is not current");
        _vao = 0;
    }
    if (_vbo > 0) {
        EASYGL_ASSERT(_ctx, "context invalid but vbo has been created!");
        auto hasDelete = _ctx->resources()->tryDelete(Resources::VBO, _vbo);
        EASYGL_ASSERT(hasDelete, "delete vbo when context is not current");
        _vbo = 0;
    }
}

NS_EASYGL_END
