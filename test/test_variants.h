#ifndef TEST_VARIANTS_H
#define TEST_VARIANTS_H

namespace grynca { namespace test_variants {

    inline constexpr uint32_t n() {
        return 1e5;
    }

    struct StuffCaller {
        template <typename T>
        static void f(MyStuffVariant& v, T& t) {
            t.dostuff();
        }
    };

    inline void testVariants() {
        fast_vector<MyStuffVariant> rvar;
        rvar.reserve(n());

        std::cout <<"Variants:" << std::endl;
        {
            BlockMeasure m(" create:");

            for (size_t i=0; i<n(); ++i) {
                rvar.push_back();
                switch (rand()%3) {
                    case 0:
                        rvar.back().set<MyStuff>();
                        break;
                    case 1:
                        rvar.back().set<MyStuffA>();
                        break;
                    case 2:
                        rvar.back().set<MyStuffB>();
                        break;
                }
            }
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
