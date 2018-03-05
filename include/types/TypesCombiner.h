#ifndef TYPESCOMBINER_H
#define TYPESCOMBINER_H

#include "TypesPack.h"

namespace grynca {

    template <typename T>
    class ClassWrap {
    public:
        const T& get()const { return a_; }
        T& acc() { return a_; }
    private:
        T a_;
    };

    template <typename...> class TypesCombiner;

    template <typename... Ts>
    class TypesCombiner : public ClassWrap<Ts>... {
    public:
        using TP = TypesPack<Ts...>;

        template <typename T>
        const T& get()const {
            ASSERT((std::is_base_of<ClassWrap<T>, TypesCombiner<Ts...>>::value));
            return ((ClassWrap<T>*)this)->get();
        }

        template <typename T>
        T& acc() {
            ASSERT((std::is_base_of<ClassWrap<T>, TypesCombiner<Ts...>>::value));
            return ((ClassWrap<T>*)this)->acc();
        }
    };

    // spec for TypesPack
    template <typename... Ts>
    class TypesCombiner<TypesPack<Ts...>> : public TypesCombiner<Ts...> {};


}

#endif //TYPESCOMBINER_H
