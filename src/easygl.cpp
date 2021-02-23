//
//  Created by Pi on 2021/2/19.
//

#include "easygl.hpp"
#include "easygl_context.hpp"
#include "easygl_resource.hpp"
#include "easygl_renderer.hpp"

NS_EASYGL_BEGIN

EasyGL::EasyGL()
{
    _ctx = ContextMgr::CreateCtx();
}
EasyGL::~EasyGL()
{
    ContextMgr::DestoryCtx(_ctx, [&](Task task){
        runTaskInGLCtx(task);
    });
}
const EasyGL::ContextPtr & EasyGL::context() const
{
    return _ctx;
}

bool EasyGL::makeCtxCurrent()
{
    if (!ContextMgr::SetCurrentCtx(_ctx)) {
        return false;
    }
    makeGLCtxCurrent();
    return true;
}


bool EasyGL::run()
{
    if (!makeCtxCurrent()) {
        return false;
    }
    
    auto targets = targetRenderers();
    for (auto &renderer : targets) {
        renderer->render();
    }
    
    return true;
}

void EasyGL::addRenderer(const RendererPtr &renderer)
{
    EASYGL_AUTO_LOCK(_mutex);
    _renderers.push_back(renderer);
}

EasyGL::RendererList EasyGL::targetRenderers()
{
    EASYGL_AUTO_LOCK(_mutex);
    return _renderers;
}

void EasyGL::releaseRenderers()
{
    RendererList targets;
    {
        EASYGL_AUTO_LOCK(_mutex);
        targets = _renderers;
        _renderers.clear();
    }
    for (auto &target : targets) {
        target->invalidate();
    }
}

void EasyGL::releaseResources()
{
    _ctx->resources()->clearAll();
}



NS_EASYGL_END
