
#ifndef _BS_EXT_HPP_
#define _BS_EXT_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Toggles
// 1<<3:parent error lines;  1<<2:BEGIN,END macro;  1<<1:DEBUG;  1<<0:INFO;
#define _DEBUG_ 0b0000 // 0b1001
#define _ERR_EXIT_ 1   // exit on error

// Statuses
#define S_INIT 1
#define S_FORMAT 2
#define S_EXEC 3
#define S_BREAK 4
#define S_RETURN 5
#define S_CONTINUE 6
#define S_STOP 7
#define S_ERROR 8
#define S_FREE 9

//#define BREAK ("\0\0\0\0\1\1\1\1\0"[status])
//#define RETURN ("\0\0\0\0\0\1\0\1\0")[status])
#define BREAK                                                        \
    (status == S_BREAK || status == S_RETURN || status == S_ERROR || \
     status == S_CONTINUE || status == S_STOP)
#define RETURN (status == S_RETURN || status == S_ERROR || status == S_STOP)


#ifndef ISPI
#    define ISPI false
#endif

#ifndef ISBOT
#    define ISBOT false
#endif


// clang-format off

extern uint debug_depth;
extern void Free();
extern bool debg;

// count macro araguments
// doesnt work for empty argument list
#define VA_ARGC_SEQ(                                                         \
    _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16,   \
    _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, N, ...) N

#define VA_ARGC(...) VA_ARGC_SEQ( \
    __VA_ARGS__, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, \
    15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)


#define COMMA ,
#define ONCE(...) do { __VA_ARGS__ } while (0)
#define NORMCOL "\033[0;37m\n"
#define FILESPACE(name) ("                    " + strlen(name))

#if UINTPTR_MAX == 0xffffffffffffffff
#define FMT_SIZE "%lu" // 64 bit
#elif UINTPTR_MAX == 0xffffffff
#define FMT_SIZE "%u" // 32 bit
#endif

// macros for error mesages
#define err_iop(o, a, b) error_exit( \
    "no operator %s for %s and %s", (o), getTypeName(a), getTypeName(b))

#define err_imb(a, b) error_exit( \
    "%ss has no member '%s'", getTypeName(a), TOSTR(b))

#define err_iat(a, b, f, t) error_exit( \
    "invalid argument type '%s' for %s.%s: expected '%s'", getTypeName(b), \
    getTypeName(a), (f), typeName(t))

#define err_iac(f, a, c) error_exit( \
    "invalid argument count for %s: got " FMT_SIZE "; expected %i", (f), \
    (a).size(), (c))

#define err_iac2(a, b, f, c) error_exit( \
    "invalid argument count for %s.%s: got " FMT_SIZE "; expected %i", \
    getTypeName(a), (f), (b).size(), (c))

#define err_rng(a, b) error_exit( \
    "%s index '%s' out of range", getTypeName(a), TOSTR(b))

// bold red error
#define error(a, ...) fprintf(stderr, \
    "%5i\033[1;31m %s%s line %4i %s\n" a NORMCOL, debug_depth, \
    __FILE__, FILESPACE(__FILE__), __LINE__, __func__, ##__VA_ARGS__)

// bold yellow info
#define warning(a, ...) fprintf(stderr, \
    "%5i\033[1;33m %s%s line %4i %s\n" a NORMCOL, debug_depth, \
    __FILE__, FILESPACE(__FILE__), __LINE__, __func__, ##__VA_ARGS__)

#define Exit() (status != S_FREE && (Free(), exit(1), 1))


#if defined(_ERR_EXIT_) && _ERR_EXIT_ != 0
#    define error_exit(a, ...) (error(a, ##__VA_ARGS__), Exit())
#else
#    define error_exit error
#endif


#define wait_enter() ONCE(stdin = freopen(NULL, "r", stdin); getchar();)

#if _DEBUG_ & 1 << 3
#define ERR_PARAM , const char* func , const char* file , uint line
#define ERR_ARGS , __func__ , __FILE__ , __LINE__
#define ERR_STR "error %s%s line %4i %s:\n"
#define ERR_VALS file , FILESPACE(file) , line , func
#else
#define ERR_PARAM
#define ERR_ARGS
#define ERR_STR "%s"
#define ERR_VALS ""
#endif

// green info
#if _DEBUG_ & 1 << 0
#    define INFO(s, ...) fprintf(stderr, \
        "%5i\033[0;32m " s NORMCOL, debug_depth, ##__VA_ARGS__)
#else
#    define INFO(...)
#endif


// yellow debug
#if _DEBUG_ & 1 << 1
#    define DEBUG(s, ...) fprintf(stderr, \
    "%5i\033[0;33m " s NORMCOL, debug_depth, ##__VA_ARGS__)
#else
#    define DEBUG(...)
#endif

// grey begin / end
#if _DEBUG_ & 1 << 2
#    define _BEGIN(func_fmt, func, s, ...)                \
        if (!debg && (debg = true)) fprintf(stderr,       \
            "\033[0;93m%5i\033[0;90m %s%s line %4i "      \
            func_fmt "(" s ")" NORMCOL,                   \
            ++debug_depth, __FILE__, FILESPACE(__FILE__), \
            __LINE__, func, ##__VA_ARGS__),               \
            debg = false

#    define _END(func_fmt, func, s, ...)                          \
        if (!debg && (debg = true)) fprintf(stderr,               \
            "\033[0;91m%5i\033[0;90m %s%s line %4i "              \
            func_fmt "() %s" s NORMCOL,                           \
            debug_depth--, __FILE__, FILESPACE(__FILE__),         \
            __LINE__, func, (s + 0) ? "-> " : "", ##__VA_ARGS__), \
            debg = false
#else
#    define _BEGIN(...) ++debug_depth
#    define _END(...) debug_depth--
#endif
//clang-format on


#ifndef CUSTOM_BEGIN
#    define BEGIN(s, ...) _BEGIN("%s", __func__, s, ##__VA_ARGS__)
#    define END(s, ...) _END("%s", __func__, s, ##__VA_ARGS__)
#else
#    define BEGIN_1(class, func, s, ...) \
        _BEGIN(class "%s", func, s, ##__VA_ARGS__)
#    define END_1(class, func, s, ...) _END(class "%s", func, s, ##__VA_ARGS__)
#endif


// compilation infos
#define PRINT_STATUS() fprintf(stderr, \
    "compiled: " _DATE_S_ _TIME_S_ _OS_NAME_S_ \
    "\n\n", __DATE__, __TIME__, _OS_NAME_)

#define _DATE_S_ "%s "
#ifndef __DATE__
#    define __DATE__ "- "
#endif

#define _TIME_S_ "%s "
#ifndef __TIME__
#    define __TIME__ "- "
#endif

#define OS_LIN 1
#define OS_WIN 2

#ifdef __unix__
#    define OS_NUM OS_LIN
#elif defined(_WIN32) || defined(WIN32) || defined(_WIN32)
#    define OS_NUM OS_WIN
#else
#    define OS_NUM 0
#endif

#define _OS_NAME_S_ "%.*s"
#define _OS_NAME_ 7, "UnknownLinux  Windows" + 7 * OS_NUM
#endif
