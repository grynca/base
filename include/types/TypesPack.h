#ifndef TYPESPACK_H
#define TYPESPACK_H

#include "Type.h"

#define IF_EMPTY(TP) typename std::enable_if< TP::empty() >::type
#define IF_NOT_EMPTY(TP) typename std::enable_if< !TP::empty() >::type
#define HEAD(TP) typename TP::Head
#define TAIL(TP) typename TP::Tail

namespace grynca {

    template <typename...Ts>  class TypesPack;

    template <>
    class TypesPack<> {
    public:
        static constexpr int getTypesCount() { return 0;}

        // get type position id in pack
        template <typename T>
        static constexpr int pos() { return position<T>::pos; }

        static constexpr int empty() { return true; }

        static const TypeInfo& getTypeInfo(int pos) { ASSERT_M(false, "empty types pack"); return Type<void>::getTypeInfo(); }

        template <typename Domain>
        static void mapIds(fast_vector<u32>& ids_mapper_out) {}

        template <typename... Tss>
        static TypesPack<Tss...> expand() {
            return TypesPack<Tss...>();
        }

        template <typename Functor, typename... Args>
        static void callOnTypes(Args&&... args) {};
    };

    template <typename F, typename... R>
    class TypesPack<F, R...> {
    public:
        typedef TypesPack<F,R...> Types;
        typedef F Head;
        typedef TypesPack<R...> Tail;

        static constexpr int getTypesCount();

        template <typename T>
        static constexpr int pos(); // get type position id in pack

        static constexpr int empty();

        static const TypeInfo& getTypeInfo(int pos);

        // creates mapping array for internal type ids of domain to type ids in pack
        template <typename Domain>
        static void mapIds(fast_vector<u32>& ids_mapper_out);

        template <typename... Tss>
        static TypesPack<F, R...,Tss...> expand();  // expand with types


        /* functor must be:
            struct Functor {
                template <typename TypesPack, typename Type>
                static void f(args) {}
            }
        */
        template <typename Functor, typename... Args>
        static void callOnTypes(Args&&... args);
    private:
        template <typename TPOrig, typename TP, typename Functor, typename... Args>
        static IF_EMPTY(TP) callOnInner_(Args&&... args);

        template <typename TPOrig, typename TP, typename Functor, typename... Args>
        static IF_NOT_EMPTY(TP) callOnInner_(Args&&... args);
    };


    // usage:
    //   typedef TypesPackMerge<TP1, TP2>::Types MyTypesPack;
    template <typename... Ts> struct TypesPackMerge;
    template <typename... Ts1, typename... Ts2>
    struct TypesPackMerge<TypesPack<Ts1...>, TypesPack<Ts2...> > {
        typedef TypesPack<Ts1..., Ts2...> Types;
    };

    // Wraps types in TypesPack with another Type W
    template <template<typename> class W, typename TP> struct TypesPackWrap;
    template <template<typename> class W, typename... Ts >
    struct TypesPackWrap<W, TypesPack<Ts...> > {
        typedef TypesPack<W<Ts>... > Types;
    };

}
#endif //TYPESPACK_H

#if !(defined(TYPES_PACK_INL)) && !defined(WITHOUT_IMPL)
# define TYPES_PACK_INL
# include "TypesPack.inl"
#endif  // TYPES_PACK_INL
