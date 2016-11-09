#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <stdio.h>

#define DEBUG_BUILD
#if defined(NDEBUG)
#   undef DEBUG_BUILD
#endif

#define PROFILE_BUILD
#if defined(NPROFILE)
#   undef PROFILE_BUILD
#endif

#ifdef _WIN32
# define WAIT_FOR_KEY_ON_WIN() KEY_TO_CONTINUE()
#else
# define WAIT_FOR_KEY_ON_WIN()
#endif

#define KEY_TO_CONTINUE() \
 std::cout << "press any key to continue ..." << std::endl; \
 getchar()

#ifdef DEBUG_BUILD
#   define ASSERT_M(condition, message) \
    if (! (condition)) { \
        std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                  << " line " << __LINE__ << ": " << (message); \
        u8* ptr = NULL; \
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
#   define dout std::cout
#else
#   define ASSERT_M(condition, message)
#   define ASSERT(condition) ASSERT_M(condition, "")
#   define NEVER_GET_HERE(msg)
#   define DEBUG_BREAK()
#   define dout 0 && std::cout
#endif

#ifdef PROFILE_BUILD
#   define PROFILE_MEASURE_FROM(m) m.from()
#   define PROFILE_MEASURE_TO(m) m.to()
#   define DEF_MEASURE(m) Measure m
#else
#   define PROFILE_MEASURE_FROM(m)
#   define PROFILE_MEASURE_TO(m)
#   define DEF_MEASURE(m)
#endif

#endif // DEBUG_H
