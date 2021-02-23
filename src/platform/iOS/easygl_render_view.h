//
//  Created by Pi on 2021/2/20.
//

#import <QuartzCore/QuartzCore.h>
#include <atomic>
#include "easygl_renderer.hpp"

class EasyGLRenderView final : public easy_gl::RendererBase
{
public:
    EasyGLRenderView(const easy_gl::Context::Ptr &ctx);
    ~EasyGLRenderView();
    
    void setSize(GLsizei width, GLsizei height);
    void attachLayer(CAEAGLLayer *layer);
    
private:
    bool setupRenderBuf() override;
    void invalidateRenderBuf() override;
    void onBeforeRender() override;
    void onAfterRender() override;

    CAEAGLLayer *_attachLayer = nil;
    GLuint _colorRenderBuf = 0;
    std::atomic<GLsizei> _width;
    std::atomic<GLsizei> _height;
};
