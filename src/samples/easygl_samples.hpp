//
//  Created by Pi on 2021/2/10.
//

#ifndef easygl_samples_h
#define easygl_samples_h

#include "easygl_vertex.hpp"
#include "easygl_uniform.hpp"
#include "easygl_shader.hpp"
#include "easygl_program.hpp"
#include "easygl_shape.hpp"
#include "easygl_customtypes.hpp"

NS_EASYGL_BEGIN

// model
DEF_VERTEX_BEGIN(PositionVertexAttr)
DEF_VERTEX(glm::vec3, pos);
DEF_VERTEX_END
DEF_VERTEX_DESCRIPTOR(PositionVertexAttr, pos);

DEF_VERTEX_BEGIN(PositionRGBVertexAttr)
DEF_VERTEX(glm::vec3, pos);
DEF_VERTEX(RGB, color);
DEF_VERTEX_END
DEF_VERTEX_DESCRIPTOR(PositionRGBVertexAttr, pos, color);

DEF_UNIFORM_BEGIN(RGBUniform)
DEF_UNIFORM(RGB, color);
DEF_UNIFORM_END
DEF_UNIFORM_DESCRIPTOR(RGBUniform, color);

// shapes
// 固定颜色shape
using PositionVSH = VertexShader<PositionVertexAttr>;
using RGBFSH = FragmentShader<RGBUniform>;
using UniformRGBProgram = ShaderProgramBase<PositionVSH, RGBFSH>;
using UniformRGBShape = ShapeBase<UniformRGBProgram>;

// 不固定颜色shape
using PositionRGBVSH = VertexShader<PositionRGBVertexAttr>;
using NoUniformFSH = FragmentShader<EmptyUniform>;
using VaryingRGBProgram = ShaderProgramBase<PositionRGBVSH, NoUniformFSH>;
using VaryingRGBShape = ShapeBase<VaryingRGBProgram>;

NS_EASYGL_END

#endif /* easygl_samples_h */
