//
//  Created by Pi on 2021/2/20.
//

#ifndef easygl_samples_shapes_h
#define easygl_samples_shapes_h

#include <string>
#include "easygl_utils.hpp"

#define NS_EASYGL_SAMPLES_BEGIN namespace easy_gl { namespace samples {
#define NS_EASYGL_SAMPLES_END   } }

NS_EASYGL_SAMPLES_BEGIN

// 固定颜色
constexpr char PositionVSHSrc[] =
EASYGL_SHADER_HEADER
R"_SH_(
precision highp float;
in vec3 pos;
void main()
{
    gl_Position = vec4(pos, 1.0);
}
)_SH_";

constexpr char UniformRGBFSHSrc[] =
EASYGL_SHADER_HEADER
R"_SH_(
precision highp float;
uniform vec3 color;
out vec4 FragColor;
void main()
{
    FragColor = vec4(color, 1.0);
}
)_SH_";

// 不固定颜色
constexpr char PositionRGBVSHSrc[] =
EASYGL_SHADER_HEADER
R"_SH_(
precision highp float;
in vec3 pos;
in vec3 color;
out vec3 Color;
void main()
{
    Color = color;
    gl_Position = vec4(pos, 1.0);
}
)_SH_";

constexpr char VaryingRGBFSHSrc[] =
EASYGL_SHADER_HEADER
R"_SH_(
precision highp float;
in vec3 Color;
out vec4 FragColor;
void main()
{
    FragColor = vec4(Color, 1.0);
}
)_SH_";

// Texture
constexpr char TexCoordVSHSrc[] =
EASYGL_SHADER_HEADER
R"_SH_(
precision highp float;
in vec2 pos;
in vec2 texCoord;
out vec2 TexCoord;

void main()
{
    TexCoord = texCoord;
    gl_Position = vec4(pos, 0.0, 1.0);
}
)_SH_";

constexpr char TextureFSHSrc[]=
EASYGL_SHADER_HEADER
R"_SH_(
precision highp float;
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoord);
}
)_SH_";

NS_EASYGL_SAMPLES_END

#undef NS_EASYGL_SAMPLES_BEGIN
#undef NS_EASYGL_SAMPLES_END

#endif /* easygl_samples_shapes_h */
