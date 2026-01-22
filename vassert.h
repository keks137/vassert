#ifndef INCLUDE_SRC_ASSERTS_H_
#define INCLUDE_SRC_ASSERTS_H_

#include <stdint.h>
#include <stdlib.h>

#ifdef VABORT_DEBUG
#if _MSC_VER
#include <intrin.h>
#define VABORT() __debugBreak()
#else
#define VABORT() __builtin_trap()
#endif

#else

#define VABORT() abort()
#endif //VABORT_DEBUG

void vlog_failure(const char *expression, const char *message, const char *file, int32_t line, const char *func);
void vlog_warn(const char *expression, const char *message, const char *file, int32_t line, const char *func);

// for when you want to assert in release builds
#if defined(__GNUC__) || defined(__clang__)
#define VLIKELY(x) __builtin_expect(!!(x), 1)
#define VUNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define VLIKELY(x) (x)
#define VUNLIKELY(x) (x)
#endif

#define VPANIC(expr)                                                             \
	do {                                                                     \
		if (VUNLIKELY(!(expr))) {                                        \
			vlog_failure(#expr, NULL, __FILE__, __LINE__, __func__); \
			VABORT();                                                \
		}                                                                \
	} while (0)

#define VPANIC_MSG(expr, msg)                                                   \
	do {                                                                    \
		if (VUNLIKELY(!(expr))) {                                       \
			vlog_failure(#expr, msg, __FILE__, __LINE__, __func__); \
			VABORT();                                               \
		}                                                               \
	} while (0)

#ifndef NDEBUG
#define VASSERT(expr) VPANIC(expr)
#define VASSERT_MSG(expr, msg) VPANIC_MSG(expr, msg)
#define VASSERT_WARN(expr)                                                    \
	do {                                                                  \
		if (VUNLIKELY(!(expr))) {                                     \
			vlog_warn(#expr, NULL, __FILE__, __LINE__, __func__); \
		}                                                             \
	} while (0)
#define VASSERT_WARN_MSG(expr, msg)                                          \
	do {                                                                 \
		if (VUNLIKELY(!(expr))) {                                    \
			vlog_warn(#expr, msg, __FILE__, __LINE__, __func__); \
		}                                                            \
	} while (0)

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

#ifdef NDEBUG
#define GL_CHECK(stmt) \
	do {           \
		stmt;  \
	} while (0)
#else
#define GL_CHECK(stmt)                                                                                            \
	do {                                                                                                      \
		stmt;                                                                                             \
		GLenum err = glGetError();                                                                        \
		if (err != GL_NO_ERROR) {                                                                         \
			VERROR("OpenGL error 0x%04X at %s:%d: %s: %s", err, __FILE__, __LINE__, __func__, #stmt); \
		}                                                                                                 \
	} while (0)
#endif // NDEBUG

typedef enum {
	VLOG_LEVEL_FATAL = 0,
	VLOG_LEVEL_ERROR = 1,
	VLOG_LEVEL_WARN = 2,
	VLOG_LEVEL_INFO = 3,
	VLOG_LEVEL_DEBUG = 4,
	VLOG_LEVEL_TRACE = 5,
} VLOG_LEVEL;

#ifdef VASSERT_IMPLEMENTATION // NOTE: you can also just provide your own

#define VLOG_MAX_MESSAGE_LEN 1024

void vlog_failure(const char *expression, const char *message, const char *file, int32_t line, const char *func)
{
	if (message == NULL) {
		log_msgn(VLOG_LEVEL_FATAL, "%s:%d: %s: Assertion '%s' failed", file, line, func, expression);
	} else {
		log_msgn(VLOG_LEVEL_FATAL, "%s:%d: %s: Assertion '%s' failed: '%s'", file, line, func, expression, message);
	}
}
void vlog_warn(const char *expression, const char *message, const char *file, int32_t line, const char *func)
{
	if (message == NULL) {
		log_msgn(VLOG_LEVEL_WARN, "%s:%d: %s: Assertion '%s' failed", file, line, func, expression);
	} else {
		log_msgn(VLOG_LEVEL_WARN, "%s:%d: %s: Assertion '%s' failed: '%s'", file, line, func, expression, message);
	}
}

void vlog_msgn(VLOG_LEVEL level, const char *message, ...)
{
	bool is_error = level < 2;
	char buffer[VLOG_MAX_MESSAGE_LEN];

	size_t pos = strblcpy(buffer, level_strings[level], sizeof(buffer));

	va_list arg_ptr;
	va_start(arg_ptr, message);
	vsnprintf(buffer + pos, sizeof(buffer) - pos, message, arg_ptr);
	va_end(arg_ptr);

#ifdef __ANDROID__
	__android_log_print(level_to_prio[level], LOG_TAG, "%s", buffer);
#else
	FILE *outfile = is_error ? stderr : stdout;
	fprintf(outfile, "%s\n", buffer);
#endif
}

#endif // VASSERT_IMPLEMENTATION

#endif // INCLUDE_SRC_ASSERTS_H_
