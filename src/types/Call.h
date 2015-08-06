#ifndef CALL_H
#define CALL_H

#include "../functions/meta.h"
#include <utility>
#include <type_traits>

namespace grynca {

    template <typename Func>
    class Call {
    public:
        template <typename Trait, typename... Args>
        static typename std::enable_if<!Trait::value>::type
        ifTrue(Args&&... args) { }      // empty fallback func

        template <typename Trait, typename... Args>
        static typename std::enable_if<Trait::value>::type
        ifTrue(Args&&... args) {
            Func::f(std::forward<Args>(args)...);
        }

        template <typename Trait, typename... Args>
        static typename std::enable_if<Trait::value>::type
        ifFalse(Args&&... args) { }      // empty fallback func

        template <typename Trait, typename... Args>
        static typename std::enable_if<!Trait::value>::type
        ifFalse(Args&&... args) {
            Func::f(std::forward<Args>(args)...);
        }
    };
}

#endif //CALL_H
