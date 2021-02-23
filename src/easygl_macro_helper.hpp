//
//  Created by Pi on 2021/2/9.
//

#ifndef easygl_macro_helper_h
#define easygl_macro_helper_h

#define EASYGL_DEBUG

// namespace
#define NS_EASYGL_BEGIN namespace easy_gl {
#define NS_EASYGL_END   }

// endian
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define EASYGL_LITTLE_ENDIAN
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define EASYGL_BIG_ENDIAN
#else
#error unkown endian or unsupported compiler!
#endif

// assert
#ifdef EASYGL_DEBUG
#include <iostream>
#define EASYGL_ASSERT(_expr, _msg) \
if (!(_expr)) { \
    std::cerr << "Assert failed: " << _msg << "\n" \
        << "Expected: " #_expr "\n" \
        << "Source: " << __FILE__ << ":" << __LINE__ \
        << std::endl; \
    abort(); \
}
#else
#define EASYGL_ASSERT(_expr, _msg)
#endif

// DISABLE COPY
#define EASYGL_DISABLE_COPY(_c) \
_c(const _c&) = delete; \
const _c& operator=(const _c&) = delete;

// Cat Macro
#define EASYGL_CAT_MACRO(x, y) _EASYGL_CAT_MACRO(x, y)
#define _EASYGL_CAT_MACRO(x, y) x##y

// auto lock
#define EASYGL_AUTO_LOCK(_m) EASYGL_AUTO_LOCK_(_m, EASYGL_CAT_MACRO(lock_, __LINE__))
#define EASYGL_AUTO_FIXLOCK(_m) EASYGL_AUTO_LOCK_(_m, lock)

#define EASYGL_AUTO_LOCK_(_m, _n) \
std::unique_lock<decltype(_m)> _n(_m)

// VA ARGS
#define EASYGL_VA_ARGS_N(\
_1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
_11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
_21, _22, _23, _24, _25, _26, _27, _28, _29, \
_30, N,...) N
#define EASYGL_VA_ARGS_RSEQ_N() \
29,28,27,26,25,24,23,22,21,20, \
19,18,17,16,15,14,13,12,11,10, \
9,8,7,6,5,4,3,2,1,0

#define EASYGL_VA_ARGS_LENGTH(...) _EASYGL_VA_ARGS_LENGTH(, ##__VA_ARGS__, EASYGL_VA_ARGS_RSEQ_N())
#define _EASYGL_VA_ARGS_LENGTH(...) EASYGL_VA_ARGS_N(__VA_ARGS__)

// expand Args
#define _EASYGL_EXPAND_ARGS_0(_macro)
#define _EASYGL_EXPAND_ARGS_1(_macro, _arg) _macro(_arg)
#define _EASYGL_EXPAND_ARGS_2(_macro, _arg1, _arg2) _EASYGL_EXPAND_ARGS_1(_macro, _arg1) _EASYGL_EXPAND_ARGS_1(_macro, _arg2)

#define _EASYGL_EXPAND_ARGS_3(_macro, _arg, ...) _EASYGL_EXPAND_ARGS_1(_macro, _arg) _EASYGL_EXPAND_ARGS_2(_macro, __VA_ARGS__)
#define _EASYGL_EXPAND_ARGS_4(_macro, _arg, ...) _EASYGL_EXPAND_ARGS_1(_macro, _arg) _EASYGL_EXPAND_ARGS_3(_macro, __VA_ARGS__)
#define _EASYGL_EXPAND_ARGS_5(_macro, _arg, ...) _EASYGL_EXPAND_ARGS_1(_macro, _arg) _EASYGL_EXPAND_ARGS_4(_macro, __VA_ARGS__)
#define _EASYGL_EXPAND_ARGS_6(_macro, _arg, ...) _EASYGL_EXPAND_ARGS_1(_macro, _arg) _EASYGL_EXPAND_ARGS_5(_macro, __VA_ARGS__)
#define _EASYGL_EXPAND_ARGS_7(_macro, _arg, ...) _EASYGL_EXPAND_ARGS_1(_macro, _arg) _EASYGL_EXPAND_ARGS_6(_macro, __VA_ARGS__)
#define _EASYGL_EXPAND_ARGS_8(_macro, _arg, ...) _EASYGL_EXPAND_ARGS_1(_macro, _arg) _EASYGL_EXPAND_ARGS_7(_macro, __VA_ARGS__)
#define _EASYGL_EXPAND_ARGS_9(_macro, _arg, ...) _EASYGL_EXPAND_ARGS_1(_macro, _arg) _EASYGL_EXPAND_ARGS_8(_macro, __VA_ARGS__)
#define _EASYGL_EXPAND_ARGS_10(_macro, _arg, ...) _EASYGL_EXPAND_ARGS_1(_macro, _arg) _EASYGL_EXPAND_ARGS_9(_macro, __VA_ARGS__)

#define EASYGL_EXPAND_ARGS(_macro, ...) EASYGL_CAT_MACRO(_EASYGL_EXPAND_ARGS_, EASYGL_VA_ARGS_LENGTH(__VA_ARGS__))(_macro, ##__VA_ARGS__)

#endif /* easygl_macro_helper_h */
