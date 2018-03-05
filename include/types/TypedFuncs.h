#ifndef TYPEDFUNCS_H
#define TYPEDFUNCS_H

#include "ObjFunc.h"
#include "Type.h"

namespace grynca {

    /* TypedFuncs:
     *   statically creates functions for all types in TypesPack
     *   which then can be called with type-id in TypesPack
     *   (those are ObjFuncs, so object must be bound to them )
     * usage:
     *
     *   typedef TypesPack<MyType, Whatever, FooBarT> MyTP;
     *
     *   TEMPLATED_FUNCTOR(MyFunctor, (T* obj, int arg) {
     *      return &obj - sizeof(T)-arg;
     *   }
     *
     *  MyType obj;
     *  auto& f = TypedFuncs<MyFunctor, MyTP>::funcs[0];        // get Functor templated for MyType
     *  int rslt = f.callFor(&obj, 10);
     */

    //fw
    template <typename... Ts> struct TypedFuncs;

    template <typename Functor, typename... Ts>
    class TypedFuncs<Functor, TypesPack<Ts...> > {
        typedef typename TypesPack<Ts...>::Head FirstT;
        typedef typename ObjFuncTypeFromFunctor<decltype(Functor::template f<FirstT>)>::FuncT FuncType;
        public:
        static constexpr FuncType funcs[] = {
                createObjFuncT<Functor, Ts, Ts>()...
        };
    };

    template <typename Functor, typename... Ts>
    constexpr typename TypedFuncs<Functor, TypesPack<Ts...> >::FuncType TypedFuncs<Functor, TypesPack<Ts...> >::funcs[];

}

#endif //TYPEDFUNCS_H
