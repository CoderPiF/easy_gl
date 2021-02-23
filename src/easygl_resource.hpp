//
//  Created by Pi on 2021/2/18.
//

#ifndef easygl_resource_hpp
#define easygl_resource_hpp

#include <memory>
#include "easygl_context.hpp"

NS_EASYGL_BEGIN

class ShaderSharedMgr;
class ProgramSharedMgr;
class Resources
{
    EASYGL_DISABLE_COPY(Resources);
public:
    using Ptr = std::shared_ptr<Resources>;
    enum Type
    {
        VAO, VBO, Texture, FBO, RBO, Shader, Program, MAX
    };
    
    virtual bool tryDelete(Type type, GLuint target) = 0;
    virtual bool tryClearDeleteQueue() = 0;
    virtual bool clearAll() = 0;
    virtual std::shared_ptr<ShaderSharedMgr> shaderMgr() = 0;
    virtual std::shared_ptr<ProgramSharedMgr> programMgr() = 0;
    
    virtual ~Resources();
    Resources(Context::ID);
    const Context::ID ctxId;
    
private:
    friend class Context;
    static Ptr Create(Context::ID);
};

NS_EASYGL_END

#endif /* easygl_resource_hpp */
