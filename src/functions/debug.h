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
#if defined(NDEBUG)
#define dout 0 && std::cout
#else
#define dout std::cout
#endif

#define KEY_TO_CONTINUE() \
std::cout << "press any key to continue ..." << std::endl; \
getchar() \

#ifndef NDEBUG
#   define ASSERT(condition, message) \
    if (! (condition)) { \
        std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                  << " line " << __LINE__ << ": " << (message); \
        uint8_t* ptr = NULL; \
        *ptr = 1; \
    }
#   define NEVER_GET_HERE(msg) ASSERT(false, ""#msg)
#else
#   define ASSERT(condition, message) do { } while (false)
#   define NEVER_GET_HERE(msg)
#endif

#endif // DEBUG_H
