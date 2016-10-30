#include "TightPool.h"

namespace grynca {

    inline TightPool::TightPool(uint32_t item_size)
     : item_size_(item_size)
    {}

    inline Index TightPool::add(uint8_t*& new_item_out) {
        uint32_t item_slot_id = (uint32_t)size();
        size_t old_size_bytes = item_slot_id*item_size_;
        items_data_.resize(old_size_bytes + item_size_);
        new_item_out = &items_data_[old_size_bytes];

        uint32_t redirect_id;
        uint16_t redirect_version;
        if (free_redirects_.empty()) {
            redirect_id = (uint32_t)redirects_.size();
            redirect_version = 0;
            redirects_.emplace_back(item_slot_id, redirect_version);
        }
        else {
            redirect_id = free_redirects_.back();
            free_redirects_.pop_back();
            redirect_version = redirects_[redirect_id].getVersion();
            redirects_[redirect_id].setIndex(item_slot_id);
            redirects_[redirect_id].setVersion(redirect_version);
        }
        redirect_ids_.push_back(redirect_id);
        return Index(redirect_id, redirect_version);
    }

    inline void TightPool::remove(Index index) {
        ASSERT_M(isValidIndex(index), "Invalid index.");

        uint32_t deleted_item_slot_id = redirects_[index.getIndex()].getIndex();

        uint8_t* deleted = &items_data_[deleted_item_slot_id*item_size_];
        uint32_t last_item_slot_id = size()-1;
        uint8_t* last = &items_data_[last_item_slot_id*item_size_];

        // move last item to deleted place
        memcpy(deleted, last, item_size_);
        // update redirect for last(moved) item
        uint32_t redirect_id = redirect_ids_[last_item_slot_id];
        redirects_[redirect_id].setIndex(deleted_item_slot_id);
        redirect_ids_[deleted_item_slot_id] = redirect_id;

        items_data_.resize(last_item_slot_id*item_size_);
        redirect_ids_.resize(last_item_slot_id);

        redirects_[index.getIndex()].setIndex(uint32_t(-1));
        // increment redirect version of removed item
        redirects_[index.getIndex()].setVersion(uint16_t(redirects_[index.getIndex()].getVersion()+1));
        // add redirect to free list
        free_redirects_.push_back(index.getIndex());
    }

    inline void TightPool::remove(Index index, DestroyFunc destructor) {
        ASSERT_M(isValidIndex(index), "Invalid index.");

        uint32_t deleted_item_slot_id = redirects_[index.getIndex()].getIndex();

        uint8_t* deleted = &items_data_[deleted_item_slot_id*item_size_];
        uint32_t last_item_slot_id = size()-1;
        uint8_t* last = &items_data_[last_item_slot_id*item_size_];

        // call destructor on item data
        destructor(deleted);
        // move last item to deleted place
        memcpy(deleted, last, item_size_);
        // update redirect for last(moved) item
        uint32_t redirect_id = redirect_ids_[last_item_slot_id];
        redirects_[redirect_id].setIndex(deleted_item_slot_id);
        redirect_ids_[deleted_item_slot_id] = redirect_id;

        items_data_.resize(last_item_slot_id*item_size_);
        redirect_ids_.resize(last_item_slot_id);

        redirects_[index.getIndex()].setIndex(uint32_t(-1));

        // increment redirect version of removed item
        redirects_[index.getIndex()].setVersion(uint16_t(redirects_[index.getIndex()].getVersion()+1));
        // add redirect to free list
        free_redirects_.push_back(index.getIndex());
    }

    inline void TightPool::reserve(size_t count) {
        items_data_.reserve(item_size_*count);
        redirect_ids_.reserve(count);
        redirects_.reserve(count);
    }

    inline uint8_t* TightPool::get(Index index) {
        ASSERT_M(isValidIndex(index), "Invalid index.");
        uint32_t item_slot_id = redirects_[index.getIndex()].getIndex();
        return &items_data_[item_slot_id*item_size_];
    }

    inline uint8_t* TightPool::getAtPos(uint32_t pos) {
        return &items_data_[pos*item_size_];
    }

    inline const uint8_t* TightPool::get(Index index)const {
        ASSERT_M(isValidIndex(index), "Invalid index.");
        uint32_t item_slot_id = redirects_[index.getIndex()].getIndex();
        return &items_data_[item_slot_id*item_size_];
    }

    inline const uint8_t* TightPool::getAtPos(uint32_t pos)const {
        return &items_data_[pos*item_size_];
    }

    inline Index TightPool::getIndexForPos(uint32_t pos) {
        uint32_t redirect_id = redirect_ids_[pos];
        return Index(redirect_id, redirects_[redirect_id].getVersion());
    }

    inline void TightPool::getIndexForPos2(uint32_t pos, Index& index_out) {
        uint32_t redirect_id = redirect_ids_[pos];
        index_out.setIndex(redirect_id);
        index_out.setVersion(redirects_[redirect_id].getVersion());
    }

    inline bool TightPool::isValidIndex(Index index) const {
        if (index.getIndex() >= redirects_.size())
            return false;
        return redirects_[index.getIndex()].getIndex() !=uint32_t(-1);
    }

    inline uint8_t* TightPool::getData() {
        return &items_data_[0];
    }

    inline uint32_t TightPool::size()const {
        return (uint32_t)redirect_ids_.size();
    }

    inline bool TightPool::empty()const {
        return redirect_ids_.empty();
    }

    inline void TightPool::clear() {
        items_data_.clear();
        redirect_ids_.clear();
        redirects_.clear();
        free_redirects_.clear();
    }

    inline void TightPool::clear(DestroyFunc destructor) {
        for (uint32_t i=0; i<size(); ++i) {
            destructor(getAtPos(i));
        }
        clear();
    }
}