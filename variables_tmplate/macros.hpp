
#ifndef _BS_EXT_HPP_
#define _BS_EXT_HPP_

#include <stdio.h>
#include <stdlib.h>

#define _DEBUG_ 2    // enables 1:INFO 2:BEGIN,END and 3:DEBUG macro
#define _ERR_EXIT_ 1 // exit on error

#define ONCE(...)   \
    do {            \
        __VA_ARGS__ \
    } while (0)

// macros for error mesages
#define err_ict(a, b) \
    printf("\033[1;31mincompatible types %s and %s\033[0;37m\n", a, b)
#define err_iop(a, b) \
    printf("\033[1;31minvalid operator %s for %s\033[0;37m\n", a, b)
#define err_imu(a, b) printf("\033[1;31m%ss have no '%s' member\033[0;37m\n", a, b)
#define error(a, ...) printf("\033[1;31m" a "\033[0;37m\n", ##__VA_ARGS__)

#if defined(_ERR_EXIT_) && _ERR_EXIT_ != 0
#    define error_exit(a, ...) (error(a, ##__VA_ARGS__), FreeVariables(), exit(0))
#else
#    define error_exit error
#endif

#define wait_enter() ONCE( stdin = freopen(NULL, "r", stdin); getchar() \
    } )

// yellow coloured debug macro
#if _DEBUG_ > 0
#    define INFO(s, ...) printf("\033[0;36m    " s "\033[0;37m\n", ##__VA_ARGS__)
#else
#    define INFO(...)
#endif

//
#if _DEBUG_ > 2
#    ifndef CUSTOM_BEGIN
#        define BEGIN(s, ...)                                                \
            printf(                                                          \
                " \033[1;33m*\033[2;32m%s\t line %4i %s(" s ")\033[0;37m\n", \
                __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#        define END(s, ...)                                                  \
            printf(                                                          \
                " \033[1;31m*\033[2;32m%s\t line %4i %s(" s ")\033[0;37m\n", \
                __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#    else
#        define BEGIN_1(class, s, ...)                                           \
            printf(                                                              \
                " \033[1;33m*\033[2;32m%s\t line %4i %s::%s(" s ")\033[0;37m\n", \
                __FILE__, __LINE__, class, __func__, ##__VA_ARGS__)
#        define END_1(class, s, ...)                                             \
            printf(                                                              \
                " \033[1;31m*\033[2;32m%s\t line %4i %s::%s(" s ")\033[0;37m\n", \
                __FILE__, __LINE__, class, __func__, ##__VA_ARGS__)
#    endif
#else
#    ifndef CUSTOM_BEGIN
#        define BEGIN(...)
#        define END(...)
#    else
#        define BEGIN_1(...)
#        define END_1(...)
#    endif
#endif

#if _DEBUG_ > 1
#    define DEBUG(s, ...) printf("\033[2;33m      " s "\033[0;37m\n", ##__VA_ARGS__)
#else
#    define DEBUG(...)
#endif

#define PRINT_STATUS() \
    printf("compiled: " _DATE_S_ _TIME_S_ "\n\n", __DATE__, __TIME__)

#define _DATE_S_ "date:\"%s\" "
#ifndef __DATE__
#    define __DATE__ "-"
#endif

#define _TIME_S_ "time:\"%s\" "
#ifndef __TIME__
#    define __TIME__ "-"
#endif

#endif
