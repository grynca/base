#ifndef UNSORTED_VECTOR_TEST_H
#define UNSORTED_VECTOR_TEST_H

#include <unordered_map>

namespace grynca { namespace test_containers {
    inline constexpr uint32_t n() {
        return 1e5;
    }

    inline fast_vector<unsigned int> randomDestructionOrder() {
        fast_vector<unsigned int> destruction_order;
        destruction_order.reserve(n());
        randomPickN(destruction_order, n(), n());
        return destruction_order;
    }

    inline void testStdMap() {
        std::unordered_map<uint32_t, MyStuff> stuff_map;
        stuff_map.reserve(n());
        fast_vector<unsigned int> destruction_order = randomDestructionOrder();

        std::cout << "std::map: " << std::endl;
        {
            BlockMeasure m(" creation");
            for (size_t i=0; i<n(); ++i) {
                stuff_map[i] = MyStuff();
            }
        }

        {
            BlockMeasure m(" loop");
            for (size_t i=0; i<n(); ++i) {
                stuff_map[i].dostuff();
            }
        }

        {
            BlockMeasure m(" destruction");
            for (size_t i=0; i<n(); ++i) {
                stuff_map.erase(destruction_order[i]);
            }
        }
    }

    inline void testUnsortedVector() {
        UnsortedVector<MyStuff> stuff_vec;
        stuff_vec.reserve(n());
        fast_vector<unsigned int> destruction_order = randomDestructionOrder();

        std::cout << "UnsortedVector: " << std::endl;
        {
            BlockMeasure m(" creation");
            for (size_t i=0; i<n(); ++i) {
                stuff_vec.add();
            }
        }

        {
            BlockMeasure m(" loop");
            for (size_t i=0; i<n(); ++i) {
                stuff_vec.get(i).dostuff();
            }
        }

        {
            BlockMeasure m(" destruction");
            for (size_t i=0; i<n(); ++i) {
                stuff_vec.remove(destruction_order[i]);
            }
        }
    }

    inline void testVirtualVector() {
        VVector<MyDomain> vv;
        vv.reserve(n());

        std::cout << "VVector:" << std::endl;
        {
            BlockMeasure m(" creation");
            for (size_t i=0; i<n(); ++i) {
                switch (rand()%3) {
                    case 0:
                        vv.add<MyStuff>();
                        break;
                    case 1:
                        vv.add<MyStuffA>();
                        break;
                    case 2:
                        vv.add<MyStuffB>();
                        break;
                }
            }
        }

        {
            BlockMeasure m(" loop");
            for (size_t i=0; i<n(); ++i) {
                ((MyStuff*)vv.get(i))->dostuff();
            }
        }

        {
            BlockMeasure m(" destruction");
            for (size_t i=0; i<n(); ++i) {
                uint32_t id = rand()%vv.getSize();
                vv.remove(id);
            }
        }
    }

}}


#endif //UNSORTED_VECTOR_TEST_H
