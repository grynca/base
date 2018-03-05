#ifndef META_H
#define META_H

#include <climits>
#include <cstddef>
#include <type_traits>

#if __GNUC__
#   if __x86_64__ || __ppc64__
#       define ENV64 1
#       define ENV32 0
#   else
#       define ENV64 0
#       define ENV32 1
#   endif
#endif

#define UNUSED(x) ((void)(x))

#define kilobytes(n) u32(1024*(n))
#define megabytes(n) (1024*kilobytes(n))
#define gigabytes(n) (1024*megabytes(n))

// Returns the number of bits in the given type
#define BITS_IN_TYPE(t)	(sizeof(t) * CHAR_BIT)

// positive modulo, M must be power of 2
#define PMOD(VAL, M) ((VAL)&(M-1))

#define BIT_MASK(b_id) (1<<(b_id))
#define ONES(from, to) (((u64(1)<<(to-from))-1)<<from)
#define ZEROS(from, to) (~ONES(from, to))
#define GET_BIT(num, b_id) (bool)(((num)>>(b_id))&1)
#define SET_BIT(num, b_id) (decltype(num))((num)|(1<<(b_id)))
#define SET_BITS2(num, b_id1, b_id2) (decltype(num))((num)|(1<<(b_id1))|(1<<(b_id2)))
#define SET_BITS3(num, b_id1, b_id2, b_id3) (decltype(num))((num)|(1<<(b_id1))|(1<<(b_id2))|(1<<(b_id3)))
#define CLEAR_BIT(num, b_id) (decltype(num))((num)&~(1<<(b_id)))
#define TGL_BIT(num, b_id) (decltype(num))((num)^(1<<(b_id)))
#define SET_BITV(num, b_id, val)  (decltype(num))((num)^((-((int)(bool)val)^(num))&(1<<(b_id))))
#define GET_BITS(num, bit_from, bits_cnt)  ((num&ONES((bit_from), bit_from+bits_cnt))>>(bit_from))
#define SET_BITS(num, bit_from, bits_cnt, val) ((num&ZEROS((bit_from), bit_from+bits_cnt)) | (val << (bit_from))); \
                                                ASSERT( val < (1<<(bits_cnt)) )

/// Returns the mask of type \p t with the lowest \p n bits set.
#define LSB_MASK(t,n)	(t(~t(0)) >> (BITS_IN_TYPE(t) - (n)))

// float decimal precision
#define FPR(num, digs) f32(i32((num)*(1e##digs))/(1e##digs))

#define ARRAY_SIZE(array) (sizeof(array)/sizeof(*array))

/* This counts the number of args (does not work for 0 args) */
#define NARGS_SEQ(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11, _12,_13,_14,_15, N,...) N
#define NARGS(...) NARGS_SEQ(__VA_ARGS__, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

/* This will let macros expand before concating them */
#define PRIMITIVE_CAT(x, y) x ## y
#define CAT(x, y) PRIMITIVE_CAT(x, y)

#define SINGLE_ARG(...) __VA_ARGS__

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
#define APPLY_SPACE_11(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11) m(x1) m(x2) m(x3) m(x4) m(x5) m(x6) m(x7) m(x8) m(x9) m(x10) m(x11)
#define APPLY_SPACE_12(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12) m(x1) m(x2) m(x3) m(x4) m(x5) m(x6) m(x7) m(x8) m(x9) m(x10) m(x11) m(x12)
#define APPLY_SPACE_13(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13) m(x1) m(x2) m(x3) m(x4) m(x5) m(x6) m(x7) m(x8) m(x9) m(x10) m(x11) m(x12) m(x13)
#define APPLY_SPACE_14(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14) m(x1) m(x2) m(x3) m(x4) m(x5) m(x6) m(x7) m(x8) m(x9) m(x10) m(x11) m(x12) m(x13) m(x14)
#define APPLY_SPACE_15(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15) m(x1) m(x2) m(x3) m(x4) m(x5) m(x6) m(x7) m(x8) m(x9) m(x10) m(x11) m(x12) m(x13) m(x14) m(x15)

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
#define APPLY_COMMA_11(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10), m(x11)
#define APPLY_COMMA_12(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10), m(x11), m(x12)
#define APPLY_COMMA_13(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10), m(x11), m(x12), m(x13)
#define APPLY_COMMA_14(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10), m(x11), m(x12), m(x13), m(x14)
#define APPLY_COMMA_15(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15) m(x1), m(x2), m(x3), m(x4), m(x5), m(x6), m(x7), m(x8), m(x9), m(x10), m(x11), m(x12), m(x13), m(x14), m(x15)

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
#define APPLY_AND_ALL_11(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11) m(x1)&& m(x2)&& m(x3)&& m(x4)&& m(x5)&& m(x6)&& m(x7)&& m(x8)&& m(x9)&& m(x10)&& m(x11)
#define APPLY_AND_ALL_12(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x12) m(x1)&& m(x2)&& m(x3)&& m(x4)&& m(x5)&& m(x6)&& m(x7)&& m(x8)&& m(x9)&& m(x10)&& m(x11)&& m(x12)
#define APPLY_AND_ALL_13(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x12, x13) m(x1)&& m(x2)&& m(x3)&& m(x4)&& m(x5)&& m(x6)&& m(x7)&& m(x8)&& m(x9)&& m(x10)&& m(x11)&& m(x12)&& m(x13)
#define APPLY_AND_ALL_14(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x12, x13, x14) m(x1)&& m(x2)&& m(x3)&& m(x4)&& m(x5)&& m(x6)&& m(x7)&& m(x8)&& m(x9)&& m(x10)&& m(x11)&& m(x12)&& m(x13)&& m(x14)
#define APPLY_AND_ALL_15(m, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x12, x13, x14, x15) m(x1)&& m(x2)&& m(x3)&& m(x4)&& m(x5)&& m(x6)&& m(x7)&& m(x8)&& m(x9)&& m(x10)&& m(x11)&& m(x12)&& m(x13)&& m(x14)&& m(x15)

#define MAKE_PARAMS_0()
#define MAKE_PARAMS_1(type) type arg1
#define MAKE_PARAMS_2(type1, type2) type1 arg1, type2 arg2
#define MAKE_PARAMS_3(type1, type2, type3) type1 arg1, type2 arg2, type3 arg3
#define MAKE_PARAMS_4(type1, type2, type3, type4) type1 arg1, type2 arg2, type3 arg3, type4 arg4
#define MAKE_PARAMS_5(type1, type2, type3, type4, type5) type1 arg1, type2 arg2, type3 arg3, type4 arg4, type5 arg5
//.. add as many MAKE_PARAMS_* as you need

#define MAKE_PARAMS_N(N, ...) MAKE_PARAMS_##N(__VA_ARGS__)
#define MAKE_PARAMS_FORCE_N(N, ...) MAKE_PARAMS_N(N, __VA_ARGS__)
#define MAKE_PARAMS(...) MAKE_PARAMS_FORCE_N(NARGS(__VA_ARGS__), __VA_ARGS__)


#define MAKE_ARGS_0()
#define MAKE_ARGS_1(t) arg1
#define MAKE_ARGS_2(t1, t2) arg1, arg2
#define MAKE_ARGS_3(t1, t2, t3) arg1, arg2, arg3
#define MAKE_ARGS_4(t1, t2, t3, t4) arg1, arg2, arg3, arg4
#define MAKE_ARGS_5(t1, t2, t3, t4, t5) arg1, arg2, arg3, arg4, arg5
//.. add as many MAKE_ARGS_* as you have MAKE_PARAMS_*

#define MAKE_ARGS_N(N, ...) MAKE_ARGS_##N(__VA_ARGS__)
#define MAKE_ARGS_FORCE_N(N, ...) MAKE_ARGS_N(N, __VA_ARGS__)
#define MAKE_ARGS(...) MAKE_ARGS_FORCE_N(NARGS(__VA_ARGS__), __VA_ARGS__)


// good for preprocessor messages
// e.g.: #pragma message "The value of ABC: " STR(ABC)
#define STR(x) _XSTR(x)
#define _XSTR(x) #x

#define OBJ_TYPE_FROM_PTR(ptr) std::remove_reference<decltype(*(ptr))>::type

#define DISALLOW_COPY_AND_ASSIGN(T) \
  T(const T&) = delete;      \
  void operator=(const T&) = delete

#define MOVE_ONLY(T) \
    DISALLOW_COPY_AND_ASSIGN(T); \
    T(T&&) = default; \
    T& operator=(T&&) = default

#define DEF_CONSTR_AND_MOVE_ONLY(T) \
    MOVE_ONLY(T); \
    T() = default

#define INHERIT_CONSTRUCTORS(BASE) \
    using BASE::BASE

#define DECLARE_PROP_DATA(PROP) \
template<class T> \
using PROP##_t = decltype(std::declval<T>().PROP);

#define DECLARE_PROP_FUNC(PROP) \
template<class T> \
using PROP##_t = decltype(std::declval<T>().PROP());

// not exactly safe on non-POD structs/classes
#define GET_PARENT_STRUCT(STRUCTNAME, MEMBERNAME, MEMBERREF) \
    grynca::internal::getParentStruct<STRUCTNAME, offsetof(STRUCTNAME, MEMBERNAME)>(MEMBERREF)

#define COND_TYPEDEF(Cond, TypeIfTrue, TypeIfFalse, TypeDef) \
    typedef typename std::conditional<Cond, TypeIfTrue, TypeIfFalse>::type TypeDef;

namespace grynca {

    namespace internal {
        template <class Struct, size_t offset, class Member>
        Struct& getParentStruct(Member &m) {
            static_assert(std::is_standard_layout<Struct>::value, "Given struct must have a standard layout type");
            return *reinterpret_cast<Struct *>(reinterpret_cast<char *>(&m) - offset);
        }
    }

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

    /// Selects type result = flag ? T : U
    template <bool flag, typename T, typename U>
    struct select { typedef T result; };
    template <typename T, typename U>
    struct select<false, T, U> { typedef U result; };

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

    struct EmptyFunctor {
        template <typename... Args>
        static void f(Args&&...) {}
    };

    struct EmptyFunctorT {
        template <typename T, typename... Args>
        static void f(Args&&...) {}
    };


    template <bool RSLT = true>
    struct BoolFunctor {
        template <typename... Args>
        static bool f(Args&&...) { return RSLT; }
    };

    // signum function, returns [-1, 0, 1]
    template <typename T> int sgn(T val) {
        return (T(0) < val) - (val < T(0));
    }

    // 0 i always same sign
    template <typename T> bool sameSign(T val1, T val2) {
        return (val1*val2)>=0;
    }

    template <typename Derived, typename Base>
    inline i32 calcBaseOffset() {
        return (u8*)((Base*)((Derived*)0x10000000)) - (u8*)0x10000000;
    }

    template<class T>
    inline constexpr T static_pow(const T base, unsigned const exponent)
    {
        return (exponent == 0) ? 1 : (base * static_pow(base, exponent-1));
    }
}

#endif //META_H
