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
        clear();
    }

    ATPL
    template <typename ...ConstructionArgs>
    inline Index ATYPE::add(ConstructionArgs&&... args) {
        u8* new_item_out;
        Index id = pool_.add(new_item_out);
        new(new_item_out) T(std::forward<ConstructionArgs>(args)...);
        return id;
    }

    ATPL
    template <typename ...ConstructionArgs>
    inline T& ATYPE::add2(Index& index_out, ConstructionArgs&&... args) {
        u8* new_item_out;
        index_out = pool_.add(new_item_out);
        return *(new(new_item_out) T(std::forward<ConstructionArgs>(args)...));
    }

    ATPL
    template <typename ...ConstructionArgs>
    inline typename ATYPE::IRefPtr ATYPE::addRC(ConstructionArgs&&... args) {
        u8* new_item_out;
        Index index = pool_.add(new_item_out);
        new(new_item_out) T(std::forward<ConstructionArgs>(args)...);
        return IRefPtr(*this, index);
    }

    ATPL
    inline void ATYPE::removeItem(Index index) {
        ASSERT_M(isValidIndex(index), "Invalid index.");
        pool_.removeItem(index, Type<T>::destroy);
    }

    ATPL
    inline void ATYPE::removeItemAtPos(u32 pos) {
        pool_.removeItemAtPos(pos, Type<T>::destroy);
    }

    ATPL
    inline void ATYPE::reserve(size_t count) {
        pool_.reserve(count);
    }

    ATPL
    inline u32 ATYPE::getItemPos(const T* item)const {
        return pool_.getItemPos((const u8*)item);
    }

    ATPL
    inline u32 ATYPE::getItemPos(Index index)const {
        return pool_.getItemPos(index);
    }

    ATPL
    inline T& ATYPE::accItem(Index index) {
        return *((T*)pool_.accItem(index));
    }

    ATPL
    inline T* ATYPE::accItemAtPos(u32 pos) {
        return (T*)pool_.accItemAtPos(pos);
    }

    ATPL
    inline T& ATYPE::accItemAtPos2(u32 pos) {
        return *(T*)pool_.accItemAtPos2(pos);
    }

    ATPL
    inline Index ATYPE::getIndexForPos(u32 pos)const {
        return pool_.getIndexForPos(pos);
    }

    ATPL
    inline Index ATYPE::getFullIndex(u32 index_id)const {
        return pool_.getFullIndex(index_id);
    }

    ATPL
    inline T& ATYPE::accItemWithInnerIndex(u32 inner_id) {
        return *(T*)pool_.accItemWithInnerIndex(inner_id);
    }

    ATPL
    inline const T& ATYPE::getItemWithInnerIndex(u32 inner_id)const {
        return *(T*)pool_.getItemWithInnerIndex(inner_id);
    }

    ATPL
    inline const T& ATYPE::getItem(Index index)const {
        return *((T*)pool_.getItem(index));
    }

    ATPL
    inline const T* ATYPE::getItemAtPos(u32 pos)const {
        return (T*)pool_.getItemAtPos(pos);
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
    inline void ATYPE::clear() {
        pool_.clear(Type<T>::destroy);
    }

    ATPL
    inline u32 ATYPE::size()const {
        return pool_.size();
    }

    ATPL
    inline bool ATYPE::empty()const {
        return pool_.empty();
    }
}

#undef ATPL
#undef ATYPE