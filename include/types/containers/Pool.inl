#include "Pool.h"

#define GROWING_FACTOR 1.5f
#define FREE_BITMASK (1<<15)

namespace grynca {

    inline Pool::Pool()
     : item_size_(InvalidId()), first_free_pos_(InvalidId())
    {
    }

    inline Pool::Pool(u32 item_size)
     : item_size_(item_size), first_free_pos_(InvalidId())
    {
        // when item is not used its data_ is used for interwined doubly linked free list
        ASSERT(item_size > sizeof(u32));
    }

    inline void Pool::init(u32 item_size) {
        item_size_ = item_size;
        ASSERT(item_size > sizeof(u32));
    }

    inline Index Pool::add(u8*& new_item_out) {
        ASSERT_M(item_size_ != InvalidId(), "Not initialized!");

        u32 slot_id;
        if (first_free_pos_ != InvalidId()) {
            slot_id = first_free_pos_;
            first_free_pos_ = getNextFree_(first_free_pos_);
        }
        else {
            // resize
            slot_id = size();

            u32 new_items_cnt = u32(ceilf(GROWING_FACTOR*(slot_id+1)));

            data_.resize(new_items_cnt*item_size_);
            versions_.resize(new_items_cnt, FREE_BITMASK);

            // interwine free ids list to new slots
            for (u32 i=slot_id+1; i<(new_items_cnt-1); ++i) {
                setNextFree_(i, i+1);
            }
            setNextFree_(new_items_cnt-1, first_free_pos_);
            first_free_pos_ = slot_id+1;
        }

        new_item_out = &data_[slot_id*item_size_];
        unsetFree_(slot_id);
        return Index(slot_id, versions_[slot_id]);
    }

    inline void Pool::removeAtPos(u32 pos) {
        ASSERT(!isFree_(pos));
        setNextFree_(pos, first_free_pos_);
        first_free_pos_ = pos;
        ++versions_[pos];
        setFree_(pos);
    }

    inline void Pool::removeAtPos(u32 pos, DestroyFunc destructor) {
        ASSERT(!isFree_(pos));
        destructor(getAtPos(pos));
        setNextFree_(pos, first_free_pos_);
        first_free_pos_ = pos;
        ++versions_[pos];
        setFree_(pos);
    }

    inline void Pool::remove(Index index) {
        ASSERT(isValidIndex(index));
        removeAtPos(index.getIndex());
    }

    inline void Pool::remove(Index index, DestroyFunc destructor) {
        ASSERT(isValidIndex(index));
        removeAtPos(index.getIndex(), destructor);
    }

    inline void Pool::reserve(size_t count) {
        data_.reserve(item_size_*count);
        versions_.reserve(count);
    }

    inline u32 Pool::getItemPos(const u8* item)const {
        u32 ptr_dist = item-&data_[0];
        ASSERT(ptr_dist%item_size_ == 0);
        u32 pos = ptr_dist/item_size_;
        ASSERT(i32(pos)>=0 && pos < size());
        return pos;
    }

    inline u32 Pool::getItemPos(Index index)const {
        ASSERT(isValidIndex(index));
        return index.getIndex();
    }

    inline u8* Pool::get(Index index) {
        ASSERT(isValidIndex(index));
        return &data_[index.getIndex()*item_size_];
    }

    inline const u8* Pool::get(Index index)const {
        ASSERT(isValidIndex(index));
        return &data_[index.getIndex()*item_size_];
    }

    inline u8* Pool::getAtPos(u32 pos) {
        if (isFree_(pos))
            return NULL;
        return &data_[pos*item_size_];
    }

    inline const u8* Pool::getAtPos(u32 pos)const {
        if (isFree_(pos))
            return NULL;
        return &data_[pos*item_size_];
    }

    inline u8* Pool::getAtPos2(u32 pos) {
        ASSERT(!isFree_(pos));
        return &data_[pos*item_size_];
    }

    inline const u8* Pool::getAtPos2(u32 pos)const {
        ASSERT(!isFree_(pos));
        return &data_[pos*item_size_];
    }

    inline Index Pool::getIndexForPos(u32 pos) {
        return Index(pos, versions_[pos]);
    }

    inline void Pool::getIndexForPos2(u32 pos, Index& index_out) {
        index_out.setIndex(pos);
        index_out.setVersion(versions_[pos]);
    }

    inline bool Pool::isValidIndex(Index index)const {
        if (index.getIndex() >= size())
            return false;
        return versions_[index.getIndex()] == index.getVersion();
    }


    inline u8* Pool::getData() {
        return &data_[0];
    }

    inline u32 Pool::size()const {
        return versions_.size();
    }

    inline u32 Pool::calcFreeSlotsCount()const {
        u32 next_free = first_free_pos_;
        u32 free_cnt = 0;
        while (next_free != InvalidId()) {
            free_cnt++;
            next_free = getNextFree_(next_free);
        }
        return free_cnt;
    }

    inline u32 Pool::calcOccupiedSize()const {
        return size()-calcFreeSlotsCount();
    }

    inline bool Pool::empty()const {
        return data_.empty();
    }

    inline void Pool::clear() {
        data_.clear();
        versions_.clear();
    }

    inline void Pool::clear(DestroyFunc destructor) {
        for (u32 i=0; i<size(); ++i) {
            u8* ptr = getAtPos(i);
            if (ptr) {
                destructor(ptr);
            }
        }
        clear();
    }

    inline f32 Pool::calcMemoryWaste() {
        return f32(calcFreeSlotsCount())/size();
    }

    inline bool Pool::isFree_(u32 pos)const {
        ASSERT(pos < versions_.size());
        return (versions_[pos] & u16(FREE_BITMASK)) != 0;
    }

    inline void Pool::setFree_(u32 pos) {
        ASSERT(pos < versions_.size());
        versions_[pos] |= FREE_BITMASK;
    }

    inline void Pool::unsetFree_(u32 pos) {
        ASSERT(pos < versions_.size());
        versions_[pos] &= ~FREE_BITMASK;
    }

    inline u32 Pool::getNextFree_(u32 pos)const {
        return *(u32*)&data_[pos*item_size_];
    }

    inline void Pool::setNextFree_(u32 pos, u32 next_free_pos) {
        *(u32*)&data_[pos*item_size_] = next_free_pos;
    }
}

#undef FREE_BITMASK
#undef GROWING_FACTOR