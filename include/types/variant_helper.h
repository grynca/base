#ifndef VARIANTHELPER_H
#define VARIANTHELPER_H

#include "Type.h"
#include <cassert>

namespace grynca {
    namespace internal {

        struct VariantIfaceCaster {
            template <typename V, typename T, typename IfaceT>
            static void f(V& v, T& t, IfaceT*& i) {
                ASSERT((std::is_base_of<IfaceT, T>::value));
                i = (IfaceT*)(&t);
            }
        };

        template<typename... Ts>
        struct VariantHelper {
            static const int typesCount = sizeof...(Ts);

            DestroyFunc destroy_funcs[typesCount];
            CopyFunc copy_funcs[typesCount];
            MoveFunc move_funcs[typesCount];

            VariantHelper();

            void destroy(int pos, void * data) {
                destroy_funcs[pos](data);
            }

            void move(int pos, void * to, void * from) {
                move_funcs[pos](to, from);
            }

            void copy(int pos, void * to, const void * from) {
                copy_funcs[pos](to, from);
            }
        private:
        };

        template<typename...Ts>
        struct FuncsFiller;

        template<typename First, typename... Rest>
        struct FuncsFiller<First, Rest...> {
            template <typename... Ts>
            static void fill(int pos, VariantHelper<Ts...>& vh) {
                vh.destroy_funcs[pos] = &Type<First>::destroy;
                vh.copy_funcs[pos] = &Type<First>::copy;
                vh.move_funcs[pos] = &Type<First>::move;
                FuncsFiller<Rest...>::fill(pos+1, vh);
            }
        };

        template<>
        struct FuncsFiller<> {
            template <typename... Ts>
            static void fill(int pos, VariantHelper<Ts...>& vh) { }
        };

        template<typename... Ts>
        inline VariantHelper<Ts...>::VariantHelper() {
            FuncsFiller<Ts...>::fill(0, *this);
        }
    };
}


#endif //VARIANTHELPER_H
