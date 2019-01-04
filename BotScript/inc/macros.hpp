
#ifndef _BS_EXT_HPP_
#define _BS_EXT_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Toggles
#define _DEBUG_ 1    // debug logs
#define _P_ERROR_ 1  // parent errors
#define _F_CALLS_ 0  // function calls
#define _INFO_ 0     // info logs
#define _ERR_EXIT_ 1 // exit on error

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


// keep comma in macro call
#define COMMA ,
// execute expression ince
#define ONCE(...) do { __VA_ARGS__ } while (0)
// escape seq to reset color to white
#define NORMCOL "\033[0;37m\n"
// format file names to equal length
#define FILESPACE(name) ("                    " + strlen(name))

// size_t format
#if UINTPTR_MAX == 0xffffffffffffffff
#define FMT_SIZE "%lu" // 64 bit
#elif UINTPTR_MAX == 0xffffffff
#define FMT_SIZE "%u" // 32 bit
#endif

// macros for error mesage types
//invalid operator
#define err_iop(o, a, b) error_exit( \
    "no operator %s for %s and %s", (o), getTypeName(a), getTypeName(b))

// invalid member
#define err_imb(a, b) error_exit( \
    "%ss has no member '%s'", getTypeName(a), TOSTR(b))

// invalid argument type
#define err_iat(a, b, f, t) error_exit( \
    "invalid argument type '%s' for %s.%s: expected '%s'", getTypeName(b), \
    getTypeName(a), (f), typeName(t))

// invalid argument type
#define err_iac(f, a, c) error_exit( \
    "invalid argument count for %s: got " FMT_SIZE "; expected %i", (f), \
    (a).size(), (c))

// invalid argument type on subfunction
#define err_iac2(a, b, f, c) error_exit( \
    "invalid argument count for %s.%s: got " FMT_SIZE "; expected %i", \
    getTypeName(a), (f), (b).size(), (c))

// range error
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

// parent func info on error
#if _P_ERROR_
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
#if _INFO_
#    define INFO(s, ...) fprintf(stderr, \
        "%5i\033[0;32m " s NORMCOL, debug_depth, ##__VA_ARGS__)
#else
#    define INFO(...)
#endif


// yellow debug
#if _DEBUG_
#    define DEBUG(s, ...) fprintf(stderr, \
    "%5i\033[0;33m " s NORMCOL, debug_depth, ##__VA_ARGS__)
#else
#    define DEBUG(...)
#endif

// grey begin / end
#if _F_CALLS_
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
#define PRINT_STATUS() INFO(                   \
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
