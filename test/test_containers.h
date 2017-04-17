#ifndef UNSORTED_VECTOR_TEST_H
#define UNSORTED_VECTOR_TEST_H

//#include <unordered_map>

namespace grynca { namespace test_containers {
        inline constexpr u32 n() {
            return 1e5;
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

        struct TestVector {
            static void f(void*, Config::ConfigSectionMap&) {
                //std::vector<MyStuff> vec;
                fast_vector<MyStuff> vec;

                {
                    MEASURE_BLOCK("creation");
                    for (size_t i=0; i<n(); ++i) {
                        vec.emplace_back();
                    }
                }

                {
                    MEASURE_BLOCK("loop(index)");
                    for (size_t i=0; i<n(); ++i) {
                        vec[i].dostuff();
                    }
                }

//                {
//                    BlockMeasure m(" destruction");
//                    for (size_t i=0; i<n(); ++i) {
//                        stuff_fv.erase(stuff_fv.begin());
//                    }
//                }
            }
        };


        struct TestHashMap {
            static void f(void*, Config::ConfigSectionMap&) {
                HashMap<MyStuff, u32, Hasher> hm;
                fast_vector<u32> indices;
                hm.reserve(n());
                indices.reserve(n());
                fast_vector<unsigned int> destruction_order = randomDestructionOrder();

                {
                    MEASURE_BLOCK("creation");
                    for (size_t i=0; i<n(); ++i) {
                        new (hm.addItem(i)) MyStuff();
                        indices.push_back(i);
                    }
                }

                {
                    MEASURE_BLOCK("loop(index)");
                    for (size_t i=0; i<n(); ++i) {
                        MyStuff& stuff = hm.getItem(i);
                        stuff.dostuff();
                    }
                }

                {
                    MEASURE_BLOCK("loop(w hashing)");
                    for (size_t i=0; i<n(); ++i) {
                        hm.findItem(i)->dostuff();
                    }
                }

                {
                    MEASURE_BLOCK("destruction");
                    for (size_t i=0; i<n(); ++i) {
                        hm.removeItem(indices[destruction_order[i]]);
                    }
                }
            }
        };

        struct TestArray {
            static void f(void*, Config::ConfigSectionMap&) {
                Array<MyStuff> stuff_vec;
                fast_vector<Index> indices;
                stuff_vec.reserve(n());
                indices.reserve(n());
                fast_vector<unsigned int> destruction_order = randomDestructionOrder();

                {
                    MEASURE_BLOCK("creation");
                    for (size_t i=0; i<n(); ++i) {
                        indices.push_back(stuff_vec.add());
                    }
                }

                {
                    MEASURE_BLOCK("loop");
                    for (size_t i=0; i<n(); ++i) {
                        MyStuff* stuff = stuff_vec.getAtPos(i);
                        // must check if not NULL (can contain holes)
                        if (stuff)
                            stuff->dostuff();
                    }
                }

                {
                    MEASURE_BLOCK("destruction");
                    for (size_t i=0; i<n(); ++i) {
                        stuff_vec.remove(indices[destruction_order[i]]);
                    }
                }
            }
        };

        struct TestTightArray {
            static void f(void*, Config::ConfigSectionMap&) {
                TightArray<MyStuff> stuff_vec;
                fast_vector<Index> indices;
                stuff_vec.reserve(n());
                indices.reserve(n());
                fast_vector<unsigned int> destruction_order = randomDestructionOrder();

                {
                    MEASURE_BLOCK("creation");
                    for (size_t i=0; i<n(); ++i) {
                        indices.push_back(stuff_vec.add());
                    }
                }

                {
                    MEASURE_BLOCK("loop");
                    for (size_t i=0; i<n(); ++i) {
                        // no need to check for NULL ... cant contain holes
                        stuff_vec.getAtPos(i)->dostuff();
                    }
                }

                {
                    MEASURE_BLOCK("destruction");
                    for (size_t i=0; i<n(); ++i) {
                        stuff_vec.remove(indices[destruction_order[i]]);
                    }
                }
            }
        };

        struct TestMultiPool {
            static void f(void*, Config::ConfigSectionMap&) {
                MultiPool<3, MyDomain> mp;
                fast_vector<Index> indices;
                mp.initComponents<StuffTypes>();
                mp.reserve(n());
                indices.reserve(n());
                fast_vector<u32> destruction_order = randomDestructionOrder();

                {
                    MEASURE_BLOCK("creation");
                    for (size_t i=0; i<n(); ++i) {
                        indices.push_back(mp.addAndConstruct());
                    }
                }

                {
                    MEASURE_BLOCK("loop");
                    for (size_t i=0; i<n(); ++i) {
                        if (!mp.isFree(i)) {
                            ((MyStuff*)mp.getAtPos(i, 0))->dostuff();
                            ((MyStuffA*)mp.getAtPos(i, 1))->dostuff();
                            ((MyStuffB*)mp.getAtPos(i, 2))->dostuff();
                        }
                    }
                }

                {
                    MEASURE_BLOCK("destruction");
                    for (size_t i=0; i<n(); ++i) {
                        mp.remove(indices[destruction_order[i]]);
                    }
                }
            }
        };

        struct TestVirtualVector {
            static void f(void*, Config::ConfigSectionMap&) {
                VVector<MyDomain> vv;
                vv.reserve(n());
                vv.addAs<MyStuffA, MyStuff>();

                {
                    MEASURE_BLOCK("creation");
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
                    MEASURE_BLOCK("loop");
                    for (size_t i=0; i<n(); ++i) {
                        ((MyStuff*)vv.get(i))->dostuff();
                    }
                }

                {
                    MEASURE_BLOCK("destruction");
                    for (size_t i=0; i<n(); ++i) {
                        u32 id = rand()%vv.getSize();
                        vv.remove(id);
                    }
                }
            }
        };

        struct TestMask {
            static void f(void*, Config::ConfigSectionMap&) {
                constexpr u32 set_n = n()/10;
                Mask<n()> mask;

                for (u32 i=0; i<set_n; ++i) {
                    mask.set(rand()%n());
                }

                volatile u32 set_cnt = 0;
                {
                    MEASURE_BLOCK("mask loop");
                    for (u32 pos=0; pos<n(); ++pos) {
                        if (mask.test(pos))
                            ++set_cnt;
                        pos++;
                    };
                }
                {
                    MEASURE_BLOCK("mask loop (bitscan)");
                    LOOP_SET_BITS(mask, it) {
                        ++set_cnt;
                    }
                }
            }
        };
}}


#endif //UNSORTED_VECTOR_TEST_H
