#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>

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
        uint8_t* ptr = NULL; \
        *ptr = 1; \
    }
#   define ASSERT(condition) ASSERT_M(condition, "")
#   define NEVER_GET_HERE(msg) ASSERT_M(false, msg)
#   define dout(x) std::cout << x
#else
#   define ASSERT_M(condition, message)
#   define ASSERT(condition) ASSERT_M(condition, "")
#   define NEVER_GET_HERE(msg)
#   define dout(x)
#endif

#endif // DEBUG_H
