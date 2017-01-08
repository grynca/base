#ifndef VARIANTCALLER_H
#define VARIANTCALLER_H

#include "Call.h"
#include "Type.h"

namespace grynca {

    namespace internal {

        template <typename TrueFunctor, typename FalseFunctor, typename Cond, typename VariantType, typename... Args>
        struct FuncsInstance {

            typedef void (*FuncType)(VariantType&, Args&&...);
            FuncsInstance() {
                createFuncs_<typename VariantType::Types>();
            }

            template<typename TP>
            IF_EMPTY(TP) createFuncs_() {};
            template<typename TP>
            IF_NOT_EMPTY(TP) createFuncs_() {
                int type_id = VariantType:: template getTypeIdOf<HEAD(TP)>();
                funcs[type_id] = func_<HEAD(TP)>;
                createFuncs_<TAIL(TP)>();
            };

            template <typename RealType>
            static void func_(VariantType& v, Args&&... args) {
                Call<TrueFunctor>::template ifTrue<Cond>(v, v.template get<RealType>(), std::forward<Args>(args)...);
                Call<FalseFunctor>::template ifFalse<Cond>(v, v.template get<RealType>(), std::forward<Args>(args)...);
            }

            FuncType funcs[VariantType::Types::getTypesCount()];
        };

        template <typename TrueFunctor, typename FalseFunctor, typename Cond, typename VariantType, typename... Args>
        struct FuncsTypes {
            typedef void (*FuncType)(Args&&...);
            FuncsTypes() {
                createFuncs_<typename VariantType::Types>();
            }

            template<typename TP>
            IF_EMPTY(TP) createFuncs_() {};
            template<typename TP>
            IF_NOT_EMPTY(TP) createFuncs_() {
                int type_id = VariantType:: template getTypeIdOf<HEAD(TP)>();

                funcs[type_id] = func_<HEAD(TP)>;
                createFuncs_<TAIL(TP)>();
            };

            template <typename RealType>
            static void func_(Args&&... args) {
                Call<TrueFunctor, RealType>::template ifTrue<typename Cond::template apply<RealType> >(std::forward<Args>(args)...);
                Call<FalseFunctor, RealType>::template ifFalse<typename Cond::template apply<RealType> >(std::forward<Args>(args)...);
            }

            FuncType funcs[VariantType::Types::getTypesCount()];
        };
    }

    struct EmptyFunctor {
        template <typename... Args>
        static void f(Args&&...) {}
    };

    struct EmptyFunctorT {
        template <typename T, typename... Args>
        static void f(Args&&...) {}
    };

    // called on variant instance
    template <typename TrueFunctor, typename FalseFunctor = EmptyFunctor>
    class VariantCaller {
    public:
        template <typename VariantType, typename...Args>
        static void call(VariantType &v, Args &&... args) {
            static internal::FuncsInstance<TrueFunctor, FalseFunctor, TrueTrait, VariantType, Args...> funcs;
            ASSERT_M(v.valid(),
                  "Cant call on invalid variant.");
            funcs.funcs[v.getTypeId()](v, std::forward<Args>(args)... );
        }

        template <typename VariantType, typename Cond, typename...Args>
        static void callCond(VariantType &v, Args &&... args) {
            static internal::FuncsInstance<TrueFunctor, FalseFunctor, Cond, VariantType, Args...> funcs;
            ASSERT_M(v.valid(),
                   "Cant call on invalid variant.");
            funcs.funcs[v.getTypeId()](v, std::forward<Args>(args)... );
        };

    };

    // called on all types in variant
    template <typename TrueFunctor, typename FalseFunctor = EmptyFunctorT>
    class VariantTypesLooper {
    public:

        template <typename VariantType, typename...Args>
        static void call(Args&&... args) {
            static internal::FuncsTypes<TrueFunctor, FalseFunctor, TrueTrait, VariantType, Args...> funcs;
            u32 types_count = VariantType::Types::getTypesCount();
            for (u32 i=0; i<types_count; ++i) {
                funcs.funcs[i](std::forward<Args>(args)... );
            }
        }

        template <typename VariantType, typename Cond, typename...Args>
        static void callCond(Args&&... args) {
            static internal::FuncsTypes<TrueFunctor, FalseFunctor, Cond, VariantType, Args...> funcs;
            u32 types_count = VariantType::Types::getTypesCount();
            for (u32 i=0; i<types_count; ++i) {
                funcs.funcs[i](std::forward<Args>(args)... );
            }
        }
    };

}

#endif //VARIANTCALLER_H
