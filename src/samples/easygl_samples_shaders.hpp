//
//  Created by Pi on 2021/2/20.
//

#ifndef easygl_samples_shapes_h
#define easygl_samples_shapes_h

#include <string>

#define NS_EASYGL_SAMPLES_BEGIN namespace easy_gl { namespace samples {
#define NS_EASYGL_SAMPLES_END   } }

NS_EASYGL_SAMPLES_BEGIN

// 固定颜色
constexpr char PositionVSHSrc[] = R"_SH_(
#version 100
precision highp float;
attribute vec3 pos;
void main()
{
    gl_Position = vec4(pos, 1.0);
}
)_SH_";

constexpr char UniformRGBFSHSrc[] = R"_SH_(
#version 100
precision highp float;
uniform vec3 color;
void main()
{
    gl_FragColor = vec4(color, 1.0);
}
)_SH_";

// 不固定颜色
constexpr char PositionRGBVSHSrc[] = R"_SH_(
#version 100
precision highp float;
attribute vec3 pos;
attribute vec3 color;
varying vec3 Color;
void main()
{
    Color = color;
    gl_Position = vec4(pos, 1.0);
}
)_SH_";

constexpr char VaryingRGBFSHSrc[] = R"_SH_(
#version 100
precision highp float;
varying vec3 Color;
void main()
{
    gl_FragColor = vec4(Color, 1.0);
}
)_SH_";

NS_EASYGL_SAMPLES_END

#undef NS_EASYGL_SAMPLES_BEGIN
#undef NS_EASYGL_SAMPLES_END

#endif /* easygl_samples_shapes_h */
