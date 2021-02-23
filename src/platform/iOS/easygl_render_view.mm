//
//  Created by Pi on 2021/2/20.
//

#import "easygl_render_view.h"
#include "easygl_resource.hpp"
#include "easygl_scope_guard.hpp"

using namespace easy_gl;
EasyGLRenderView::EasyGLRenderView(const Context::Ptr &ctx)
: RendererBase(ctx), _width(0), _height(0)
{
}

EasyGLRenderView::~EasyGLRenderView()
{
    invalidateRenderBuf();
}

void EasyGLRenderView::attachLayer(CAEAGLLayer *layer)
{
    _attachLayer = layer;
    _attachLayer.opaque = NO;
    _attachLayer.drawableProperties = @{
        kEAGLDrawablePropertyRetainedBacking: @(YES),
        kEAGLDrawablePropertyColorFormat: kEAGLColorFormatRGBA8
    };
}

void EasyGLRenderView::setSize(GLsizei width, GLsizei height)
{
    _width = width;
    _height = height;
}

bool EasyGLRenderView::setupRenderBuf()
{
    EASYGL_AUTO_LOCK(_renderMutex);
    if (_colorRenderBuf > 0) { return true; }
    EASYGL_ASSERT(_attachLayer != nil, "MUST ATTACH to layer before setup!");
    if (!_attachLayer) { return false; }
    
    EASYGL_CHECK_ERROR(glGenRenderbuffers(1, &_colorRenderBuf));
    EASYGL_ASSERT(_colorRenderBuf > 0, "generate color render buffer fail!");
    if (_colorRenderBuf == 0) { return false; }
    
    EASYGL_ON_SCOPE_EXIT_WITH_NAME(rollback)
    {
        EASYGL_CHECK_ERROR(glDeleteRenderbuffers(1, &_colorRenderBuf));
        _colorRenderBuf = 0;
    };
    EASYGL_CHECK_ERROR(glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBuf));
    
    if (![EAGLContext.currentContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:_attachLayer]) {
        EASYGL_ASSERT(false, "render buffer storage to attach layer fail!");
        return false;
    }
    EASYGL_CHECK_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _colorRenderBuf));
    rollback.dismiss();
    return true;
}

void EasyGLRenderView::invalidateRenderBuf()
{
    EASYGL_AUTO_LOCK(_renderMutex);
    if (_colorRenderBuf > 0)
    {
        _ctx->resources()->tryDelete(Resources::RBO, _colorRenderBuf);
        _colorRenderBuf = 0;
    }
}

void EasyGLRenderView::onBeforeRender()
{
    EASYGL_CHECK_ERROR(glViewport(0, 0, _width, _height));
    EASYGL_CHECK_ERROR(glClearColor(0, 0, 0, 1));
    EASYGL_CHECK_ERROR(glClear(GL_COLOR_BUFFER_BIT));
}


void EasyGLRenderView::onAfterRender()
{
    EASYGL_CHECK_ERROR(glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBuf));
    [EAGLContext.currentContext presentRenderbuffer:GL_RENDERBUFFER];
}
