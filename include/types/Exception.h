#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <cassert>
#include <stdexcept>
#include "3rdp/ustring.h"
#include "../functions/debug.h"

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
