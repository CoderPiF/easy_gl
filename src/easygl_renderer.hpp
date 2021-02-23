//
//  Created by Pi on 2021/2/19.
//

#ifndef easygl_renderer_hpp
#define easygl_renderer_hpp

#include "easygl_utils.hpp"
#include "easygl_context.hpp"
#include <memory>
#include <vector>
#include <mutex>

NS_EASYGL_BEGIN

class Renderer
{
    EASYGL_DISABLE_COPY(Renderer);
public:
    using Ptr = std::shared_ptr<Renderer>;
    
    virtual void render() = 0;
    virtual void invalidate() = 0;
    
    virtual ~Renderer();
protected:
    Renderer(const Context::Ptr &);
    Context::Ptr _ctx;
};

class Shape;
class RendererBase : public Renderer
{
public:
    using ShapePtr = std::shared_ptr<Shape>;

    void addShape(const ShapePtr &shape);
    virtual void render() override;
    virtual void invalidate() override;
    
    virtual ~RendererBase();
protected:
    bool setup();
    void invalidateFBO();
    virtual bool setupRenderBuf() = 0;
    virtual void invalidateRenderBuf() = 0;
    virtual void onBeforeRender();
    virtual void onAfterRender();
    RendererBase(const Context::Ptr &);

    std::recursive_mutex _renderMutex;
    GLuint _fbo = 0;
    
    std::mutex _mutex;
    std::vector<ShapePtr> _shapes;
};

NS_EASYGL_END

#endif /* easygl_renderer_hpp */
