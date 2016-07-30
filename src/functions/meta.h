#ifndef META_H
#define META_H

#include <cstddef>
#include <type_traits>

#define ARRAY_SIZE(array) (sizeof(array)/sizeof(*array))

/* This counts the number of args (does not work for 0 args) */
#define NARGS_SEQ(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10, N,...) N
#define NARGS(...) NARGS_SEQ(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

/* This will let macros expand before concating them */
#define PRIMITIVE_CAT(x, y) x ## y
#define CAT(x, y) PRIMITIVE_CAT(x, y)

/* This will call a macro on each argument passed in */
#define APPLY(macro, ...) CAT(APPLY_, NARGS(__VA_ARGS__))(macro, __VA_ARGS__)
#define APPLY_1(m, x1) m(x1)
#define APPLY_2(m, x1, x2) m(x1), m(x2)
#define APPLY_3(m, x1, x2, x3) m(x1), m(x2), m(x3)
#define APPLY_4(m, x1, x2, x3, x4) m(x1), m(x2), m(x3), m(x4)
#define APPLY_5(m, x1, x2, x3, x4, x5) m(x1), m(x2), m(x3), m(x4), m(x5)
#define APPLY_6(m, x1, x2, x3, x4, x5, x6) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6)
#define APPLY_7(m, x1, x2, x3, x4, x5, x6, x7) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7)
#define APPLY_8(m, x1, x2, x3, x4, x5, x6, x7, x8) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8)
#define APPLY_9(m, x1, x2, x3, x4, x5, x6, x7, x8, x9) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9)
#define APPLY_10(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10)

#define AND_ALL(macro, ...) CAT(AND_ALL_, NARGS(__VA_ARGS__))(macro, __VA_ARGS__)
#define AND_ALL_1(m, x1) m(x1)
#define AND_ALL_2(m, x1, x2) m(x1)&& m(x2)
#define AND_ALL_3(m, x1, x2, x3) m(x1)&& m(x2)&& m(x3)
#define AND_ALL_4(m, x1, x2, x3, x4) m(x1)&& m(x2)&& m(x3)&& m(x4)
#define AND_ALL_5(m, x1, x2, x3, x4, x5) m(x1)&& m(x2)&& m(x3)&& m(x4)&& m(x5)
#define AND_ALL_6(m, x1, x2, x3, x4, x5, x6) m(x1)&& m(x2)&& m(x3)&& m(x4)&& m(x5)&& m(x6)
#define AND_ALL_7(m, x1, x2, x3, x4, x5, x6, x7) m(x1)&& m(x2)&& m(x3)&& m(x4)&& m(x5)&& m(x6)&& m(x7)
#define AND_ALL_8(m, x1, x2, x3, x4, x5, x6, x7, x8) m(x1)&& m(x2)&& m(x3)&& m(x4)&& m(x5)&& m(x6)&& m(x7)&& m(x8)
#define AND_ALL_9(m, x1, x2, x3, x4, x5, x6, x7, x8, x9) m(x1)&& m(x2)&& m(x3)&& m(x4)&& m(x5)&& m(x6)&& m(x7)&& m(x8)&& m(x9)
#define AND_ALL_10(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10) m(x1)&& m(x2)&& m(x3)&& m(x4)&& m(x5)&& m(x6)&& m(x7)&& m(x8)&& m(x9)&& m(x10)

#define NONE(S) S
#define DEFINE_ENUM(NAME, ...) struct NAME { enum { APPLY(NONE, __VA_ARGS__), end };};
#define DEFINE_ENUM_E(NAME, BASE, FIRST, ...) struct NAME: public BASE { enum { FIRST = BASE::end, APPLY(NONE,##__VA_ARGS__, end)};};

// works correctly only on static member functions
#define DECLARE_PROP(PROP) \
template<class T> \
using PROP##_t = decltype(std::declval<T>().PROP); //
\
//template<class T> \
//using has_##PROP = grynca::can_apply<PROP##_t, T>

namespace grynca {

    enum class enabler_t {};

    template<typename T>
    using EnableIf = typename std::enable_if<T::value, enabler_t>::type;

// e.g. static_max<int, double, MyType>::value
    template <size_t arg1, size_t ... others>  struct static_max;

    template <size_t arg>
    struct static_max<arg>
    {
        static const size_t value = arg;
    };

    template <size_t arg1, size_t arg2, size_t ... others>
    struct static_max<arg1, arg2, others...>
    {
        static const size_t value = arg1 >= arg2 ? static_max<arg1, others...>::value :
                                    static_max<arg2, others...>::value;
    };

// e.g. position<int, MyType, bool, int>::pos == 2
    template <typename X, typename... Ts>
    struct position;

    template <typename X>
    struct position<X> {
        static const int pos = -1;
    };

    template <typename X, typename ... Ts>
    struct position<X, X, Ts...> {
        static const int pos = 0;
    };

    template <typename X, typename T, typename... Ts>
    struct position<X, T, Ts...> {
        static const int pos = position<X, Ts...>::pos != -1
                               ?position<X, Ts...>::pos+1
                               :-1;
    };


    namespace details {
        template<class...>struct voider{using type=void;};
        template<class...Ts>using void_t=typename voider<Ts...>::type;

        template<template<class...>class Z, class, class...Ts>
        struct can_apply: std::false_type {};
        template<template<class...>class Z, class...Ts>
        struct can_apply<Z, void_t<Z<Ts...> >, Ts...> : std::true_type {};
    }

    template<template<class...>class Z, class...Ts>
    using can_apply=details::can_apply<Z,void,Ts...>;

    struct TrueTrait {
        static constexpr bool value = true;
    };

    template <template<class> class Prop>
    struct HasPropTrait {

        template <class T>
        struct apply {
            static constexpr bool value = grynca::can_apply<Prop, T&>();
        };

    };

    template<class... Traits>
    struct CondAll;

    template <>
    struct CondAll<> {

        template <typename T>
        struct apply {
            static constexpr bool value = true;
        };
    };

    template<class FirstTrait, class... OtherTraits>
    struct CondAll<FirstTrait, OtherTraits...> {

        template <typename T>
        struct apply {
            static constexpr bool value = FirstTrait::template apply<T>::value && CondAll<OtherTraits...>::template apply<T>::value;
        };
    };

    template <typename T> int sgn(T val) {
        return (T(0) < val) - (val < T(0));
    }
}

#endif //META_H
