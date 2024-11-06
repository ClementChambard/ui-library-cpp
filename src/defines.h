#ifndef DEFINES_HEADER_INCLUDED
#define DEFINES_HEADER_INCLUDED

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using i8 = signed char;
using i16 = signed short;
using i32 = signed int;
using i64 = signed long long;

using usize = unsigned long long;
using isize = signed long long;

using f32 = float;
using f64 = double;

using cstr = char const*;
using pstr = char*;
using ptr = void*;
using roptr = const void*;
using byte = u8;
using bytes = byte*;
using robytes = const byte*;
using NSID = u32;

#define AS_BYTES(x) reinterpret_cast<bytes>(x)

#if defined(__clang__) || defined(__gcc__)
#define NS_STATIC_ASSERT _Static_assert
#else
#define NS_STATIC_ASSERT static_assert
#endif

NS_STATIC_ASSERT(sizeof(u8) == 1, "Expected u8 to be 1 byte.");
NS_STATIC_ASSERT(sizeof(u16) == 2, "Expected u16 to be 2 byte.");
NS_STATIC_ASSERT(sizeof(u32) == 4, "Expected u32 to be 4 byte.");
NS_STATIC_ASSERT(sizeof(u64) == 8, "Expected u64 to be 8 byte.");

NS_STATIC_ASSERT(sizeof(i8) == 1, "Expected i8 to be 1 byte.");
NS_STATIC_ASSERT(sizeof(i16) == 2, "Expected i16 to be 2 byte.");
NS_STATIC_ASSERT(sizeof(i32) == 4, "Expected i32 to be 4 byte.");
NS_STATIC_ASSERT(sizeof(i64) == 8, "Expected i64 to be 8 byte.");

NS_STATIC_ASSERT(sizeof(usize) == 8, "Expected usize to be 8 byte.");
NS_STATIC_ASSERT(sizeof(isize) == 8, "Expected isize to be 8 byte.");

NS_STATIC_ASSERT(sizeof(f32) == 4, "Expected f32 to be 4 byte.");
NS_STATIC_ASSERT(sizeof(f64) == 8, "Expected f64 to be 8 byte.");

#define INVALID_ID 4294967295U

#define NS_PLATFORM_WND_SDL2
#define NS_RENDERER_OPENGL

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define NS_PLATFORM_WINDOWS 1
#ifndef _WIN64
#error "64-bit is required on Windows!"
#endif
#elif defined(__linux__) || defined(__gnu_linux__)
#define NS_PLATFORM_LINUX 1
#if defined(__ANDROID__)
#define NS_PLATFORM_ANDROID 1
#endif
#elif defined(__unix__)
#define NS_PLATFORM_UNIX 1
#elif defined(_POSIX_VERSION)
#define NS_PLATFORM_POSIX 1
#elif __APPLE__
#define NS_PLATFORM_APPLE 1
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
#define NS_PLATFORM_IOS 1
#define NS_PLATFORM_IOS_SIMULATOR 1
#elif TARGET_OS_IPHONE
#define NS_PLATFORM_IOS 1
#elif TARGET_OS_MAC
#else
#error "Unknown Apple platform"
#endif
#else
#error "Unknown platform!"
#endif

#ifdef NS_EXPORT
#ifdef _MSC_VER
#define NS_API __declspec(dllexport)
#else
#define NS_API //__attribute__((visibility("default")))
#endif
#else
#ifdef _MSC_VER
#define NS_API __declspec(dllimport)
#else
#define NS_API
#endif
#endif

#ifdef _MSC_VER
#define NS_INLINE __forceinline
#define NS_NOINLINE __declspec(noinline)
#else
#define NS_INLINE static inline
#define NS_NOINLINE
#endif

#define OWNED

#endif // DEFINES_HEADER_INCLUDED
