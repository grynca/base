#include "Manager.h"
#include "Type.h"

namespace grynca {

    template <typename ItemType>
    template <typename...ConstructionArgs>
    inline ItemType& Manager<ItemType>::addItem(ConstructionArgs&&... args) {
        uint32_t id = items_.add(std::forward<ConstructionArgs>(args)...);
        ItemType& item = items_.get(id);
        item.id_ = id;
        item.manager_ = (typename ItemType::ManagerType*)(this);
        return item;
    }

    template <typename ItemType>
    inline ItemType& Manager<ItemType>::getItem(uint32_t id) {
        return items_.get(id);
    }

    template <typename ItemType>
    inline ItemType& Manager<ItemType>::getItemAtPos(uint32_t pos) {
        return items_.getAtPos(pos);
    }

    template <typename ItemType>
    inline void Manager<ItemType>::removeItem(uint32_t id) {
        items_.remove(id);
    }

    template <typename ItemType>
    inline void Manager<ItemType>::reserveSpaceForItems(size_t count) {
        items_.reserve(count);
    }

    template <typename ItemType>
    inline uint32_t Manager<ItemType>::getItemsCount() {
        return items_.size();
    }

    template <typename ItemType>
    inline bool Manager<ItemType>::empty() {
        return items_.empty();
    }

    template <typename ItemType>
    template <typename...ConstructionArgs>
    inline ItemType& ManagerVersioned<ItemType>::addItem(ConstructionArgs&&... args) {
        VersionedIndex id = items_.add(std::forward<ConstructionArgs>(args)...);
        ItemType& item = items_.get(id);
        item.id_ = id;
        item.manager_ = (typename ItemType::ManagerType*)(this);
        return item;
    }

    template <typename ItemType>
    inline ItemType& ManagerVersioned<ItemType>::getItem(VersionedIndex id) {
        return items_.get(id);
    }

    template <typename ItemType>
    inline ItemType& ManagerVersioned<ItemType>::getItemAtPos(uint32_t pos) {
        return items_.getAtPos(pos);
    }

    template <typename ItemType>
    inline void ManagerVersioned<ItemType>::removeItem(VersionedIndex id) {
        return items_.remove(id);
    }

    template <typename ItemType>
    inline void ManagerVersioned<ItemType>::reserveSpaceForItems(size_t count) {
        items_.reserve(count);
    }

    template <typename ItemType>
    inline bool ManagerVersioned<ItemType>::isValidIndex(VersionedIndex index) {
        return items_.isValidIndex(index);
    }

    template <typename ItemType>
    inline uint32_t ManagerVersioned<ItemType>::getItemsCount() {
        return items_.size();
    }

    template <typename ItemType>
    inline bool ManagerVersioned<ItemType>::empty() {
        return items_.empty();
    }


    template <typename Derived, typename ItemType>
    inline ManagerSingletons<Derived, ItemType>::~ManagerSingletons() {
        for (uint32_t i=0; i<items_.size(); ++i) {
            if (items_[i])
                delete items_[i];
        }
    };

    template <typename Derived, typename ItemType>
    template <typename T>
    inline T& ManagerSingletons<Derived, ItemType>::get() {
        uint32_t tid = Type<T,  Derived>::getInternalTypeId();
        if (tid >= items_.size()) {
            items_.resize(tid+1, NULL);
        }
        if (!items_[tid]) {
            T* item = new T();
            item->id_ = tid;
            item->manager_ = (typename ItemType::ManagerType*)(this);
            items_[tid] = item;
        }
        return *(T*)items_[tid];
    }

    template <typename Derived, typename ItemType>
    inline ItemType* ManagerSingletons<Derived, ItemType>::getById(uint32_t id) {
        return items_[id];
    }

    template <typename Derived, typename ItemType>
    inline uint32_t ManagerSingletons<Derived, ItemType>::getSize() {
        return items_.size();
    };
}
