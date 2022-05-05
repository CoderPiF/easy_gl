//
//  easygl_customtypes.hpp
//  EasyGLDemo
//
//  Created by 江派锋 on 2021/2/23.
//

#ifndef easygl_customtypes_h
#define easygl_customtypes_h

#include "easygl_utils.hpp"

NS_EASYGL_BEGIN

struct ARGB
{
    ARGB() : argb(0) {}
    ARGB(uint8_t a, uint8_t r, uint8_t g, uint8_t b) : a(a), r(r), g(g), b(b) {}
    ARGB(uint32_t argb) : argb(argb) {}
    union
    {
#ifdef EASYGL_LITTLE_ENDIAN
        struct
        {
            uint8_t b;
            uint8_t g;
            uint8_t r;
            uint8_t a;
        };
#else // big Endian
        struct
        {
            uint8_t a;
            uint8_t r;
            uint8_t g;
            uint8_t b;
        };
#endif // Endian
        uint32_t argb = 0;
    };
    
    using real_type = glm::vec4;
    operator real_type() const
    {
        return glm::vec4(r/255.0, g/255.0, b/255.0, a/255.0);
    }
};

struct RGB
{
    RGB() : xrgb(0) {}
    RGB(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b), x(0) {}
    RGB(uint32_t rgb) : xrgb(rgb) {}
    union
    {
#ifdef EASYGL_LITTLE_ENDIAN
        struct
        {
            uint8_t b;
            uint8_t g;
            uint8_t r;
            uint8_t x;
        };
#else // big Endian
        struct
        {
            uint8_t x;
            uint8_t r;
            uint8_t g;
            uint8_t b;
        };
#endif // Endian
        uint32_t xrgb = 0;
    };
    
    using real_type = glm::vec3;
    operator real_type() const
    {
        return real_type(r/255.0, g/255.0, b/255.0);
    }
};

NS_EASYGL_END

#endif /* easygl_customtypes_h */
