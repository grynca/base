#ifndef UNSORTED_VECTOR_TEST_H
#define UNSORTED_VECTOR_TEST_H

//#include <unordered_map>

namespace grynca { namespace test_containers {
        inline constexpr u32 n() {
            return 1e5;
        }

        inline fast_vector<u32> randomDestructionOrder() {
            fast_vector<u32> destruction_order;
            destruction_order.reserve(n());
            randomPickN(destruction_order, n(), n());
            return destruction_order;
        }

        template <typename ContainerType, typename AddItemFunc>
        inline void testPtrs(ContainerType& cont, const AddItemFunc& aif) {
            typename ContainerType::ItemIndexType new_item_id = aif(cont);
            typename ContainerType::IPtr ptr(cont, new_item_id);
            ptr.acc().dostuff();
            ptr.accPtr()->dostuff();

            {
                typename ContainerType::IRefPtr ref(ptr);
                ref.acc().dostuff();
                ref.accPtr()->dostuff();
                // here ref should unref and remove item
            }

            ASSERT(!ptr.isValid());
        }

        struct Hasher {
            size_t operator()(const u32& key)const {
                return calcHash32(key);
            }
        };

        struct TestVector {
            static void f(void*, Config::ConfigSectionMap&) {
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

        struct TestChunkedBuffer {
            static void f(void*, Config::ConfigSectionMap&) {
                //std::vector<MyStuff> vec;
                ChunkedBuffer chb;
                chb.init(sizeof(MyStuff));
                chb.reserve(n());

                {
                    MEASURE_BLOCK("creation");
                    for (size_t i=0; i<n(); ++i) {
                        chb.resize(i+1);
                        new (chb.accItem(i)) MyStuff();
                    }
                }

                {
                    MEASURE_BLOCK("loop(indices)");
                    for (size_t i=0; i<n(); ++i) {
                        ((MyStuff*)chb.accItem(i))->dostuff();
                    }
                }

                {
                    MEASURE_BLOCK("loop(foreach)");
                    chb.forEach([](u8* item) {
                        ((MyStuff*)item)->dostuff();
                    });
                }

//                {
//                    BlockMeasure m(" destruction");
//                    for (size_t i=0; i<n(); ++i) {
//                        stuff_fv.erase(stuff_fv.begin());
//                    }
//                }
            }
        };

        struct TestItems {
            static void f(void*, Config::ConfigSectionMap&) {
                Items<MyStuff> items;
                items.reserve(n());

                {
                    MEASURE_BLOCK("creation");
                    for (size_t i=0; i<n(); ++i) {
                        items.emplace_back();
                    }
                }

                {
                    MEASURE_BLOCK("loop(index)");
                    for (size_t i=0; i<n(); ++i) {
                        items[i].dostuff();
                    }
                }

                {
                    MEASURE_BLOCK("destruction");
                    for (size_t i=0; i<n(); ++i) {
                       u32 id = rand()%items.size();
                       items.replaceWithLast(id);
                    }
                }
            }
        };

        struct TestItems2 {
            static void f(void*, Config::ConfigSectionMap&) {
                Items2<> items;
                items.init<MyStuff>();
                items.reserve(n());

                {
                    MEASURE_BLOCK("creation");
                    for (size_t i=0; i<n(); ++i) {
                        items.emplace_back();
                    }
                }

                {
                    MEASURE_BLOCK("loop(index)");
                    for (size_t i=0; i<n(); ++i) {
                        items.accItem<MyStuff>(i).dostuff();
                    }
                }

                {
                    MEASURE_BLOCK("destruction");
                    for (size_t i=0; i<n(); ++i) {
                        u32 id = rand()%items.size();
                        items.replaceWithLast(id);
                    }
                }
            }
        };

        struct TestTightManager {
            static void f(void*, Config::ConfigSectionMap&) {
                MyStuffManager mgr;
                fast_vector<Index> indices;
                mgr.reserveSpaceForItems(n());
                indices.reserve(n());
                fast_vector<u32> destruction_order = randomDestructionOrder();

                testPtrs(mgr, [](auto& ct) {
                    return ct.addItem().getId();
                });


                {
                    MEASURE_BLOCK("creation");
                    for (size_t i=0; i<n(); ++i) {
                        indices.push_back(mgr.addItem().getId());
                    }
                }

                {
                    MEASURE_BLOCK("loop(index)");
                    for (size_t i=0; i<n(); ++i) {
                        mgr.accItem(indices[i]).dostuff();
                    }
                }

                {
                    MEASURE_BLOCK("loop (positions)");
                    for (size_t i=0; i<n(); ++i) {
                        mgr.accItemAtPos(i)->dostuff();
                    }
                }

                {
                    MEASURE_BLOCK("destruction");
                    for (size_t i=0; i<n(); ++i) {
                        mgr.removeItem(indices[destruction_order[i]]);
                    }
                }
            }
        };


        struct TestHashMap {
            static void f(void*, Config::ConfigSectionMap&) {
                HashMap<MyStuff, u32, Hasher> hm;
                fast_vector<u32> indices;
                hm.reserve(n());
                indices.reserve(n());
                fast_vector<u32> destruction_order = randomDestructionOrder();

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
                        MyStuff& stuff = hm.accItem(i);
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
                Array<MyStuff> stuff_arr;
                fast_vector<Index> indices;
                stuff_arr.reserve(n());
                indices.reserve(n());
                fast_vector<u32> destruction_order = randomDestructionOrder();

                testPtrs(stuff_arr, [](auto& ct) {
                    return ct.add();
                });

                {
                    MEASURE_BLOCK("creation");
                    for (size_t i=0; i<n(); ++i) {
                        indices.push_back(stuff_arr.add());
                    }
                }

                {
                    MEASURE_BLOCK("loop");
                    for (size_t i=0; i<n(); ++i) {
                        MyStuff* stuff = stuff_arr.accItemAtPos(i);
                        // must check if not NULL (can contain holes)
                        if (stuff)
                            stuff->dostuff();
                    }
                }

                {
                    MEASURE_BLOCK("destruction");
                    for (size_t i=0; i<n(); ++i) {
                        stuff_arr.removeItem(indices[destruction_order[i]]);
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
                fast_vector<u32> destruction_order = randomDestructionOrder();

                testPtrs(stuff_vec, [](auto& ct) {
                    return ct.add();
                });

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
                        stuff_vec.accItemAtPos(i)->dostuff();
                    }
                }

                {
                    MEASURE_BLOCK("destruction");
                    for (size_t i=0; i<n(); ++i) {
                        stuff_vec.removeItem(indices[destruction_order[i]]);
                    }
                }
            }
        };

        struct TestSortedArray {
            static void f(void*, Config::ConfigSectionMap&) {
                SortedArray<MyStuffSorted> stuff_arr;
                stuff_arr.reserve(n());
                fast_vector<u32> destruction_order = randomDestructionOrder();

                {
                    MEASURE_BLOCK("creation");
                    for (size_t i=0; i<n(); ++i) {
                        u32 priority = u32(rand()%10);
                        stuff_arr.add(MyStuffSorted(priority));
                    }
                }

                {
                    MEASURE_BLOCK("loop");
//                    stuff_arr.loopItems([](MyStuffSorted& stuff) {
//                        stuff.dostuff();
//                    });
                }

                {
                    MEASURE_BLOCK("destruction");
                    for (size_t i=0; i<n(); ++i) {
                        stuff_arr.removeItem(destruction_order[i]);
                    }
                }
            }
        };

        struct TestCompsPool {
            static void f(void*, Config::ConfigSectionMap&) {
                CompsPool<3, MyDomain> mp;
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
                    MEASURE_BLOCK("loop (indices)");
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
                        mp.removeItem(indices[destruction_order[i]]);
                    }
                }
            }
        };

        struct TestPolyPool {
            static void f(void*, Config::ConfigSectionMap&) {
                PolyPool<3, MyStuff> pp;
                fast_vector<Index> indices;
                pp.initTypes<StuffTypes>();
                for (u32 i=0; i<pp.getTypesCount(); ++i) {
                    pp.reserve(i, n());
                }
                indices.reserve(n());
                fast_vector<u32> destruction_order = randomDestructionOrder();


                testPtrs(pp, [] (auto& ct) {
                    Index new_item_id;
                    ct.add(0, new_item_id);
                    return new_item_id;
                });

                {
                    MEASURE_BLOCK("creation");
                    for (size_t i=0; i<n(); ++i) {
                        u16 tid = 0;
                        switch (rand()%3) {
                            case 0:
                                tid = pp.getTypeIdOf<MyStuff>();
                                break;
                            case 1:
                                tid = pp.getTypeIdOf<MyStuffA>();
                                break;
                            case 2:
                                tid = pp.getTypeIdOf<MyStuffB>();
                                break;
                        }
                        Index id;
                        pp.add(tid, id);
                        indices.push_back(id);
                    }
                }

                {
                    MEASURE_BLOCK("loop (foreach)");
                    for (u16 tid=0; tid<pp.getTypesCount(); ++tid) {
                        pp.forEachWithId(tid, [](MyStuff& ms, const Index&) {
                            ms.dostuff();
                        });
                    }
                }

                {
                    MEASURE_BLOCK("loop (positions)");
                    for (u16 tid=0; tid<pp.getTypesCount(); ++tid) {
                        for (u32 i=0; i<pp.getSize(tid); ++i) {
                            if (!pp.isFree(tid, i)) {
                                pp.accItemAtPos(tid, i).dostuff();
                            }
                        }
                    }
                }

                {
                    MEASURE_BLOCK("loop (indices)");
                    for (size_t i=0; i<indices.size(); ++i) {
                        pp.accItem(indices[i]).dostuff();
                    }
                }

                {
                    MEASURE_BLOCK("destruction");
                    for (size_t i=0; i<n(); ++i) {
                        pp.removeItem(indices[destruction_order[i]]);
                    }
                }
            }
        };

        struct TestVArray {
            static void f(void*, Config::ConfigSectionMap&) {
                VArray<MyStuff> vv;
                fast_vector<Index> indices;
                vv.initTypes<StuffTypes>();
                vv.reserve(n());
                indices.reserve(n());
                fast_vector<u32> destruction_order = randomDestructionOrder();

                testPtrs(vv, [] (auto& ct) {
                    return ct.add(0);
                });

                {
                    MEASURE_BLOCK("creation");
                    for (size_t i=0; i<n(); ++i) {
                        u16 tid = 0;
                        switch (rand()%3) {
                            case 0:
                                tid = vv.getTypeIdOf<MyStuff>();
                                break;
                            case 1:
                                tid = vv.getTypeIdOf<MyStuffA>();
                                break;
                            case 2:
                                tid = vv.getTypeIdOf<MyStuffB>();
                                break;
                        }
                        indices.push_back(vv.add(tid));
                    }
                }

                {
                    MEASURE_BLOCK("loop (indices)");
                    for (size_t i=0; i<indices.size(); ++i) {
                        vv.accItem(indices[i]).dostuff();
                    }
                }

                {
                    MEASURE_BLOCK("destruction");
                    for (size_t i=0; i<n(); ++i) {
                        vv.removeItem(indices[destruction_order[i]]);
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
