#ifndef TEST_VARIANTS_H
#define TEST_VARIANTS_H

namespace grynca { namespace test_variants {

    inline constexpr u32 n() {
        return 1e5;
    }


    TEMPLATED_FUNCTOR(StuffCaller2,(T* t) {
        t->dostuff();
    });

    struct Test {
        static void f(void*, Config::ConfigSectionMap&) {
            std::cout <<"Variants " << n() << ":" << std::endl;

            fast_vector<MyStuffVariant> rvar;
            {
                BlockMeasure m(" create:");
                std::cout << "Variant size: " << sizeof(MyStuffVariant) << std::endl;
                rvar.resize(n());
                for (size_t i=0; i<n(); ++i) {
                    u32 type_id = u32(rand()%MyStuffVariant::Types::getTypesCount());
                    rvar[i].setByTypeId(type_id);
                }
            }

            {
                BlockMeasure m(" loop:");
                for (size_t i=0; i<n(); ++i) {
                    rvar[i].callFunctor<StuffCaller2>();
                }
            }

            {
                BlockMeasure m(" loop cond:");
                for (size_t i=0; i<n(); ++i) {
                    rvar[i].callFunctorCond<HasPropTrait<props::b_t>, StuffCaller2>();
                }
            }

            {
                BlockMeasure m(" delete:");
                rvar.clear();
            }
        }
    };
}}

#endif //TEST_VARIANTS_H
