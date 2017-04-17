#ifndef SDLINCLUDE_H
#define SDLINCLUDE_H

#include "types/Exception.h"
#include "SDL2/SDL.h"

namespace grynca {

    struct SDL_Exception : public Exception
    {
        SDL_Exception() throw()
                : Exception(ustring("SDL : ") + SDL_GetError())
        {}

        SDL_Exception(const char * text) throw()
                : Exception(ustring("SDL: ") + text + " " + SDL_GetError())
        {}
    };

    inline static void printSDLError(const char* function, const char* file, int line) {
        const char* err = SDL_GetError();
        if (*err) {
            std::cerr << "(" << file << ":" << line << ") " << function << ": " << err << std::endl;
            ASSERT(false);
            SDL_ClearError();
        }
    }
}

#ifndef NDEBUG
#define SDLCall(x) \
    x; grynca::printSDLError(#x, __FILE__, __LINE__);
#else
#define SDLCall(x) x
#endif

#endif //SDLINCLUDE_H
