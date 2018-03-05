#ifndef TYPE_INTERNAL_H_H
#define TYPE_INTERNAL_H_H

#define WITHOUT_IMPL
#include "TypesPack.h"
#undef WITHOUT_IMPL
#include "Index.h"

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
                u32 type_pos = u32(Types::template pos<HEAD(TP)>());
                TypeInfoManager<Types>::template setTypeId<HEAD(TP)>(type_pos);
                setTypeId<TAIL(TP)>();
            }
        };

        template <typename Types, typename Domain>
        struct TypeIdTypesPackMapper {

            static void map(fast_vector<u32>& ids_mapper_out) {
                mapInner_<Types>(ids_mapper_out);
            }

        private:
            template <typename TP>
            static IF_EMPTY(TP) mapInner_(fast_vector<u32>& ids_mapper_out) {}

            template <typename TP>
            static IF_NOT_EMPTY(TP) mapInner_(fast_vector<u32>& ids_mapper_out) {
                u32 int_tid = Type<HEAD(TP), Domain>::getInternalTypeId();
                u32 pack_tid = u32(Types::template pos<HEAD(TP)>());
                if (ids_mapper_out.size() >= int_tid) {
                    ids_mapper_out.resize(int_tid+1, InvalidId());
                }
                ids_mapper_out[int_tid] = pack_tid;
                mapInner_<TAIL(TP)>(ids_mapper_out);
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

        struct CopyConstruct {
            template <typename T>
            static void f(void* obj, const void* src) {
                new (obj) T(*(const T*)src);
            }
        };

        struct NoCopyConstruct {
            template <typename T>
            static void f() {
                NEVER_GET_HERE("Class does not have copy constructor.");
            }
        };
    }

}

#endif //TYPE_INTERNAL_H_H
