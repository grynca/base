#include "Type.h"

/// Inline implementation

namespace grynca {

    template<typename Domain>
    inline uint32_t TypeIdGiver<Domain>::getNewId() {
        // static
        static uint32_t id_pool = 0;
        return id_pool++;
    }

    template <typename T, typename Domain>
    inline void Type<T, Domain>::destroy(void* place) {
        //static
        (void)place; // so compiler does not complain
        ((T*)place)->~T();
    }

    template <typename T, typename Domain>
    inline uint32_t Type<T, Domain>::getInternalTypeId() {
        //static
        static uint32_t id = TypeIdGiver<Domain>::getNewId();
        return id;
    }

    template <typename T, typename Domain>
    inline void Type<T, Domain>::copy(void* to, const void *from) {
        //static
        std::copy((T*)from, ((T*)from)+1, (T*)to);
    }

    template <typename T, typename Domain>
    inline void Type<T, Domain>::move(void *to, void *from)  {
        std::move((T*)from, ((T*)from)+1, (T*)to);
    }

    template <typename T>
    inline uint32_t CustomTypeId<T>::getTypeId() {
        // static
        assert(isTypeIdSet() && "Type id not set.");
        return tid_();
    }

    template <typename T>
    inline void CustomTypeId<T>::setTypeId(uint32_t tid) {
        // static
        assert(!isTypeIdSet() && "Type id already set.");
        tid_() = tid;
    }

    template <typename T>
    inline bool CustomTypeId<T>::isTypeIdSet() {
        // static
        return tid_() >= 0;
    }

    template <typename T>
    inline int32_t& CustomTypeId<T>::tid_() {
        // static
        static int32_t tid = -1;
        return tid;
    }

}
