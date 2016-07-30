#ifndef TEST_TYPE_IF_H
#define TEST_TYPE_IF_H

namespace grynca { namespace test_type_if {

    struct YesFunctor {
        template <typename T = void>
        static void f() {
            std::cout << "  Yes." << std::endl;
        }
    };

    struct NoFunctor {
        template <typename T = void>
        static void f() {
            std::cout << "  No." << std::endl;
        }
    };

    template <typename Cond>
    inline void checkIf(const std::string& msg) {
        std::cout << msg << std::endl;
        Call<YesFunctor>::ifTrue<Cond>();
        Call<NoFunctor>::ifFalse<Cond>();
    }
    class BLA { BLA(int a) {} };

    inline void test() {
        checkIf<std::is_same<MyStuff, MyStuff> >("is MyStuff same as MyStuff?");
        checkIf<HasPropTrait<props::a_t>::apply<MyStuff> >("has MyStuff 'a' property?");
        checkIf<HasPropTrait<props::b_t>::apply<MyStuff> >("has MyStuff 'b' property?");
        checkIf<CondAll<HasPropTrait<props::b_t>, HasPropTrait<props::c_t> >::apply<MyStuffB> >("has MyStuffB 'b' and 'c' properties?");
        checkIf<CondAll<HasPropTrait<props::b_t>, HasPropTrait<props::c_t> >::apply<MyStuffA> >("has MyStuffA 'b' and 'c' properties?");

        std::cout << "Loop all Variant types, has 'c' property?:" << std::endl;
        VariantTypesLooper<YesFunctor, NoFunctor>::callCond<MyStuffVariant, HasPropTrait<props::c_t> >();
    }
}}

#endif //TEST_TYPE_IF_H
