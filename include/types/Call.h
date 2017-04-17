#ifndef CALL_H
#define CALL_H

#include "../functions/meta.h"

namespace grynca {

    template <typename Functor, typename... TypeArgs>
    class Call {
    public:
        template <typename Trait, typename... Args>
        static typename std::enable_if<!Trait::value>::type
        ifTrue(Args&&... args) { }      // empty fallback func

        template <typename Trait, typename... Args>
        static typename std::enable_if<Trait::value>::type
        ifTrue(Args&&... args) {
            Functor::template f<TypeArgs...>(std::forward<Args>(args)...);
        }

        template <typename Trait, typename... Args>
        static typename std::enable_if<Trait::value>::type
        ifFalse(Args&&... args) { }      // empty fallback func

        template <typename Trait, typename... Args>
        static typename std::enable_if<!Trait::value>::type
        ifFalse(Args&&... args) {
            Functor::template f<TypeArgs...>(std::forward<Args>(args)...);
        }
    };
}

#endif //CALL_H
