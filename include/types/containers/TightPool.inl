#include "TightPool.h"
#define FREE_BITMASK u16(1<<15)

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
            first_free_pos_ = takeNextFree_(first_free_pos_);
            redirect_version = redirects_[redirect_id].getVersion();
            redirects_[redirect_id].setIndex(item_pos);
        }
        redirect_ids_.push_back(redirect_id);
        ASSERT(redirect_version != u16(InvalidId()));
        return Index(redirect_id, redirect_version);
    }

    inline Index TightPool::add(u8*& new_item_out, u16 aux_id) {
        Index id = add(new_item_out);
        redirects_[id.getIndex()].setAuxIndex(aux_id);
        id.setAuxIndex(aux_id);
        return id;
    }

    inline void TightPool::removeItem(Index index) {
        ASSERT_M(isValidIndex(index), "Invalid index.");

        u32 deleted_item_pos = getItemPos(index);

        u8* deleted = &data_[deleted_item_pos*item_size_];
        u32 last_item_pos = size()-1;
        u8* last = &data_[last_item_pos*item_size_];

        // move last item to deleted place
        if (last_item_pos != deleted_item_pos) {
            memcpy(deleted, last, item_size_);
        }
        // update redirect for last(moved) item
        u32 redirect_id = redirect_ids_[last_item_pos];
        redirects_[redirect_id].setIndex(deleted_item_pos);
        redirect_ids_[deleted_item_pos] = redirect_id;

        data_.resize(last_item_pos*item_size_);
        redirect_ids_.resize(last_item_pos);

        redirects_[index.getIndex()].setIndex(u32(-1));
        // increment redirect version of removed item
        redirects_[index.getIndex()].setVersion(u16());
        // add redirect to free list
        setNextFree_(index.getIndex(), first_free_pos_);

        first_free_pos_ = index.getIndex();
    }

    inline void TightPool::removeItem(Index index, DestroyFunc destructor) {
        ASSERT_M(isValidIndex(index), "Invalid index.");

        u32 deleted_item_pos = getItemPos(index);

        u8* deleted = &data_[deleted_item_pos*item_size_];
        u32 last_item_pos = size()-1;
        u8* last = &data_[last_item_pos*item_size_];

        // call destructor on item data_
        destructor(deleted);
        // move last item to deleted place
        if (last_item_pos != deleted_item_pos) {
            memcpy(deleted, last, item_size_);
        }
        // update redirect for last(moved) item
        u32 redirect_id = redirect_ids_[last_item_pos];
        redirects_[redirect_id].setIndex(deleted_item_pos);
        redirect_ids_[deleted_item_pos] = redirect_id;

        data_.resize(last_item_pos*item_size_);
        redirect_ids_.resize(last_item_pos);

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

    inline u8* TightPool::accItem(Index index) {
        ASSERT_M(isValidIndex(index), "Invalid index.");
        return &data_[getItemPos(index)*item_size_];
    }

    inline const u8* TightPool::getItem(Index index)const {
        ASSERT_M(isValidIndex(index), "Invalid index.");
        return &data_[getItemPos(index)*item_size_];
    }

    inline u8* TightPool::accItemWithInnerIndex(u32 inner_id) {
        u32 pos = redirects_[inner_id].getIndex();
        return &data_[pos*item_size_];
    }

    inline const u8* TightPool::getItemWithInnerIndex(u32 inner_id)const {
        u32 pos = redirects_[inner_id].getIndex();
        return &data_[pos*item_size_];
    }

    inline u8* TightPool::accItemAtPos(u32 pos) {
        return &data_[pos*item_size_];
    }

    inline const u8* TightPool::getItemAtPos(u32 pos)const {
        return &data_[pos*item_size_];
    }

    inline u8* TightPool::accItemAtPos2(u32 pos) {
        return accItemAtPos(pos);
    }

    inline const u8* TightPool::getItemAtPos2(u32 pos)const {
        return getItemAtPos(pos);
    }

    inline Index TightPool::getIndexForPos(u32 pos)const {
        return getFullIndex(redirect_ids_[pos]);
    }

    inline u32 TightPool::getInnerIndexForPos(u32 pos)const {
        return redirect_ids_[pos];
    }

    inline void TightPool::getIndexForPos2(u32 pos, Index& index_out)const {
        u32 redirect_id = redirect_ids_[pos];
        index_out.setIndex(redirect_id);
        index_out.setVerAux(redirects_[redirect_id].getVerAux());
    }

    inline Index TightPool::getFullIndex(u32 index_id)const {
        return Index(index_id, redirects_[index_id].getVersion(), redirects_[index_id].getAuxIndex());
    }

    inline bool TightPool::isValidIndex(Index index) const {
        if (index.getIndex() >= redirects_.size())
            return false;

        return redirects_[index.getIndex()].getVersion() == index.getVersion();
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
            destructor(accItemAtPos(i));
        }
        clear();
    }

    inline bool TightPool::isFree_(u32 pos)const {
        return (redirects_[pos].getVersion() & FREE_BITMASK) != 0;
    }

    inline void TightPool::setFree_(u32 pos) {
        redirects_[pos].setVersion(redirects_[pos].getVersion() | FREE_BITMASK);
    }

    inline void TightPool::unsetFree_(u32 pos) {
        redirects_[pos].setVersion(redirects_[pos].getVersion() &~ FREE_BITMASK);
    }

    inline u32 TightPool::takeNextFree_(u32 pos) {
        unsetFree_(pos);
        return redirects_[pos].getIndex();
    }

    inline void TightPool::setNextFree_(u32 pos, u32 next_free_pos) {
        setFree_(pos);
        redirects_[pos].setIndex(next_free_pos);
    }

}

#undef FREE_BITMASK