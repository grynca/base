#ifndef META_H
#define META_H

#include <cstddef>
#include <type_traits>
#include "../types/Mask.h"

#define kilobytes(n) u32(1024*(n))
#define megabytes(n) (1024*kilobytes(n))
#define gigabytes(n) (1024*megabytes(n))

#define ONES(from, to) (((1<<(to-from+1))-1)<<from)
#define ZEROS(from, to) (~ONES(from, to))

#define GET_BIT(num, b_id) (bool)(((num)>>(b_id))&1)
#define SET_BIT(num, b_id) (decltype(num))((num)|(1<<(b_id)))
#define CLEAR_BIT(num, b_id) (decltype(num))((num)&~(1<<(b_id)))
#define TGL_BIT(num, b_id) (decltype(num))((num)^(1<<(b_id)))
#define SET_BITV(num, b_id, val)  (decltype(num))((num)^((-((int)(bool)val)^(num))&(1<<(b_id))))
#define GET_BITS(num, bit_from, bits_cnt)  ((num&ONES((bit_from), bit_from+bits_cnt-1))>>(bit_from))
#define SET_BITS(num, bit_from, bits_cnt, val) ((num&ZEROS((bit_from), bit_from+bits_cnt-1)) | (val << (bit_from))); \
                                                ASSERT( val < (1<<(bits_cnt)) )


#define ARRAY_SIZE(array) (sizeof(array)/sizeof(*array))

/* This counts the number of args (does not work for 0 args) */
#define NARGS_SEQ(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10, N,...) N
#define NARGS(...) NARGS_SEQ(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

/* This will let macros expand before concating them */
#define PRIMITIVE_CAT(x, y) x ## y
#define CAT(x, y) PRIMITIVE_CAT(x, y)

/* This will call a macro on each argument passed in */
// with space in between
#define APPLY_SPACE(macro, ...) CAT(APPLY_SPACE_, NARGS(__VA_ARGS__))(macro, __VA_ARGS__)
#define APPLY_SPACE_1(m, x1) m(x1)
#define APPLY_SPACE_2(m, x1, x2) m(x1) m(x2)
#define APPLY_SPACE_3(m, x1, x2, x3) m(x1) m(x2) m(x3)
#define APPLY_SPACE_4(m, x1, x2, x3, x4) m(x1) m(x2) m(x3) m(x4)
#define APPLY_SPACE_5(m, x1, x2, x3, x4, x5) m(x1) m(x2) m(x3) m(x4) m(x5)
#define APPLY_SPACE_6(m, x1, x2, x3, x4, x5, x6) m(x1) m(x2) m(x3) m(x4) m(x5) m(x6)
#define APPLY_SPACE_7(m, x1, x2, x3, x4, x5, x6, x7) m(x1) m(x2) m(x3) m(x4) m(x5) m(x6) m(x7)
#define APPLY_SPACE_8(m, x1, x2, x3, x4, x5, x6, x7, x8) m(x1) m(x2) m(x3) m(x4) m(x5) m(x6) m(x7) m(x8)
#define APPLY_SPACE_9(m, x1, x2, x3, x4, x5, x6, x7, x8, x9) m(x1) m(x2) m(x3) m(x4) m(x5) m(x6) m(x7) m(x8) m(x9)
#define APPLY_SPACE_10(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10) m(x1) m(x2) m(x3) m(x4) m(x5) m(x6) m(x7) m(x8) m(x9) m(x10)

// with comma in between
#define APPLY_COMMA(macro, ...) CAT(APPLY_COMMA_, NARGS(__VA_ARGS__))(macro, __VA_ARGS__)
#define APPLY_COMMA_1(m, x1) m(x1)
#define APPLY_COMMA_2(m, x1, x2) m(x1), m(x2)
#define APPLY_COMMA_3(m, x1, x2, x3) m(x1), m(x2), m(x3)
#define APPLY_COMMA_4(m, x1, x2, x3, x4) m(x1), m(x2), m(x3), m(x4)
#define APPLY_COMMA_5(m, x1, x2, x3, x4, x5) m(x1), m(x2), m(x3), m(x4), m(x5)
#define APPLY_COMMA_6(m, x1, x2, x3, x4, x5, x6) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6)
#define APPLY_COMMA_7(m, x1, x2, x3, x4, x5, x6, x7) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7)
#define APPLY_COMMA_8(m, x1, x2, x3, x4, x5, x6, x7, x8) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8)
#define APPLY_COMMA_9(m, x1, x2, x3, x4, x5, x6, x7, x8, x9) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9)
#define APPLY_COMMA_10(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10)

// with && in between
#define APPLY_AND_ALL(macro, ...) CAT(APPLY_AND_ALL_, NARGS(__VA_ARGS__))(macro, __VA_ARGS__)
#define APPLY_AND_ALL_1(m, x1) m(x1)
#define APPLY_AND_ALL_2(m, x1, x2) m(x1)&& m(x2)
#define APPLY_AND_ALL_3(m, x1, x2, x3) m(x1)&& m(x2)&& m(x3)
#define APPLY_AND_ALL_4(m, x1, x2, x3, x4) m(x1)&& m(x2)&& m(x3)&& m(x4)
#define APPLY_AND_ALL_5(m, x1, x2, x3, x4, x5) m(x1)&& m(x2)&& m(x3)&& m(x4)&& m(x5)
#define APPLY_AND_ALL_6(m, x1, x2, x3, x4, x5, x6) m(x1)&& m(x2)&& m(x3)&& m(x4)&& m(x5)&& m(x6)
#define APPLY_AND_ALL_7(m, x1, x2, x3, x4, x5, x6, x7) m(x1)&& m(x2)&& m(x3)&& m(x4)&& m(x5)&& m(x6)&& m(x7)
#define APPLY_AND_ALL_8(m, x1, x2, x3, x4, x5, x6, x7, x8) m(x1)&& m(x2)&& m(x3)&& m(x4)&& m(x5)&& m(x6)&& m(x7)&& m(x8)
#define APPLY_AND_ALL_9(m, x1, x2, x3, x4, x5, x6, x7, x8, x9) m(x1)&& m(x2)&& m(x3)&& m(x4)&& m(x5)&& m(x6)&& m(x7)&& m(x8)&& m(x9)
#define APPLY_AND_ALL_10(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10) m(x1)&& m(x2)&& m(x3)&& m(x4)&& m(x5)&& m(x6)&& m(x7)&& m(x8)&& m(x9)&& m(x10)

#define _ENUM_ID(NAME) NAME##Id
#define _DEF_ENUM_MASK(NAME)  \
    static const Mask<end>& NAME##Mask() { \
        static Mask<end> v{NAME##Id}; \
        return v; \
    }
#define DEFINE_ENUM(NAME, ...) \
    struct NAME { \
        enum { \
            APPLY_COMMA(_ENUM_ID, __VA_ARGS__), \
            end \
        }; \
        APPLY_SPACE(_DEF_ENUM_MASK, __VA_ARGS__) \
    };
#define DEFINE_ENUM_E(NAME, BASE, FIRST, ...) \
    struct NAME: public BASE { \
        enum { \
            _ENUM_ID(FIRST) = BASE::end, \
            APPLY_COMMA(_ENUM_ID, __VA_ARGS__), \
            end \
        }; \
        _DEF_ENUM_MASK(FIRST) \
        APPLY_SPACE(_DEF_ENUM_MASK, __VA_ARGS__) \
    };

// works correctly only on static member functions
#define DECLARE_PROP(PROP) \
template<class T> \
using PROP##_t = decltype(std::declval<T>().PROP);

namespace grynca {

    enum class enabler_t {};

    template<typename T>
    using EnableIf = typename std::enable_if<T::value, enabler_t>::type;

// e.g. static_max<int, f64, MyType>::value
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
