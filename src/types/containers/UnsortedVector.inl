#include "UnsortedVector.h"

namespace grynca {

    template <typename T>
    inline UnsortedVector<T>::~UnsortedVector() {
        // destruct items
        for (unsigned int i=0; i<size(); ++i)
            getData()[i].~T();
    }

    template <typename T>
    template <typename ...ConstructionArgs>
    inline uint32_t UnsortedVector<T>::add(ConstructionArgs&&... args) {
        uint32_t item_slot_id = size();
        size_t old_size_bytes = item_slot_id*sizeof(T);
        items_data_.resize(old_size_bytes + sizeof(T));
        void* new_item_place = &items_data_[old_size_bytes];
        // create item with placement new
        new(new_item_place) T(std::forward<ConstructionArgs>(args)...);

        uint32_t redirect_id;
        if (free_redirects_.empty()) {
            redirect_id = (uint32_t)redirects_.size();
            redirects_.push_back(item_slot_id);
        }
        else {
            redirect_id = free_redirects_.back();
            free_redirects_.pop_back();
            redirects_[redirect_id] = item_slot_id;
        }
        redirect_ids_.push_back(redirect_id);
        return redirect_id;
    }

    template <typename T>
    inline void UnsortedVector<T>::remove(uint32_t index) {
        ASSERT(isValidIndex(index), "Invalid index.");

        uint32_t deleted_item_slot_id = redirects_[index];

        T* deleted = ((T*)&items_data_[deleted_item_slot_id*sizeof(T)]);
        uint32_t last_item_slot_id = size()-1;
        T* last = ((T*)&items_data_[last_item_slot_id*sizeof(T)]);

        // call destructor on item data
        deleted->~T();
        // move last item to deleted place
        std::copy(last, last+1, deleted);
        // update redirect for last item
        uint32_t redirect_id = redirect_ids_[last_item_slot_id];
        redirects_[redirect_id] = deleted_item_slot_id;
        redirect_ids_[deleted_item_slot_id] = redirect_id;

        items_data_.resize(last_item_slot_id*sizeof(T));
        redirect_ids_.resize(last_item_slot_id);

        // add to free list
        free_redirects_.push_back(index);
        redirects_[index] = uint32_t(-1);
    }

    template <typename T>
    inline void UnsortedVector<T>::reserve(size_t count) {
        items_data_.reserve(sizeof(T)*count);
        redirect_ids_.reserve(count);
        redirects_.reserve(count);
    }

    template <typename T>
    inline T& UnsortedVector<T>::get(uint32_t index) {
        ASSERT(isValidIndex(index), "Invalid index.");

        uint32_t item_slot_id = redirects_[index];
        return *((T*)&items_data_[item_slot_id*sizeof(T)]);
    }

    template <typename T>
    inline T& UnsortedVector<T>::getAtPos(uint32_t pos) {
        return *((T*)&items_data_[pos*sizeof(T)]);
    }

    template <typename T>
    inline const T& UnsortedVector<T>::get(uint32_t index)const {
        return ((UnsortedVector<T>*)this)->get(index);
    }

    template <typename T>
    inline const T& UnsortedVector<T>::getAtPos(uint32_t pos)const {
        return ((UnsortedVector<T>*)this)->getAtPos(pos);
    }

    template <typename T>
    inline bool UnsortedVector<T>::isValidIndex(uint32_t index)const {
        if (index >= redirects_.size())
            return false;
        return redirects_[index]!=uint32_t(-1);
    }

    template <typename T>
    inline T* UnsortedVector<T>::getData() {
        return (T*)&items_data_[0];
    }

    template <typename T>
    inline uint32_t UnsortedVector<T>::size()const {
        return (uint32_t)redirect_ids_.size();
    }

    template <typename T>
    inline bool UnsortedVector<T>::empty()const {
        return redirect_ids_.empty();
    }
}
