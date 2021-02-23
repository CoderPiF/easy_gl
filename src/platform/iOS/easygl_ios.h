//
//  Created by Pi on 2021/2/20.
//

#include "easygl.hpp"

class EasyGLIOSImp;
class EasyGLIOS final : public easy_gl::EasyGL
{
    EASYGL_DISABLE_COPY(EasyGLIOS);
public:
    using Task = easy_gl::EasyGL::Task;
    EasyGLIOS();
    ~EasyGLIOS();
    
private:
    void makeGLCtxCurrent() override;
    void runTaskInGLCtx(Task task) override;
    
    EasyGLIOSImp *_impl = nullptr;
};
