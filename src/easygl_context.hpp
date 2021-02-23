//
//  Created by Pi on 2021/2/18.
//

#ifndef easygl_context_h
#define easygl_context_h

#include "easygl_utils.hpp"
#include <thread>
#include <memory>
#include <functional>

NS_EASYGL_BEGIN

class Resources;
class Context final : public std::enable_shared_from_this<Context>
{
    EASYGL_DISABLE_COPY(Context);
public:
    using Ptr = std::shared_ptr<Context>;
    using ID = size_t;
    using TID = std::thread::id;
    using ResourcesPtr = std::shared_ptr<Resources>;
    
    bool isCurrent() const;
    ID ctxId() const;
    TID threadId() const;
    ResourcesPtr resources() const;
    
    ~Context();
private:
    friend class ContextMgrImpl;
    Context(ID cid);
    bool setIsCurrent(bool isCur);

    mutable std::recursive_mutex _mutex;
    bool _isCurrent = false;
    TID _tid;
    const ID _cid;
    ResourcesPtr _resources;
};

struct ContextMgr final
{
    using Task = std::function<void(void)>;
    using RunTaskInGLContext = std::function<void(Task)>;
    
    static Context::Ptr CreateCtx();
    static void DestoryCtx(Context::Ptr ctx, RunTaskInGLContext runTaskInGL);
    
    static Context::Ptr CurrentCtx();
    static bool SetCurrentCtx(Context::Ptr ctx);
};

NS_EASYGL_END

#define EASYGL_CHECK_CURCTX_(_x, _s) \
do { \
    EASYGL_ASSERT(_ctx && _ctx->isCurrent(), "context is not current : " _s); \
    _x; \
} while(0)
#define EASYGL_CHECK_CURCTX(_x) EASYGL_CHECK_CURCTX_(_x, #_x)
#define EASYGL_CHECK_CURCTX_DESCR(_s) EASYGL_CHECK_CURCTX_(, _s)


#endif /* easygl_context_h */
