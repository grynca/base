#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include "../types/BlockMeasure.h"

// Debug printing
// usage:
//      dout << "in foobar with x= " << x << " and y= " << y << '\n';
//
//  If the preprocessor replaces 'dout' with '0 && cout' note that << has higher precedence
//  than && and short-circuit evaluation of && makes the whole line evaluate to 0. Since
//  the 0 is not used the compiler generates no code at all for that line.
#if !defined(NDEBUG)
#   define dout std::cout
#   define DEBUG_BUILD
#else
#   define dout 0 && std::cout
#endif

#define KEY_TO_CONTINUE() \
std::cout << "press any key to continue ..." << std::endl; \
getchar() \

#ifdef DEBUG_BUILD
#   define ASSERT_M(condition, message) \
    if (! (condition)) { \
        std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                  << " line " << __LINE__ << ": " << (message); \
        uint8_t* ptr = NULL; \
        *ptr = 1; \
    }
#   define ASSERT(condition) ASSERT_M(condition, "")
#   define NEVER_GET_HERE(msg) ASSERT_M(false, msg)
#   ifdef _WIN32
#       include <windows.h>
#       define DEBUG_BREAK() DebugBreak()
#   else
#       include <csignal>
#       define DEBUG_BREAK() raise(SIGTRAP)
#   endif
#else
#   define ASSERT_M(condition, message)
#   define ASSERT(condition) ASSERT_M(condition, "")
#   define NEVER_GET_HERE(msg)
#   define DEBUG_BREAK()
#endif

#endif // DEBUG_H
