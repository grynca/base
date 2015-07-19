#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>
#include <cassert>
#include "../functions/debug.h"

namespace grynca {

    struct Exception : public std::runtime_error
    {
        Exception() throw()
         : std::runtime_error("")
        {
            dout << this->what() << std::endl;
            assert(false);
        }
        Exception(const std::string& msg) throw()
         : std::runtime_error(msg)
        {
            dout << this->what() << std::endl;
            assert(false);
        }
    };

}
#endif // EXCEPTION_H