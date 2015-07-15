#include "UnsortedVersionedVector.h"

namespace grynca {

    template <typename T>
    inline UnsortedVersionedVector<T>::~UnsortedVersionedVector() {
        // destruct items
        for (unsigned int i=0; i<size(); ++i)
            getData()[i].~T();
    }

    template <typename T>
    template <typename ...ConstructionArgs>
    inline VersionedIndex UnsortedVersionedVector<T>::add(ConstructionArgs&&... args) {
        uint32_t item_slot_id = (uint32_t)size();
        size_t old_size_bytes = item_slot_id*sizeof(T);
        items_data_.resize(old_size_bytes + sizeof(T));
        void* new_item_place = &items_data_[old_size_bytes];
        // create item with placement new
        new(new_item_place) T(std::forward<ConstructionArgs>(args)...);

        uint32_t redirect_id;
        uint32_t redirect_version;
        if (free_redirects_.empty()) {
            redirect_id = (uint32_t)redirects_.size();
            redirect_version = 0;
            redirects_.push_back({redirect_version, item_slot_id});
        }
        else {
            redirect_id = free_redirects_.back();
            free_redirects_.pop_back();
            redirect_version = redirects_[redirect_id].version;
            redirects_[redirect_id] = {redirect_version, item_slot_id};
        }
        redirect_ids_.push_back(redirect_id);
        return {redirect_version, redirect_id};
    }

    template <typename T>
    inline void UnsortedVersionedVector<T>::remove(VersionedIndex index) {
        assert(isValidIndex(index));

        uint32_t deleted_item_slot_id = redirects_[index.index].index;

        T* deleted = ((T*)&items_data_[deleted_item_slot_id*sizeof(T)]);
        uint32_t last_item_slot_id = size()-1;
        T* last = ((T*)&items_data_[last_item_slot_id*sizeof(T)]);

        // call destructor on item data
        deleted->~T();
        // move last item to deleted place
        memmove(deleted, last, sizeof(T));
        // update redirect for last(moved) item
        uint32_t redirect_id = redirect_ids_[last_item_slot_id];
        redirects_[redirect_id].index = deleted_item_slot_id;
        redirect_ids_[deleted_item_slot_id] = redirect_id;

        items_data_.resize(last_item_slot_id*sizeof(T));
        redirect_ids_.resize(last_item_slot_id);

        redirects_[deleted_item_slot_id].index = uint32_t(-1);
        ++redirects_[deleted_item_slot_id].version;     // increment redirect version of removed item
        // add redirect to free list
        free_redirects_.push_back(index.index);
    }

    template <typename T>
    inline void UnsortedVersionedVector<T>::reserve(size_t count) {
        items_data_.reserve(sizeof(T)*count);
        redirect_ids_.reserve(count);
        redirects_.reserve(count);
    }

    template <typename T>
    inline T& UnsortedVersionedVector<T>::get(VersionedIndex index) {
        assert(isValidIndex(index));
        uint32_t item_slot_id = redirects_[index.index].index;
        return *((T*)&items_data_[item_slot_id*sizeof(T)]);
    }

    template <typename T>
    inline T& UnsortedVersionedVector<T>::getAtPos(uint32_t pos) {
        return *((T*)&items_data_[pos*sizeof(T)]);
    }

    template <typename T>
    inline bool UnsortedVersionedVector<T>::isValidIndex(VersionedIndex index) {
        if (index.index >= redirects_.size())
            return false;
        return redirects_[index.index].index!=uint32_t(-1);
    }


    template <typename T>
    inline T* UnsortedVersionedVector<T>::getData() {
        return (T*)&items_data_[0];
    }

    template <typename T>
    inline uint32_t UnsortedVersionedVector<T>::size() {
        return (uint32_t)redirect_ids_.size();
    }

    template <typename T>
    inline bool UnsortedVersionedVector<T>::empty() {
        return redirect_ids_.empty();
    }

}
