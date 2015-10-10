#ifndef TEST_ENUM_H
#define TEST_ENUM_H

namespace grynca { namespace test_enum {

/*
 * Expands to this:
    struct A {
        enum {
            zero,
            one,
            end
        };
    };

    struct B : public A {
        enum {
            two = A::end,
            three,
            end
        };
    };
*/

        DEFINE_ENUM(A, zero, one);
        DEFINE_ENUM_E(B, A, two, three);

        inline void test() {

            std::cout << "args " <<  NARGS() << std::endl;

            std::cout << "A::zero " << A::zero <<std::endl;
            std::cout << "A::one " << A::one <<std::endl;
            std::cout << "A::end " << A::end <<std::endl;
            std::cout << "B::zero " << B::zero <<std::endl;
            std::cout << "B::one " << B::one <<std::endl;
            std::cout << "B::two " << B::two <<std::endl;
            std::cout << "B::three " << B::three <<std::endl;
            std::cout << "B::end " << B::end <<std::endl;
        }
    }}

#endif //TEST_ENUM_H
