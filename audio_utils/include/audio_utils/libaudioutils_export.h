#pragma once

#if defined(WIN32) || defined(_MSC_VER)

#if defined(LIBAUDIOUTILS_IMPLEMENTATION)
#define LIBAUDIOUTILS_EXPORT __declspec(dllexport)
#else
#define LIBAUDIOUTILS_EXPORT __declspec(dllimport)
#endif  // defined(LIBAUDIOUTILS_IMPLEMENTATION)

#else  // defined(WIN32)
#if defined(LIBAUDIOUTILS_IMPLEMENTATION)
#define LIBAUDIOUTILS_EXPORT __attribute__((visibility("default")))
#else
#define LIBAUDIOUTILS_EXPORT
#endif  // defined(LIBAUDIOUTILS_IMPLEMENTATION)
#endif
