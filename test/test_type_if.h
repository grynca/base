#ifndef TEST_TYPE_IF_H
#define TEST_TYPE_IF_H

namespace grynca { namespace test_type_if {

    namespace props {
        DECLARE_PROP(a);
        DECLARE_PROP(b);
        DECLARE_PROP(c);
    }

    struct YesFunctor {
        static void f() {
            std::cout << " Yes." << std::endl;
        }
    };

    struct NoFunctor {
        static void f() {
            std::cout << " No." << std::endl;
        }
    };

    template <typename Cond>
    inline void checkIf(const std::string& msg) {
        std::cout << msg << std::endl;
        Call<YesFunctor>::ifTrue<Cond>();
        Call<NoFunctor>::ifFalse<Cond>();
    }

    inline void test() {
        checkIf<std::is_same<MyStuff, MyStuff> >("is MyStuff same as MyStuff?");
        checkIf<HasProps<MyStuff, props::has_a> >("has MyStuff 'a' property?");
        checkIf<HasProps<MyStuff, props::has_b> >("has MyStuff 'b' property?");
        checkIf<HasProps<MyStuffB, props::has_b, props::has_c> >("has MyStuffB 'b' and 'c' properties?");
        checkIf<HasProps<MyStuffA, props::has_b, props::has_c> >("has MyStuffA 'b' and 'c' properties?");
    }
}}

#endif //TEST_TYPE_IF_H
