#ifndef TEST_VARIANTS_H
#define TEST_VARIANTS_H

namespace grynca { namespace test_variants {

    inline constexpr uint32_t n() {
        return 100;
    }

    struct StuffCaller {
        template <typename T>
        static void f(MyStuffVariant& v, T& t) {
            t.dostuff();
        }
    };

    inline void testVariants() {
        std::cout <<"Variants:" << std::endl;

        fast_vector<MyStuffVariant> rvar;
                {
            BlockMeasure m(" create:");
            std::cout << "Variant size: " << sizeof(MyStuffVariant) << std::endl;
            rvar = MyStuffVariant::generateN(n());
        }

        {
            BlockMeasure m(" loop:");
            for (size_t i=0; i<n(); ++i) {
                VariantCaller<StuffCaller>::call(rvar[i]);
            }
        }

        {
            BlockMeasure m(" delete:");
            rvar.clear();
        }
    }

}}

#endif //TEST_VARIANTS_H
