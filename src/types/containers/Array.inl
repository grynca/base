#include "Array.h"

#define ATPL template <typename T, typename PT>
#define ATYPE Array<T, PT>

namespace grynca {

    ATPL
    inline ATYPE::Array()
     : pool_(sizeof(T))
    {}

    ATPL
    inline ATYPE::~Array() {
        pool_.clear(Type<T>::destroy);
    }

    ATPL
    template <typename ...ConstructionArgs>
    inline Index ATYPE::add(ConstructionArgs&&... args) {
        uint8_t* new_item_out;
        Index id = pool_.add(new_item_out);
        new(new_item_out) T(std::forward<ConstructionArgs>(args)...);
        return id;
    }

    ATPL
    inline void ATYPE::remove(Index index) {
        ASSERT_M(isValidIndex(index), "Invalid index.");
        pool_.remove(index, Type<T>::destroy);
    }

    ATPL
    inline void ATYPE::removeAtPos(uint32_t pos) {
        pool_.removeAtPos(pos, Type<T>::destroy);
    }

    ATPL
    inline void ATYPE::reserve(size_t count) {
        pool_.reserve(count);
    }

    ATPL
    inline T& ATYPE::get(Index index) {
        ASSERT_M(isValidIndex(index), "Invalid index.");

        return *((T*)pool_.get(index));
    }

    ATPL
    inline T* ATYPE::getAtPos(uint32_t pos) {
        return (T*)pool_.getAtPos(pos);
    }

    ATPL
    inline Index ATYPE::getIndexForPos(uint32_t pos) {
        return pool_.getIndexForPos(pos);
    }

    ATPL
    inline const T& ATYPE::get(Index index)const {
        return *((T*)pool_.get(index));
    }

    ATPL
    inline const T* ATYPE::getAtPos(uint32_t pos)const {
        return (T*)pool_.getAtPos(pos);
    }

    ATPL
    inline bool ATYPE::isValidIndex(Index index)const {
        return pool_.isValidIndex(index);
    }

    ATPL
    inline T* ATYPE::getData() {
        return (T*)pool_.getData();
    }

    ATPL
    inline uint32_t ATYPE::size()const {
        return pool_.size();
    }

    ATPL
    inline bool ATYPE::empty()const {
        return pool_.empty();
    }
}

#undef ATPL
#undef ATYPE