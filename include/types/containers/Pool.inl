#include "Pool.h"

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
            unsetFree_(slot_id);
        }
        else {
            // resize
            slot_id = size();
            data_.enlarge((slot_id+1)*item_size_);
            item_ctxs_.emplace_back(0, 0);
        }

        new_item_out = &data_[slot_id*item_size_];
        return Index(slot_id, item_ctxs_[slot_id].version);
    }

    inline Index Pool::add(u8*& new_item_out, u16 aux_id) {
        Index id = add(new_item_out);
        item_ctxs_[id.getIndex()].aux_id = aux_id;
        id.setAuxIndex(aux_id);
        return id;
    }

    inline void Pool::removeItemAtPos(u32 pos) {
        ASSERT(!isFree_(pos));
        setNextFree_(pos, first_free_pos_);
        first_free_pos_ = pos;
        ++item_ctxs_[pos].version;
        setFree_(pos);
    }

    inline void Pool::removeItemAtPos(u32 pos, DestroyFunc destructor) {
        ASSERT(!isFree_(pos));
        destructor(accItemAtPos(pos));
        setNextFree_(pos, first_free_pos_);
        first_free_pos_ = pos;
        ++item_ctxs_[pos].version;
        setFree_(pos);
    }

    inline void Pool::removeItem(Index index) {
        ASSERT(isValidIndex(index));
        removeItemAtPos(index.getIndex());
    }

    inline void Pool::removeItem(Index index, DestroyFunc destructor) {
        ASSERT(isValidIndex(index));
        removeItemAtPos(index.getIndex(), destructor);
    }

    inline void Pool::reserve(size_t count) {
        data_.reserve(item_size_*count);
        item_ctxs_.reserve(count);
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

    inline u8* Pool::accItem(Index index) {
        ASSERT(isValidIndex(index));
        return &data_[index.getIndex()*item_size_];
    }

    inline const u8* Pool::getItem(Index index)const {
        ASSERT(isValidIndex(index));
        return &data_[index.getIndex()*item_size_];
    }

    inline u8* Pool::accItemWithInnerIndex(u32 inner_id) {
        return accItemAtPos(inner_id);
    }

    inline const u8* Pool::getItemWithInnerIndex(u32 inner_id)const {
        return getItemAtPos(inner_id);
    }

    inline u8* Pool::accItemAtPos(u32 pos) {
        if (isFree_(pos))
            return NULL;
        return &data_[pos*item_size_];
    }

    inline const u8* Pool::getItemAtPos(u32 pos)const {
        if (isFree_(pos))
            return NULL;
        return &data_[pos*item_size_];
    }

    inline u8* Pool::accItemAtPos2(u32 pos) {
        ASSERT(!isFree_(pos));
        return &data_[pos*item_size_];
    }

    inline const u8* Pool::getItemAtPos2(u32 pos)const {
        ASSERT(!isFree_(pos));
        return &data_[pos*item_size_];
    }

    inline Index Pool::getIndexForPos(u32 pos)const {
        // for Pool with holes pos == Index.index
        return getFullIndex(pos);
    }

    inline void Pool::getIndexForPos2(u32 pos, Index& index_out)const {
        index_out.setIndex(pos);
        index_out.setVerAux(item_ctxs_[pos].ver_aux);
    }

    inline Index Pool::getFullIndex(u32 index_id)const {
        return Index(index_id, item_ctxs_[index_id].version, item_ctxs_[index_id].aux_id);
    }

    inline bool Pool::isValidIndex(Index index)const {
        if (index.getIndex() >= size())
            return false;
        return item_ctxs_[index.getIndex()].version == index.getVersion();
    }


    inline u8* Pool::getData() {
        return &data_[0];
    }

    inline u32 Pool::size()const {
        return u32(item_ctxs_.size());
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
        item_ctxs_.clear();
    }

    inline void Pool::clear(DestroyFunc destructor) {
        for (u32 i=0; i<size(); ++i) {
            u8* ptr = accItemAtPos(i);
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
        ASSERT(pos < item_ctxs_.size());
        return (item_ctxs_[pos].version & u16(FREE_BITMASK)) != 0;
    }

    inline void Pool::setFree_(u32 pos) {
        ASSERT(pos < item_ctxs_.size());
        item_ctxs_[pos].version |= FREE_BITMASK;
    }

    inline void Pool::unsetFree_(u32 pos) {
        ASSERT(pos < item_ctxs_.size());
        item_ctxs_[pos].version &= ~FREE_BITMASK;
    }

    inline u32 Pool::getNextFree_(u32 pos)const {
        return *(u32*)&data_[pos*item_size_];
    }

    inline void Pool::setNextFree_(u32 pos, u32 next_free_pos) {
        *(u32*)&data_[pos*item_size_] = next_free_pos;
    }
}

#undef FREE_BITMASK