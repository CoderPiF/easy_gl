//
//  Created by Pi on 2021/2/18.
//

#include "easygl_context.hpp"
#include "easygl_scope_guard.hpp"
#include "easygl_resource.hpp"
#include <atomic>
#include <map>

NS_EASYGL_BEGIN

thread_local std::weak_ptr<Context> tls_currentCtx;
class ContextMgrImpl final
{
public:
    using ID = Context::ID;
    using RunTaskInGLContext = ContextMgr::RunTaskInGLContext;
    
    static Context::Ptr CreateCtx()
    {
        return Context::Ptr(new Context(++s_nextId));
    }
    
    static void DestoryCtx(Context::Ptr ctx, RunTaskInGLContext runTaskInGL)
    {
        if (!ctx) { return; }
        
        auto oldCur = tls_currentCtx.lock();
        runTaskInGL([&]{
            if (SetCurrentCtx(ctx)) {
                ctx->resources()->clearAll();
            }
        });
        if (oldCur.get() == ctx.get()) {
            SetCurrentCtx(nullptr);
        } else {
            SetCurrentCtx(oldCur);
        }
    }
    
    static Context::Ptr CurrentCtx()
    {
        return tls_currentCtx.lock();
    }
    
private:
    friend class ContextMgr;
    static bool SetCurrentCtx(Context::Ptr ctx)
    {
        auto oldCur = tls_currentCtx.lock();
        if (oldCur)
        {
            if (ctx.get() == oldCur.get())
            {
                return true;
            }
            
            bool changeSuccess = oldCur->setIsCurrent(false);
            assert(changeSuccess);
            tls_currentCtx.reset();
        }
        EASYGL_ON_SCOPE_EXIT_WITH_NAME(rollback)
        {
            if (oldCur)
            {
                oldCur->setIsCurrent(true);
            }
            tls_currentCtx = oldCur;
        };
        
        if (ctx && !ctx->setIsCurrent(true))
        {
            return false;
        }
        
        tls_currentCtx = ctx;
        rollback.dismiss();
        if (ctx)
        {
            ctx->resources()->tryClearDeleteQueue();
        }
        return true;
    }
    
private:
    static std::atomic<ID> s_nextId;
};
std::atomic<Context::ID> ContextMgrImpl::s_nextId(0);

Context::Ptr ContextMgr::CreateCtx() { return ContextMgrImpl::CreateCtx(); }
void ContextMgr::DestoryCtx(Context::Ptr ctx, RunTaskInGLContext runTaskInGL) { ContextMgrImpl::DestoryCtx(ctx, runTaskInGL); }

Context::Ptr ContextMgr::CurrentCtx() { return ContextMgrImpl::CurrentCtx(); }
bool ContextMgr::SetCurrentCtx(Context::Ptr ctx)
{
    return ContextMgrImpl::SetCurrentCtx(ctx);
}

Context::Context(ID cid)
: _cid(cid), _resources(Resources::Create(cid))
{
}

Context::~Context()
{
    if (isCurrent())
    {
        _resources->clearAll();
        ContextMgr::SetCurrentCtx(nullptr);
    }
}

bool Context::setIsCurrent(bool isCur)
{
    EASYGL_AUTO_LOCK(_mutex);
    if (isCurrent() == isCur) {
        return true;
    }
    
    if (_isCurrent) {
        bool isCurThread = (_tid == std::this_thread::get_id());
        EASYGL_ASSERT(isCurThread, "can not change other thread context!");
        if (!isCurThread) {
            return false;
        }
        assert(!isCur);
        _isCurrent = false;
        return true;
    }
    
    assert(isCur);
    _isCurrent = true;
    _tid = std::this_thread::get_id();
    return true;
}
bool Context::isCurrent() const
{
    EASYGL_AUTO_LOCK(_mutex);
    return (_isCurrent && _tid == std::this_thread::get_id());
}
Context::TID Context::threadId() const
{
    EASYGL_AUTO_LOCK(_mutex);
    return _tid;
}
Context::ID Context::ctxId() const { return _cid; }
Resources::Ptr Context::resources() const { return _resources; }

NS_EASYGL_END
