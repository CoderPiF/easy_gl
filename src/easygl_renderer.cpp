//
//  Created by Pi on 2021/2/19.
//

#include "easygl_renderer.hpp"
#include "easygl_scope_guard.hpp"
#include "easygl_shape.hpp"

NS_EASYGL_BEGIN

Renderer::~Renderer() {}
Renderer::Renderer(const Context::Ptr &ctx) : _ctx(ctx) {}

RendererBase::RendererBase(const Context::Ptr &ctx) : Renderer(ctx) {}
RendererBase::~RendererBase()
{
    invalidateFBO();
}

void RendererBase::invalidateFBO()
{
    EASYGL_AUTO_LOCK(_renderMutex);
    if (_fbo > 0)
    {
        _ctx->resources()->tryDelete(Resources::FBO, _fbo);
        _fbo = 0;
    }
}

void RendererBase::invalidate()
{
    EASYGL_AUTO_LOCK(_renderMutex);
    invalidateRenderBuf();
    invalidateFBO();
}

bool RendererBase::setup()
{
    EASYGL_AUTO_LOCK(_renderMutex);
    if (_fbo > 0) { return true; }

    EASYGL_CHECK_ERROR(glGenFramebuffers(1, &_fbo));
    EASYGL_ASSERT(_fbo > 0, "generate fbo fail!");
    if (_fbo == 0) { return false; }
    EASYGL_ON_SCOPE_EXIT_WITH_NAME(rollback)
    {
        EASYGL_CHECK_ERROR(glDeleteFramebuffers(1, &_fbo));
        _fbo = 0;
    };
    
    EASYGL_CHECK_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, _fbo));
    
    bool isSetupRBOSuccess = setupRenderBuf();
    EASYGL_ASSERT(isSetupRBOSuccess, "setup render buffer fail");
    if (!isSetupRBOSuccess) { return false; }
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    EASYGL_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "fbo complete fail");
    if (status != GL_FRAMEBUFFER_COMPLETE) { return false; }

    rollback.dismiss();
    return true;
}

void RendererBase::onBeforeRender() {}
void RendererBase::onAfterRender() {}

void RendererBase::render()
{
    EASYGL_CHECK_CURCTX_DESCR("ready to render");
    if (!_ctx->isCurrent()) { return; }

    std::vector<ShapePtr> targetShapes;
    {
        EASYGL_AUTO_LOCK(_mutex);
        targetShapes = _shapes;
    }
    if (targetShapes.empty()) { return; }

    EASYGL_AUTO_LOCK(_renderMutex);
    if (!setup()) {
        EASYGL_ASSERT(false, "setup renderer error");
        return;
    }

    EASYGL_CHECK_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, _fbo));
    onBeforeRender();
    EASYGL_ON_SCOPE_EXIT
    {
        onAfterRender();
        EASYGL_CHECK_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    };
    for (auto &shape : targetShapes) {
        shape->draw();
    }
}

void RendererBase::addShape(const ShapePtr &shape)
{
    EASYGL_AUTO_LOCK(_mutex);
    _shapes.push_back(shape);
}

NS_EASYGL_END
