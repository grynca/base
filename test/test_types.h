#ifndef TEST_TYPES_H
#define TEST_TYPES_H

namespace grynca { namespace test_types {

        inline void test() {

            std::cout << "Loop over MyStuff TypePack types info: " << std::endl;
            for (uint32_t i=0; i<StuffTypes::getTypesCount(); ++i) {
                const TypeInfo& ti = StuffTypes::getTypeInfo(i);
                std::cout << " Pos " << i << ":" << " TID= " << ti.getId() << ", size= " << ti.getSize() << ", name= " << ti.getTypename() << std::endl;
            }


            std::cout << "Loop over MyDomain types: " << std::endl;
            for (uint32_t i=0; i<TypeInfoManager<MyDomain>::getTypesCount(); ++i) {
                if (!TypeInfoManager<MyDomain>::isTypeIdSet(i)) {
                    std::cout << " Pos " << i << ":" << " NULL " << std::endl;
                }
                else {
                    const TypeInfo& ti = TypeInfoManager<MyDomain>::get(i);
                    std::cout << " Pos " << i << ":" << " TID= " << ti.getId() << ", size= " << ti.getSize() << ", name= " << ti.getTypename() << std::endl;
                }
            }
        }

    }
}

#endif //TEST_TYPES_H
