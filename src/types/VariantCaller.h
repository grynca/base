#ifndef VARIANTCALLER_H
#define VARIANTCALLER_H

namespace grynca {

    namespace internal {
        template <typename Functor, typename VariantType, typename... Args>
        struct Funcs {
            typedef void (*FuncType)(VariantType&, Args&&...);
            Funcs() {
                createFuncs_<typename VariantType::Types>();
            }

            template<typename TP>
            IF_EMPTY(TP) createFuncs_() {};
            template<typename TP>
            IF_NOT_EMPTY(TP) createFuncs_() {
                int type_id = VariantType:: template typePos<HEAD(TP)>();
                funcs[type_id] = func_<HEAD(TP)>;
                createFuncs_<TAIL(TP)>();
            };

            template <typename RealType>
            static void func_(VariantType& v, Args&&... args) {
                Functor::f(v, v.template get<RealType>(), std::forward<Args>(args)...);
            }

            FuncType funcs[VariantType::Types::getTypesCount()];
        };
    }

    template <typename Functor>
    class VariantCaller {
    public:
        template <typename VariantType, typename...Args>
        static void call(VariantType& v, Args&&... args) {
            static internal::Funcs<Functor, VariantType, Args...> funcs;
            ASSERT(v.valid(),
                  "Cant call on variant.");
            funcs.funcs[v.getCurrentType()](v, std::forward<Args>(args)... );
        }
    };

}

#endif //VARIANTCALLER_H
