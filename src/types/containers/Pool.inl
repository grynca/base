#include "Pool.h"

namespace grynca {

    inline Pool::Pool(uint32_t item_size)
     : item_size_(item_size)
    {}

    inline Index Pool::add(uint8_t*& new_item_out) {
        uint32_t slot_id;
        if (!free_slots_.empty()) {
            slot_id = free_slots_.back();
            free_slots_.pop_back();
        }
        else {
            slot_id = size();
            data_.resize((slot_id+1)*item_size_);
            versions_.resize(slot_id+1, (1<<15));
        }

        new_item_out = &data_[slot_id*item_size_];
        unsetFree_(slot_id);
        return Index(slot_id, versions_[slot_id]);
    }

    inline void Pool::removeAtPos(uint32_t pos) {
        ASSERT(!isFree_(pos));
        free_slots_.push_back(pos);
        ++versions_[pos];
        setFree_(pos);
    }

    inline void Pool::removeAtPos(uint32_t pos, DestroyFunc destructor) {
        ASSERT(!isFree_(pos));
        destructor(getAtPos(pos));
        free_slots_.push_back(pos);
        ++versions_[pos];
        setFree_(pos);
    }

    inline void Pool::remove(Index index) {
        ASSERT(isValidIndex(index));
        free_slots_.push_back(index.getIndex());
        ++versions_[index.getIndex()];
        setFree_(index.getIndex());
    }

    inline void Pool::remove(Index index, DestroyFunc destructor) {
        // call destructor on item data
        destructor(get(index));
        free_slots_.push_back(index.getIndex());
        ++versions_[index.getIndex()];
        setFree_(index.getIndex());
    }

    inline void Pool::reserve(size_t count) {
        data_.reserve(item_size_*count);
        versions_.reserve(count);
    }

    inline uint8_t* Pool::get(Index index) {
        ASSERT(isValidIndex(index));
        return &data_[index.getIndex()*item_size_];
    }

    inline const uint8_t* Pool::get(Index index)const {
        ASSERT(isValidIndex(index));
        return &data_[index.getIndex()*item_size_];
    }

    inline uint8_t* Pool::getAtPos(uint32_t pos) {
        if (isFree_(pos))
            return NULL;
        return &data_[pos*item_size_];
    }

    inline const uint8_t* Pool::getAtPos(uint32_t pos)const {
        if (isFree_(pos))
            return NULL;
        return &data_[pos*item_size_];
    }

    inline Index Pool::getIndexForPos(uint32_t pos) {
        return Index(pos, versions_[pos]);
    }

    inline void Pool::getIndexForPos2(uint32_t pos, Index& index_out) {
        index_out.setIndex(pos);
        index_out.setVersion(versions_[pos]);
    }

    inline bool Pool::isValidIndex(Index index)const {
        if (index.getIndex() >= size())
            return false;
        return versions_[index.getIndex()] == index.getVersion();
    }


    inline uint8_t* Pool::getData() {
        return &data_[0];
    }

    inline uint32_t Pool::size()const {
        return versions_.size();
    }

    inline uint32_t Pool::occupiedSize()const {
        return size()-free_slots_.size();
    }

    inline bool Pool::empty()const {
        return data_.empty();
    }

    inline void Pool::clear() {
        data_.clear();
        versions_.clear();
    }

    inline void Pool::clear(DestroyFunc destructor) {
        for (uint32_t i=0; i<size(); ++i) {
            uint8_t* ptr = getAtPos(i);
            if (ptr) {
                destructor(ptr);
            }
        }
        clear();
    }

    inline float Pool::getMemoryWaste() {
        return ((float)free_slots_.size())/size();
    }

    inline bool Pool::isFree_(uint32_t pos)const {
        ASSERT(pos < versions_.size());
        return (versions_[pos] & uint16_t(1<<15)) != 0;
    }

    inline void Pool::setFree_(uint32_t pos) {
        ASSERT(pos < versions_.size());
        versions_[pos] |= 1<<15;
    }

    inline void Pool::unsetFree_(uint32_t pos) {
        ASSERT(pos < versions_.size());
        versions_[pos] &= ~(1<<15);
    }


}