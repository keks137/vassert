#ifndef INCLUDE_SRC_ASSERTS_H_
#define INCLUDE_SRC_ASSERTS_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef VABORT_DEBUG
#if _MSC_VER
#include <intrin.h>
#define VABORT() __debugBreak()
#else
#define VABORT() __builtin_trap()
#endif

#else

#include <stdlib.h>
#define VABORT() abort()
#endif //VABORT_DEBUG

// for when you want to assert in release builds
#if defined(__GNUC__) || defined(__clang__)
#define VLIKELY(x) __builtin_expect(!!(x), 1)
#define VUNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define VLIKELY(x) (x)
#define VUNLIKELY(x) (x)
#endif

#ifdef _WIN32
#ifndef VNO_WINDOWS_DEFAULTS
#define VNO_COLORS
#include <windows.h>
static inline void VWinMessageBox(DWORD type, const char *title, const char *prefix,
				  const char *file, int line, const char *func,
				  const char *fmt, ...)
{
	char message[4096];
	char formatted[2048];

	va_list args;
	va_start(args, fmt);
	vsnprintf(formatted, sizeof(formatted), fmt, args);
	va_end(args);

	snprintf(message, sizeof(message),
		 "%s:%s:%d:%s\n\n%s",
		 prefix, file, line, func, formatted);

	MessageBoxA(NULL, message, title, type | MB_SETFOREGROUND | MB_TOPMOST);
}
#define VLOGHANDLER_FATAL(fmt, ...)                              \
	VWinMessageBox(MB_ICONERROR | MB_OK, "Fatal Error", "F", \
		       __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define VLOGHANDLER_ERROR(fmt, ...)                        \
	VWinMessageBox(MB_ICONERROR | MB_OK, "Error", "E", \
		       __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#endif // VNO_WINDOWS_DEFAULTS
#endif // _WIN32

#ifndef VNO_COLORS
#define VFATAL_PREFIX "\033[35m[F"
#define VERROR_PREFIX "\033[31m[E"
#define VWARN_PREFIX "\033[33m[W"
#define VINFO_PREFIX "\033[36m[I"
#define VCODE_LOCATION ":%s:%d:%s]"
#define VCOLOR_TERMINATION "\033[0m "

#else
#define VFATAL_PREFIX "[F"
#define VWARN_PREFIX "[W"
#define VERROR_PREFIX "[E"
#define VINFO_PREFIX "[I"
#define VCODE_LOCATION ":%s:%d:%s] "
#define VCOLOR_TERMINATION " "
#endif // VNO_COLORS

#ifdef __ANDROID__
#ifndef VLOG_TAG
#define VLOG_TAG "Vassert"
#endif // VLOG_TAG
#include <android/log.h>

#ifndef VLOGHANDLER_FATAL
#define VLOGHANDLER_FATAL(fmt, ...) __android_log_print(ANDROID_LOG_FATAL, VLOG_TAG, fmt, ##__VA_ARGS__)
#endif // VLOGHANDLER_FATAL
#ifndef VLOGHANDLER_WARN
#define VLOGHANDLER_WARN(fmt, ...) __android_log_print(ANDROID_LOG_WARN, VLOG_TAG, fmt, ##__VA_ARGS__)
#endif // VLOGHANDLER_WARN
#ifndef VLOGHANDLER_ERROR
#define VLOGHANDLER_ERROR(fmt, ...) __android_log_print(ANDROID_LOG_ERROR, VLOG_TAG, fmt, ##__VA_ARGS__)
#endif // VLOGHANDLER_ERROR

#else
#include <stdio.h>
#ifndef VLOGHANDLER_FATAL
#define VLOGHANDLER_FATAL(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#endif // VLOGHANDLER_FATAL
#ifndef VLOGHANDLER_ERROR
#define VLOGHANDLER_ERROR(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#endif //VLOGHANDLER_ERROR
#ifndef VLOGHANDLER_WARN
#define VLOGHANDLER_WARN(fmt, ...) fprintf(stdout, fmt, ##__VA_ARGS__)
#endif // VLOGHANDLER_WARN
#ifndef VLOGHANDLER_INFO
#define VLOGHANDLER_INFO(fmt, ...) fprintf(stdout, fmt, ##__VA_ARGS__)
#endif // VLOGHANDLER_INFO
#endif // __ANDROID__

#ifndef VFATAL_LOG
#define VFATAL_LOG(expr) VLOGHANDLER_FATAL(VFATAL_PREFIX VCODE_LOCATION VCOLOR_TERMINATION "Assertion failed: '" #expr "'" \
											   "\n",                           \
					   __FILE__, __LINE__, __func__)
#endif // VFATAL_LOG
#ifndef VFATAL_LOG_MSG
#define VFATAL_LOG_MSG(expr, fmt, ...) VLOGHANDLER_FATAL(VFATAL_PREFIX VCODE_LOCATION VCOLOR_TERMINATION "Assertion failed: '" #expr "' '" fmt "'\n", \
							 __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#endif // VFATAL_LOG_MSG
#ifndef VWARN_LOG
#define VWARN_LOG(expr) VLOGHANDLER_WARN(VWARN_PREFIX VCODE_LOCATION VCOLOR_TERMINATION "Assertion failed: '" #expr "'" \
											"\n",                           \
					 __FILE__, __LINE__, __func__)
#endif // VWARN_LOG

#ifndef VWARN_LOG_MSG
#define VWARN_LOG_MSG(expr, fmt, ...) VLOGHANDLER_WARN(VWARN_PREFIX VCODE_LOCATION VCOLOR_TERMINATION "Assertion failed: '" #expr "' '" fmt "'\n", \
						       __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#endif // VWARN_LOG_MSG

#ifndef VNO_LOGGING_SYSTEM

#ifdef VNO_LOCATED_LOGS
#define VFATAL(fmt, ...) VLOGHANDLER_FATAL(VFATAL_PREFIX "]" VCOLOR_TERMINATION fmt "\n", ##__VA_ARGS__)
#define VERROR(fmt, ...) VLOGHANDLER_ERROR(VERROR_PREFIX "]" VCOLOR_TERMINATION fmt "\n", ##__VA_ARGS__)
#define VINFO(fmt, ...) VLOGHANDLER_INFO(VINFO_PREFIX "]" VCOLOR_TERMINATION fmt "\n", ##__VA_ARGS__)
#define VWARN(fmt, ...) VLOGHANDLER_WARN(VWARN_PREFIX "]" VCOLOR_TERMINATION fmt "\n", ##__VA_ARGS__)
#else
#define VFATAL(fmt, ...) VLOGHANDLER_FATAL(VFATAL_PREFIX VCODE_LOCATION VCOLOR_TERMINATION fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define VERROR(fmt, ...) VLOGHANDLER_ERROR(VERROR_PREFIX VCODE_LOCATION VCOLOR_TERMINATION fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define VINFO(fmt, ...) VLOGHANDLER_INFO(VINFO_PREFIX VCODE_LOCATION VCOLOR_TERMINATION fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define VWARN(fmt, ...) VLOGHANDLER_WARN(VWARN_PREFIX VCODE_LOCATION VCOLOR_TERMINATION fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#endif // VNO_LOCATED_LOGS

#endif // VNO_LOGGING_SYSTEM
// Asserts that can't be optimized out
#define VENSURE(expr)                     \
	do {                              \
		if (VUNLIKELY(!(expr))) { \
			VFATAL_LOG(expr); \
			VABORT();         \
		}                         \
	} while (0)

#define VENSURE_MSG(expr, msg, ...)                                \
	do {                                                       \
		if (VUNLIKELY(!(expr))) {                          \
			VFATAL_LOG_MSG(#expr, msg, ##__VA_ARGS__); \
			VABORT();                                  \
		}                                                  \
	} while (0)

#ifndef NDEBUG
#define VASSERT(expr) VENSURE(expr)
#define VASSERT_MSG(expr, msg, ...) VENSURE_MSG(expr, msg, ##__VA_ARGS__)
#define VASSERT_WARN(expr)                \
	do {                              \
		if (VUNLIKELY(!(expr))) { \
			VWARN_LOG(expr);  \
		}                         \
	} while (0)
#define VASSERT_WARN_MSG(expr, msg, ...)                          \
	do {                                                      \
		if (VUNLIKELY(!(expr))) {                         \
			VWARN_LOG_MSG(#expr, msg, ##__VA_ARGS__); \
		}                                                 \
	} while (0)

#define VALWAYS(expr) true
#define VNEVER(expr) false
#else
#define VASSERT(expr) ((void)0)
#define VASSERT_MSG(expr, msg) ((void)0)
#define VASSERT_WARN(expr) ((void)0)
#define VASSERT_WARN_MSG(expr, msg) ((void)0)
#define VALWAYS(expr) expr
#define VNEVER(expr) expr

#endif //NDEBUG

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define VASSERT_STATIC(expr, msg) _Static_assert(expr, msg)

#elif defined(__cplusplus) && __cplusplus >= 201103L
#define VASSERT_STATIC(expr, msg) static_assert(expr, msg)

#else
#define VASSERT_STATIC(expr, msg) \
	typedef char VASSERT_STATIC_##__LINE__[(expr) ? 1 : -1]
#endif

#define VASSERT_STATIC_NOMSG(expr) VASSERT_STATIC(expr, "static assertion failed")

#endif // INCLUDE_SRC_ASSERTS_H_
