//
//  Created by Pi on 2021/2/19.
//

#ifndef easygl_hpp
#define easygl_hpp

#include "easygl_utils.hpp"
#include <memory>
#include <vector>
#include <mutex>
#include <functional>

NS_EASYGL_BEGIN

class Renderer;
class Context;
class EasyGL
{
    EASYGL_DISABLE_COPY(EasyGL);
public:
    using Ptr = std::shared_ptr<EasyGL>;
    using ContextPtr = std::shared_ptr<Context>;
    using RendererPtr = std::shared_ptr<Renderer>;
    using RendererList = std::vector<RendererPtr>;
    
    bool run();
    const ContextPtr & context() const;
    void addRenderer(const RendererPtr &renderer);
    void releaseResources();
    void releaseRenderers();
    
    virtual ~EasyGL();
protected:
    using Task = std::function<void(void)>;
    EasyGL();
    RendererList targetRenderers();
    bool makeCtxCurrent();
    virtual void makeGLCtxCurrent() = 0;
    virtual void runTaskInGLCtx(Task task) = 0;
    
    std::mutex _mutex;
    ContextPtr _ctx;
    RendererList _renderers;
};

NS_EASYGL_END

#endif /* easygl_hpp */
