#ifndef VARIANTHELPER_H
#define VARIANTHELPER_H

#include "Type.h"
#include "Call.h"
#include <cassert>

namespace grynca {
    namespace internal {

        template <typename... ConstrArgs>
        TEMPLATED_FUNCTOR(VariantConstructorF, (T* obj, ConstrArgs&&... args) {
                Type<T>::create2(obj, std::forward(args)...);
        })

        template <typename BaseT>
        TEMPLATED_FUNCTOR(BaseCasterF, (T* obj) {
            return Type<T>::template castToBase<BaseT>(obj);
        })

        template <typename Cond, typename TrueFunctor, typename FalseFunctor, typename... Args>
        TEMPLATED_FUNCTOR(CallCondF, SINGLE_ARG((T* obj, Args&&... args) {
                Call<TrueFunctor, T>::template ifTrue<typename Cond::template apply<T> >(obj, std::forward<Args>(args)...);
                Call<FalseFunctor, T>::template ifFalse<typename Cond::template apply<T> >(obj, std::forward<Args>(args)...);
        }))

        TEMPLATED_FUNCTOR(VariantCopyConstructorF, (T* obj, const void* src) {
                Type<T>::copy(obj, src);
        })

        TEMPLATED_FUNCTOR(VariantMoveConstructorF, (T* obj, void* src) {
            Type<T>::move(obj, src);
        })

        TEMPLATED_FUNCTOR(VariantDestructorF, (T* obj) {
            Type<T>::destroy(obj);
        })
    };
}


#endif //VARIANTHELPER_H
