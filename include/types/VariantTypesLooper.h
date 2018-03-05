#ifndef VARIANTTYPESLOOPER_H
#define VARIANTTYPESLOOPER_H

#include "../functions/meta.h"
#include "Type.h"
#include "TypedFuncs.h"

namespace grynca {

    // called on all types in variant
    template <typename TrueFunctor, typename FalseFunctor = EmptyFunctorT>
    class VariantTypesLooper {
    public:
        template <typename VariantType, typename...Args>
        static void call(Args&&... args) {
            u32 types_count = VariantType::Types::getTypesCount();
            for (u32 i=0; i<types_count; ++i) {
                TypedFuncs<CallWithoutObjF_<Args...>, typename VariantType::Types>::funcs[i](std::forward<Args>(args)...);
            }
        }

        template <typename VariantType, typename Cond, typename...Args>
        static void callCond(Args&&... args) {
            u32 types_count = VariantType::Types::getTypesCount();
            for (u32 i=0; i<types_count; ++i) {
                TypedFuncs<CallCondF_<Cond, Args...>, typename VariantType::Types>::funcs[i](std::forward<Args>(args)...);
            }
        }
    private:
        template <typename... Args>
        TEMPLATED_FUNCTOR(CallWithoutObjF_, SINGLE_ARG((void*, Args&&... args) {
                TrueFunctor::template f<T>(std::forward<Args>(args)...);
        }))

        template <typename Cond, typename... Args>
        TEMPLATED_FUNCTOR(CallCondF_, SINGLE_ARG((void*, Args&&... args) {
                Call<TrueFunctor, T>::template ifTrue<typename Cond::template apply<T> >(std::forward<Args>(args)...);
                Call<FalseFunctor, T>::template ifFalse<typename Cond::template apply<T> >(std::forward<Args>(args)...);
        }))
    };
}

#endif //VARIANTTYPESLOOPER_H
