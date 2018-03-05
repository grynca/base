#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <cassert>
#include <stdexcept>
#include "3rdp/ustring.h"
#include "../functions/debug.h"

#ifdef DEBUG_BUILD
#   ifdef _WIN32
#       include <windows.h>
#       define DEBUG_BREAK() DebugBreak()
#   else
#       include <signal.h>
#       define DEBUG_BREAK() raise(SIGTRAP)
#   endif
#else
#   define DEBUG_BREAK()
#endif

namespace grynca {

    struct Exception : public std::runtime_error
    {
        Exception() throw()
         : std::runtime_error("")
        {
            dout(this->what() << std::endl);
            DEBUG_BREAK();
        }
        Exception(const ustring& msg) throw()
         : std::runtime_error(msg.cpp_str())
        {
            dout(this->what() << std::endl);
            DEBUG_BREAK();
        }
    };

}
#endif // EXCEPTION_H
