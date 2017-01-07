#include "TightPool.h"

namespace grynca {

    inline TightPool::TightPool(u32 item_size)
     : item_size_(item_size), first_free_pos_(InvalidId())
    {}

    inline Index TightPool::add(u8*& new_item_out) {
        u32 item_pos = (u32)size();
        size_t old_size_bytes = item_pos*item_size_;
        data_.resize(old_size_bytes + item_size_);
        new_item_out = &data_[old_size_bytes];

        u32 redirect_id;
        u16 redirect_version;
        if (first_free_pos_ == InvalidId()) {
            redirect_id = (u32)redirects_.size();
            redirect_version = 0;
            redirects_.emplace_back(item_pos, redirect_version);
        }
        else {
            redirect_id = first_free_pos_;
            first_free_pos_ = getNextFree_(first_free_pos_);
            redirect_version = redirects_[redirect_id].getVersion();
            redirects_[redirect_id].setIndex(item_pos);
        }
        redirect_ids_.push_back(redirect_id);
        return Index(redirect_id, redirect_version);
    }

    inline void TightPool::remove(Index index) {
        ASSERT_M(isValidIndex(index), "Invalid index.");

        u32 deleted_item_pos = getItemPos(index);

        u8* deleted = &data_[deleted_item_pos*item_size_];
        u32 last_item_pos = size()-1;
        u8* last = &data_[last_item_pos*item_size_];

        // move last item to deleted place
        memcpy(deleted, last, item_size_);
        // update redirect for last(moved) item
        u32 redirect_id = redirect_ids_[last_item_pos];
        redirects_[redirect_id].setIndex(deleted_item_pos);
        redirect_ids_[deleted_item_pos] = redirect_id;

        data_.resize(last_item_pos*item_size_);
        redirect_ids_.resize(last_item_pos);

        redirects_[index.getIndex()].setIndex(u32(-1));
        // increment redirect version of removed item
        redirects_[index.getIndex()].setVersion(u16(redirects_[index.getIndex()].getVersion()+1));
        // add redirect to free list
        setNextFree_(index.getIndex(), first_free_pos_);
        first_free_pos_ = index.getIndex();
    }

    inline void TightPool::remove(Index index, DestroyFunc destructor) {
        ASSERT_M(isValidIndex(index), "Invalid index.");

        u32 deleted_item_pos = getItemPos(index);

        u8* deleted = &data_[deleted_item_pos*item_size_];
        u32 last_item_pos = size()-1;
        u8* last = &data_[last_item_pos*item_size_];

        // call destructor on item data
        destructor(deleted);
        // move last item to deleted place
        memcpy(deleted, last, item_size_);
        // update redirect for last(moved) item
        u32 redirect_id = redirect_ids_[last_item_pos];
        redirects_[redirect_id].setIndex(deleted_item_pos);
        redirect_ids_[deleted_item_pos] = redirect_id;

        data_.resize(last_item_pos*item_size_);
        redirect_ids_.resize(last_item_pos);

        redirects_[index.getIndex()].setIndex(InvalidId());

        // increment redirect version of removed item
        redirects_[index.getIndex()].setVersion(u16(redirects_[index.getIndex()].getVersion()+1));
        // add redirect to free list
        setNextFree_(index.getIndex(), first_free_pos_);
        first_free_pos_ = index.getIndex();
    }

    inline void TightPool::reserve(size_t count) {
        data_.reserve(item_size_*count);
        redirect_ids_.reserve(count);
        redirects_.reserve(count);
    }

    inline u32 TightPool::getItemPos(const u8* item)const {
        u32 ptr_dist = item-&data_[0];
        ASSERT(ptr_dist%item_size_ == 0);
        u32 pos = ptr_dist/item_size_;
        ASSERT(i32(pos)>=0 && pos < size());
        return pos;
    }

    inline u32 TightPool::getItemPos(Index index)const {
        ASSERT_M(isValidIndex(index), "Invalid index.");
        return redirects_[index.getIndex()].getIndex();
    }

    inline u8* TightPool::get(Index index) {
        ASSERT_M(isValidIndex(index), "Invalid index.");
        return &data_[getItemPos(index)*item_size_];
    }

    inline const u8* TightPool::get(Index index)const {
        ASSERT_M(isValidIndex(index), "Invalid index.");
        return &data_[getItemPos(index)*item_size_];
    }

    inline u8* TightPool::getAtPos(u32 pos) {
        return &data_[pos*item_size_];
    }

    inline const u8* TightPool::getAtPos(u32 pos)const {
        return &data_[pos*item_size_];
    }

    inline u8* TightPool::getAtPos2(u32 pos) {
        return getAtPos(pos);
    }

    inline const u8* TightPool::getAtPos2(u32 pos)const {
        return getAtPos(pos);
    }

    inline Index TightPool::getIndexForPos(u32 pos) {
        u32 redirect_id = redirect_ids_[pos];
        return Index(redirect_id, redirects_[redirect_id].getVersion());
    }

    inline void TightPool::getIndexForPos2(u32 pos, Index& index_out) {
        u32 redirect_id = redirect_ids_[pos];
        index_out.setIndex(redirect_id);
        index_out.setVersion(redirects_[redirect_id].getVersion());
    }

    inline bool TightPool::isValidIndex(Index index) const {
        if (index.getIndex() >= redirects_.size())
            return false;
        return redirects_[index.getIndex()].getIndex() != InvalidId();
    }

    inline u8* TightPool::getData() {
        return &data_[0];
    }

    inline u32 TightPool::size()const {
        return (u32)redirect_ids_.size();
    }

    inline bool TightPool::empty()const {
        return redirect_ids_.empty();
    }

    inline void TightPool::clear() {
        data_.clear();
        redirect_ids_.clear();
        redirects_.clear();
    }

    inline void TightPool::clear(DestroyFunc destructor) {
        for (u32 i=0; i<size(); ++i) {
            destructor(getAtPos(i));
        }
        clear();
    }

    inline u32 TightPool::getNextFree_(u32 pos)const {
        return *(u32*)&redirects_[pos];
    }

    inline void TightPool::setNextFree_(u32 pos, u32 next_free_pos) {
        *(u32*)&redirects_[pos] = next_free_pos;
    }

}