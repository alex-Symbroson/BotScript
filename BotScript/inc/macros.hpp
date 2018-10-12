
#ifndef _BS_EXT_HPP_
#define _BS_EXT_HPP_

#include <stdio.h>
#include <stdlib.h>


extern uint debug_depth;
extern void Free();
extern bool debg;


#define _DEBUG_ 0b001 // 1<<0:INFO;  1<<1:DEBUG;  1<<2:BEGIN,END macro
#define _ERR_EXIT_ 1  // exit on error
#define COMMA ,


#ifndef ISPI
#    define ISPI false
#endif

#ifndef ISBOT
#    define ISBOT true
#endif

// doesnt work for empty argument list
#define VA_ARGC_SEQ(                                                         \
    _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16,   \
    _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, N, ...) \
    N
#define VA_ARGC(...)                                                         \
    VA_ARGC_SEQ(                                                             \
        __VA_ARGS__, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, \
        15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

#define S_INIT 1
#define S_FORMAT 2
#define S_EXEC 3
#define S_FREE 4

#define ONCE(...) \
    do { __VA_ARGS__ } while (0)

// macros for error mesages
#define err_iop(o, a, b)                                           \
    error_exit(                                                    \
        "\033[1;31mno operator %s for %s and %s\033[0;37m\n", (o), \
        getTypeName(a), getTypeName(b))

#define err_imb(a, b)                                                   \
    error_exit(                                                         \
        "\033[1;31m%ss has no member '%s'\033[0;37m\n", getTypeName(a), \
        TOSTR(b))

#define err_iat(a, b, f, t)                                         \
    error_exit(                                                     \
        "\033[1;31minvalid argument type '%s' for %s.%s: expected " \
        "'%s'\033[0;37m\n",                                         \
        getTypeName(b), getTypeName(a), (f), typeName(t))

#define err_iac(f, a, c)                                                \
    error_exit(                                                         \
        "\033[1;31minvalid argument count for %s: got %lu; expected %i" \
        "\033[0;37m\n",                                                 \
        (f), (a).size(), (c))

#define err_iac2(a, b, f, c)                                               \
    error_exit(                                                            \
        "\033[1;31minvalid argument count for %s.%s: got %lu; expected %i" \
        "\033[0;37m\n",                                                    \
        getTypeName(a), (f), (b).size(), (c))

#define err_rng(a, b)                                                       \
    error_exit(                                                             \
        "\033[1;31m%s index '%s' out of range\033[0;37m\n", getTypeName(a), \
        TOSTR(b))

#define error(a, ...) \
    fprintf(stderr, "\033[1;31m" a "\033[0;37m\n", ##__VA_ARGS__)

#define Exit() (Free(), exit(1))

#if defined(_ERR_EXIT_) && _ERR_EXIT_ != 0
#    define error_exit(a, ...) (error(a, ##__VA_ARGS__), Exit())
#else
#    define error_exit error
#endif

#define wait_enter() ONCE(stdin = freopen(NULL, "r", stdin); getchar();)

// blue coloured info macro
#if _DEBUG_ & 1 << 0
#    define INFO(s, ...) \
        printf("%5i\033[0;36m " s "\033[0;37m\n", debug_depth, ##__VA_ARGS__)
#else
#    define INFO(...)
#endif

// yellow coloured debug macro
#if _DEBUG_ & 1 << 1
#    define DEBUG(s, ...) \
        printf("%5i\033[2;33m " s "\033[0;37m\n", debug_depth, ##__VA_ARGS__)
#else
#    define DEBUG(...)
#endif

// green colored begin / end macro
#if _DEBUG_ & 1 << 2
#    define _BEGIN(func_fmt, func, s, ...)                             \
        if (!debg && (debg = true))                                    \
        printf(                                                        \
            "\033[1;33m%5i\033[2;32m %s%s line %4i " func_fmt "(" s    \
            ")\033[0;37m\n",                                           \
            ++debug_depth, __FILE__,                                   \
            "                    " + strlen(__FILE__), __LINE__, func, \
            ##__VA_ARGS__),                                            \
            debg = false

#    define _END(func_fmt, func, s, ...)                                \
                                                                        \
        if (!debg && (debg = true))                                     \
        printf(                                                         \
            "\033[1;31m%5i\033[2;32m %s%s line %4i " func_fmt "() %s" s \
            "\033[0;37m\n",                                             \
            debug_depth--, __FILE__,                                    \
            "                    " + strlen(__FILE__), __LINE__, func,  \
            (s + 0) ? "-> " : "", ##__VA_ARGS__),                       \
            debg = false
#else
#    define _BEGIN(...) ++debug_depth
#    define _END(...) debug_depth--
#endif

#ifndef CUSTOM_BEGIN
#    define BEGIN(s, ...) _BEGIN("%s", __func__, s, ##__VA_ARGS__)
#    define END(s, ...) _END("%s", __func__, s, ##__VA_ARGS__)
#else
#    define BEGIN_1(class, func, s, ...) \
        _BEGIN(class "%s", func, s, ##__VA_ARGS__)
#    define END_1(class, func, s, ...) _END(class "%s", func, s, ##__VA_ARGS__)
#endif

// compilation infos
#define PRINT_STATUS()                                                         \
    printf(                                                                    \
        "compiled: " _DATE_S_ _TIME_S_ _OS_NAME_S_ "\n\n", __DATE__, __TIME__, \
        _OS_NAME_)

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
