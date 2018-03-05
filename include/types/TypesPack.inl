#include "TypesPack.h"
#include "Type_internal.h"

namespace grynca {


    template <typename F, typename... R>
    inline constexpr int TypesPack<F, R...>::getTypesCount() {
        //static
        return sizeof...(R) +1;
    }

    template <typename F, typename... R>
    template <typename T>
    inline constexpr int TypesPack<F, R...>::pos() {
        //static
        return position<T, F, R...>::pos;
    }

    template <typename F, typename... R>
    inline constexpr int TypesPack<F, R...>::empty() {
        //static
        return false;
    }

    template <typename F, typename... R>
    inline const TypeInfo& TypesPack<F, R...>::getTypeInfo(int pos) {
        //static
        static internal::TypeIdTypesPackSetter<Types> lazy_setter_;

        ASSERT_M(pos < getTypesCount(), "Invalid type pos.");
        return TypeInfoManager<Types>::get(pos);
    }

    template <typename F, typename... R>
    template <typename... Tss>
    inline TypesPack<F, R...,Tss...> TypesPack<F, R...>::expand() {
        //static
        return TypesPack<F, R...,Tss...>();
    }

    template <typename F, typename... R>
    template <typename Functor, typename... Args>
    inline void TypesPack<F, R...>::callOnTypes(Args&&... args) {
        // static
        callOnInner_<Types, Types, Functor>(std::forward<Args>(args)...);
    }

    template <typename F, typename... R>
    template <typename TPOrig, typename TP, typename Functor, typename... Args>
    inline IF_EMPTY(TP) TypesPack<F, R...>::callOnInner_(Args&&... args)
    // static
    {}

    template <typename F, typename... R>
    template <typename TPOrig, typename TP, typename Functor, typename... Args>
    inline IF_NOT_EMPTY(TP) TypesPack<F, R...>::callOnInner_(Args&&... args) {
        // static
        Functor::template f<TPOrig, HEAD(TP)>(std::forward<Args>(args)...);
        callOnInner_<TPOrig, TAIL(TP), Functor>(std::forward<Args>(args)...);
    }

    template <typename F, typename... R>
    template <typename Domain>
    void TypesPack<F, R...>::mapIds(fast_vector<u32>& ids_mapper_out) {
        // static
        internal::TypeIdTypesPackMapper<Types, Domain>::map(ids_mapper_out);
    };

}