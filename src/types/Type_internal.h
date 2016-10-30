#ifndef TYPE_INTERNAL_H_H
#define TYPE_INTERNAL_H_H

#include "Type.h"

namespace grynca {

    namespace internal {

        // sets type ids for TypePack
        template <typename Types>
        struct TypeIdTypesPackSetter {
            TypeIdTypesPackSetter() {
                setTypeId<Types>();
            }

            template <typename TP>
            IF_EMPTY(TP) setTypeId() { }

            template <typename TP>
            IF_NOT_EMPTY(TP) setTypeId() {
                uint32_t type_pos = uint32_t(Types::template pos<HEAD(TP)>());
                TypeInfoManager<Types>::template setTypeId<HEAD(TP)>(type_pos);
                setTypeId<TAIL(TP)>();
            }
        };

        // sets type id for class
        template <typename T, unsigned int ID, typename Domain = void>
        struct TypeIdClassSetter {
            TypeIdClassSetter() {
                TypeInfoManager<Domain>::template setTypeId<T>(ID);
            }
        };

        struct DefConstruct {
            template <typename T>
            static void f(void* ptr) {
                new (ptr) T();
            }
        };

        struct NoDefConstruct {
            template <typename T>
            static void f() {
                NEVER_GET_HERE("Class does not have default constructor.");
            }
        };
    }

}

#endif //TYPE_INTERNAL_H_H
