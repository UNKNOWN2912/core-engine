#pragma once

#include <print>

#define ENABLE_LOGGING 1
#define ENABLE_WARNING 1
#define ENABLE_ERROR 1
#define ENABLE_FATAL 1

#if ENABLE_LOGGING
#define LOG(x, ...) std::println("{}: " x, "log"  __VA_OPT__(,)  __VA_ARGS__)
#endif

#if ENABLE_WARNING
#define WARN(x, ...) std::println("{}: " x, "warn"  __VA_OPT__(,)  __VA_ARGS__)
#endif

#if ENABLE_ERROR
#define ERROR(x, ...) std::println("{}: " x, "error"  __VA_OPT__(,)  __VA_ARGS__)
#endif

#if ENABLE_FATAL
#define FATAL(x, ...) std::println("{}: " x, "fatal"  __VA_OPT__(,)  __VA_ARGS__)
#endif
