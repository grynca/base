#ifndef ENUM_H
#define ENUM_H

#include "../types/Mask.h"

#define _ENUM_ID(NAME) NAME##Id
#define _DEF_ENUM_MASK(NAME)  \
    static const grynca::Mask<end>& NAME##Mask() { \
        static grynca::Mask<end> v = grynca::Mask<end>::bit(NAME##Id); \
        return v; \
    }
#define DEFINE_ENUM(NAME, ...) \
    struct NAME { \
        enum { \
            APPLY_COMMA(_ENUM_ID, __VA_ARGS__), \
            end \
        }; \
        APPLY_SPACE(_DEF_ENUM_MASK, __VA_ARGS__) \
    };

// TODO fix: currently must contain at least two more ids to work correctly
#define DEFINE_ENUM_E(NAME, BASE, FIRST, ...) \
    struct NAME: public BASE { \
        enum { \
            _ENUM_ID(FIRST) = BASE::end, \
            APPLY_COMMA(_ENUM_ID, __VA_ARGS__), \
            end \
        }; \
        _DEF_ENUM_MASK(FIRST) \
        APPLY_SPACE(_DEF_ENUM_MASK, __VA_ARGS__) \
    };

#endif //ENUM_H
