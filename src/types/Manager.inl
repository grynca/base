#include "Manager.h"

namespace grynca {

    template <typename ItemType>
    inline ItemType& Manager<ItemType>::addItem() {
        uint32_t id = items_.add();
        ItemType& item = items_.get(id);
        item.id_ = id;
        item.manager_ = reinterpret_cast<typename ItemType::ManagerType*>(this);
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
    inline ItemType& ManagerVersioned<ItemType>::addItem() {
        VersionedIndex id = items_.add();
        ItemType& item = items_.get(id);
        item.id_ = id;
        item.manager_ = reinterpret_cast<typename ItemType::ManagerType*>(this);
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

}
