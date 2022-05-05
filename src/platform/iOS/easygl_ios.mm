//
//  Created by Pi on 2021/2/20.
//

#include "easygl_ios.h"
#import <OpenGLES/EAGL.h>

class EasyGLIOSImp final
{
public:
    using Task = EasyGLIOS::Task;
    EasyGLIOSImp()
    {
        _eglCtx = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    }
    
    ~EasyGLIOSImp()
    {
        _eglCtx = nil;
    }
    
    void makeGLCtxCurrent()
    {
        if (_eglCtx != EAGLContext.currentContext) {
            EAGLContext.currentContext = _eglCtx;
        }
    }
    
    void runTaskInGLCtx(Task task)
    {
        if (EAGLContext.currentContext == _eglCtx) {
            task();
            return;
        }
        
        EAGLContext *curEGLCtx = EAGLContext.currentContext;
        EAGLContext.currentContext = _eglCtx;
        task();
        EAGLContext.currentContext = curEGLCtx;
    }
    
private:
    EAGLContext *_eglCtx = nil;
};


EasyGLIOS::EasyGLIOS() : _impl(new EasyGLIOSImp) {}
EasyGLIOS::~EasyGLIOS()
{
    if (_impl) {
        delete _impl;
        _impl = nullptr;
    }
}

void EasyGLIOS::makeGLCtxCurrent() { _impl->makeGLCtxCurrent(); }
void EasyGLIOS::runTaskInGLCtx(Task task) { _impl->runTaskInGLCtx(task); }
