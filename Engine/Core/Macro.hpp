#pragma once
#include <print>
#include "Profiler/ChromeTraceProfiler.hpp"
#include "Timer.hpp"

#define ENABLE_LOGGING 1
#define ENABLE_WARNING 1
#define ENABLE_ERROR 1
#define ENABLE_FATAL 1

#if !defined(_MSC_VER) && defined(__clang__)
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
#else
	#if ENABLE_LOGGING
	#define LOG(x, ...) std::println("{}: " x, "log",  ##__VA_ARGS__)
	#endif

	#if ENABLE_WARNING
	#define WARN(x, ...) std::println("{}: " x, "warn",  ##__VA_ARGS__)
	#endif

	#if ENABLE_ERROR
	#define ERROR(x, ...) std::println("{}: " x, "error",  ##__VA_ARGS__)
	#endif

	#if ENABLE_FATAL
	#define FATAL(x, ...) std::println("{}: " x, "fatal",  ##__VA_ARGS__)
	#endif
#endif

#if defined(_MSC_VER)
#define __FNNAME__ __FUNCSIG__
#elif defined(__clang__) || defined(__GNUC__)
#define __FNNAME__ __PRETTY_FUNCTION__
#endif

#define PROFILE_FUNCTION() ScopedTimer __profilingScopedTimer__(__FNNAME__)


#define CHROME_TRACE_ENABLED 1


#if CHROME_TRACE_ENABLED

inline ChromeTraceProfiler __global_profiler__("profile.json");

#define CHROME_TRACE_FUNCTION() ChromeFunctionTrace __function_trace__ = __global_profiler__.ProfileFunction(__FNNAME__, 0, 0)

#endif