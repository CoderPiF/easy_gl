//
//  Created by Pi on 2021/2/18.
//

#ifndef easygl_shape_h
#define easygl_shape_h

#include "easygl_utils.hpp"
#include "easygl_context.hpp"
#include "easygl_scope_guard.hpp"
#include "easygl_program.hpp"
#include <memory>
#include <mutex>

NS_EASYGL_BEGIN

class Shape
{
    EASYGL_DISABLE_COPY(Shape);
public:
    using Ptr = std::shared_ptr<Shape>;
    
    bool draw();
    void invalidate();
    void release();

    virtual ~Shape();
    
protected:
    Shape(const ShaderProgram::Ptr &program, GLenum mode);
    void updateVertexs(size_t vertexCount, GLenum usage = GL_DYNAMIC_DRAW);
    virtual void resetVertexs(size_t count) = 0;
    virtual const GLvoid * vertexsData() const = 0;
    virtual size_t vertexSize() const = 0;
    virtual const uniform_data & uniformData() const = 0;
    
protected:
    std::recursive_mutex _mutex;
    const GLenum _shapeMode;
    Context::Ptr _ctx;
    ShaderProgram::Ptr _program;
    bool _isVertexsDirty = false;
    GLenum _vertexUsage = GL_DYNAMIC_DRAW;
    size_t _vertexCount = 0;
    GLuint _vao = 0;
    GLuint _vbo = 0;
};

template<typename ProgramType>
class ShapeBase : public Shape
{
public:
    using program_type = ProgramType;
    using program_ptr = std::shared_ptr<program_type>;
    using vertex_type = typename program_type::vertex_type;
    using uniform_type = typename program_type::uniform_type;
    
    ShapeBase(const program_ptr &program, GLenum mode)
    : Shape(std::static_pointer_cast<ShaderProgram>(program), mode)
    {}
    
    void updateVertexs(size_t vertexCount, std::function<void(vertex_type *)> updater, GLenum usage = GL_DYNAMIC_DRAW)
    {
        EASYGL_AUTO_LOCK(_mutex);
        Shape::updateVertexs(vertexCount, usage);
        if (vertexCount > 0) {
            updater(_vertexs.get());
            _isVertexsDirty = true;
        }
    }
    
    void updateUniforms(std::function<void(uniform_type *)> updater)
    {
        EASYGL_AUTO_LOCK(_mutex);
        updater(&_uniforms);
    }

    virtual ~ShapeBase() {}
protected:
    void resetVertexs(size_t count) override
    {
        EASYGL_AUTO_LOCK(_mutex);
        if (count == 0) {
            _vertexs.reset();
        } else {
            _vertexs.reset(new vertex_type[count]);
        }
    }
    const GLvoid * vertexsData() const override { return static_cast<const GLvoid *>(_vertexs.get()); }
    size_t vertexSize() const override { return sizeof(vertex_type); }
    const uniform_data & uniformData() const override { return _uniforms.getData(); }

    std::unique_ptr<vertex_type[]> _vertexs;
    uniform_type _uniforms;
};

NS_EASYGL_END

#endif /* easygl_shape_h */
