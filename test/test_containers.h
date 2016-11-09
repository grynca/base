#ifndef UNSORTED_VECTOR_TEST_H
#define UNSORTED_VECTOR_TEST_H

#include <unordered_map>

namespace grynca { namespace test_containers {
    inline constexpr u32 n() {
        return 1e6;
    }

    inline fast_vector<unsigned int> randomDestructionOrder() {
        fast_vector<unsigned int> destruction_order;
        destruction_order.reserve(n());
        randomPickN(destruction_order, n(), n());
        return destruction_order;
    }

    struct Hasher {
        size_t operator()(const u32& key)const {
            return calcHash32(key);
        }
    };

    inline void testStdMap() {
        std::unordered_map<u32, MyStuff, Hasher> stuff_map;
        stuff_map.reserve(n());
        fast_vector<unsigned int> destruction_order = randomDestructionOrder();

        std::cout << "std::unordered_map " << n() << ":" << std::endl;
        {
            BlockMeasure m(" creation");
            for (size_t i=0; i<n(); ++i) {
                stuff_map[i] = MyStuff();
            }
        }

        {
            BlockMeasure m(" loop (iter)");
            for (auto it = stuff_map.begin(); it != stuff_map.end(); ++it) {
                it->second.dostuff();
            }
        }
        {
            BlockMeasure m(" loop (index)");
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

    inline void testHashMap() {
        HashMap<MyStuff, u32, Hasher> hm;
        fast_vector<u32> indices;
        hm.reserve(n());
        indices.reserve(n());
        fast_vector<unsigned int> destruction_order = randomDestructionOrder();

        std::cout << "HashMap " << n() << ":" << std::endl;
        {
            BlockMeasure m(" creation");
            for (size_t i=0; i<n(); ++i) {
                new (hm.addItem(i)) MyStuff();
                indices.push_back(i);
            }
        }

        {
            BlockMeasure m(" loop(index)");
            for (size_t i=0; i<n(); ++i) {
                MyStuff& stuff = hm.getItem(i);
                stuff.dostuff();
            }
        }

        {
            BlockMeasure m(" loop(w hashing)");
            for (size_t i=0; i<n(); ++i) {
                hm.getItem(hm.findItem(i)).dostuff();
            }
        }

        {
            BlockMeasure m(" destruction");
            for (size_t i=0; i<n(); ++i) {
                hm.removeItem(indices[destruction_order[i]]);
            }
        }
    }

    inline void testArray() {
        Array<MyStuff> stuff_vec;
        fast_vector<Index> indices;
        stuff_vec.reserve(n());
        indices.reserve(n());
        fast_vector<unsigned int> destruction_order = randomDestructionOrder();

        std::cout << "Array " << n() << ":" << std::endl;
        {
            BlockMeasure m(" creation");
            for (size_t i=0; i<n(); ++i) {
                indices.push_back(stuff_vec.add());
            }
        }

        {
            BlockMeasure m(" loop");
            for (size_t i=0; i<n(); ++i) {
                MyStuff* stuff = stuff_vec.getAtPos(i);
                // must check if not NULL (can contain holes)
                if (stuff)
                    stuff->dostuff();
            }
        }

        {
            BlockMeasure m(" destruction");
            for (size_t i=0; i<n(); ++i) {
                stuff_vec.remove(indices[destruction_order[i]]);
            }
        }
    }

    inline void testTightArray() {
        TightArray<MyStuff> stuff_vec;
        fast_vector<Index> indices;
        stuff_vec.reserve(n());
        indices.reserve(n());
        fast_vector<unsigned int> destruction_order = randomDestructionOrder();

        std::cout << "Tight Array " << n() << ":" << std::endl;
        {
            BlockMeasure m(" creation");
            for (size_t i=0; i<n(); ++i) {
                indices.push_back(stuff_vec.add());
            }
        }

        {
            BlockMeasure m(" loop");
            for (size_t i=0; i<n(); ++i) {
                // no need to check for NULL ... cant contain holes
                stuff_vec.getAtPos(i)->dostuff();
            }
        }

        {
            BlockMeasure m(" destruction");
            for (size_t i=0; i<n(); ++i) {
                stuff_vec.remove(indices[destruction_order[i]]);
            }
        }
    }

    inline void testMultiPool() {
        MultiPool<3, MyDomain> mp;
        fast_vector<Index> indices;
        mp.initComponents<StuffTypes>();
        mp.reserve(n());
        indices.reserve(n());
        fast_vector<unsigned int> destruction_order = randomDestructionOrder();

        std::cout << "Multi Pool " << n() << ":" << std::endl;
        {
            BlockMeasure m(" creation");
            for (size_t i=0; i<n(); ++i) {
                indices.push_back(mp.addAndConstruct());
            }
        }

        {
            BlockMeasure m(" loop");
            for (size_t i=0; i<n(); ++i) {
                if (!mp.isFree(i)) {
                    ((MyStuff*)mp.getAtPos(i, 0))->dostuff();
                    ((MyStuffA*)mp.getAtPos(i, 1))->dostuff();
                    ((MyStuffB*)mp.getAtPos(i, 2))->dostuff();
                }
            }
        }

        {
            BlockMeasure m(" destruction");
            for (size_t i=0; i<n(); ++i) {
                mp.remove(indices[destruction_order[i]]);
            }
        }
    }

    inline void testVirtualVector() {
        VVector<MyDomain> vv;
        vv.reserve(n());
        vv.addAs<MyStuffA, MyStuff>();

        std::cout << "VVector " << n() << ":" << std::endl;
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
                u32 id = rand()%vv.getSize();
                vv.remove(id);
            }
        }
    }
}}


#endif //UNSORTED_VECTOR_TEST_H
