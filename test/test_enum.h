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

        DEFINE_ENUM(A, zero, one)
        DEFINE_ENUM_E(B, A, two, three)

        inline void test() {

            std::cout << "args " <<  NARGS() << std::endl;
            std::cout << "A::zero id:" << A::zeroId << ", m: " << A::zeroMask() << std::endl;
            std::cout << "A::one id:" << A::oneId << ", m: " << A::oneMask() << std::endl;
            std::cout << "A::end " << A::end << std::endl;
            std::cout << "B::zero id:" << B::zeroId << ", m: " << B::zeroMask() << std::endl;
            std::cout << "B::one id:" << B::oneId << ", m: " << B::oneMask() << std::endl;
            std::cout << "B::two id:" << B::twoId << ", m: " << B::twoMask() << std::endl;
            std::cout << "B::three id:" << B::threeId << ", m: " << B::threeMask() << std::endl;
            std::cout << "B::end " << B::end << std::endl;
        }
    }}

#endif //TEST_ENUM_H
