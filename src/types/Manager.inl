#include "Manager.h"
#include "Type.h"

namespace grynca {

    template <typename T>
    ManagedItem<T>::ManagedItem()
     : id_(Index::Invalid()), manager_(NULL), ref_count_(0)
    {}

    template <typename T>
    ManagedItemVersioned<T>::ManagedItemVersioned()
            : id_(IndexType::Invalid()), manager_(NULL), ref_count_(0)
    {}

    template <typename T>
    template <typename...ConstructionArgs>
    inline T& Manager<T>::addItem(ConstructionArgs&&... args) {
        IndexType id = items_.add(std::forward<ConstructionArgs>(args)...);
        ItemType& item = items_.get(id);
        item.id_ = id;
        item.manager_ = (typename ItemType::ManagerType*)(this);
        return item;
    }

    template <typename T>
    inline T& Manager<T>::getItem(IndexType id) {
        return items_.get(id);
    }

    template <typename T>
    inline const T& Manager<T>::getItem(IndexType id)const  {
        return items_.get(id);
    }

    template <typename T>
    inline T& Manager<T>::getItemAtPos(uint32_t pos) {
        return items_.getAtPos(pos);
    }

    template <typename T>
    inline const T& Manager<T>::getItemAtPos(uint32_t pos)const {
        return items_.getAtPos(pos);
    }

    template <typename T>
    inline void Manager<T>::removeItem(IndexType id) {
        items_.remove(id);
    }

    template <typename T>
    inline void Manager<T>::reserveSpaceForItems(size_t count) {
        items_.reserve(count);
    }

    template <typename T>
    inline bool Manager<T>::isValidIndex(IndexType index)const {
        return items_.isValidIndex(index);
    }

    template <typename T>
    inline uint32_t Manager<T>::getItemsCount()const {
        return items_.size();
    }

    template <typename T>
    inline bool Manager<T>::empty() {
        return items_.empty();
    }

    template <typename T>
    template <typename...ConstructionArgs>
    inline T& ManagerVersioned<T>::addItem(ConstructionArgs&&... args) {
        IndexType id = items_.add(std::forward<ConstructionArgs>(args)...);
        ItemType& item = items_.get(id);
        item.id_ = id;
        item.manager_ = (typename ItemType::ManagerType*)(this);
        return item;
    }

    template <typename T>
    inline T& ManagerVersioned<T>::getItem(IndexType id) {
        return items_.get(id);
    }

    template <typename T>
    inline const T& ManagerVersioned<T>::getItem(IndexType id)const {
        return items_.get(id);
    }

    template <typename T>
    inline T& ManagerVersioned<T>::getItemAtPos(uint32_t pos) {
        return items_.getAtPos(pos);
    }

    template <typename T>
    inline const T& ManagerVersioned<T>::getItemAtPos(uint32_t pos)const {
        return items_.getAtPos(pos);
    }

    template <typename T>
    inline void ManagerVersioned<T>::removeItem(IndexType id) {
        return items_.remove(id);
    }

    template <typename T>
    inline void ManagerVersioned<T>::reserveSpaceForItems(size_t count) {
        items_.reserve(count);
    }

    template <typename T>
    inline bool ManagerVersioned<T>::isValidIndex(IndexType index)const {
        return items_.isValidIndex(index);
    }

    template <typename T>
    inline uint32_t ManagerVersioned<T>::getItemsCount()const {
        return items_.size();
    }

    template <typename T>
    inline bool ManagerVersioned<T>::empty() {
        return items_.empty();
    }


    template <typename Derived, typename BaseType>
    inline ManagerSingletons<Derived, BaseType>::~ManagerSingletons() {
        for (uint32_t i=0; i<items_.size(); ++i) {
            if (items_[i])
                delete items_[i];
        }
    };

    template <typename Derived, typename BaseType>
    template <typename T>
    inline T& ManagerSingletons<Derived, BaseType>::get() {
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

    template <typename Derived, typename BaseType>
    inline BaseType* ManagerSingletons<Derived, BaseType>::getById(uint32_t id) {
        return items_[id];
    }

    template <typename Derived, typename BaseType>
    inline const BaseType* ManagerSingletons<Derived, BaseType>::getById(uint32_t id)const {
        return items_[id];
    };

    template <typename Derived, typename BaseType>
    inline uint32_t ManagerSingletons<Derived, BaseType>::getSize() {
        return items_.size();
    };

    template <typename ItemType>
    inline ManagedItemRef<ItemType>::ManagedItemRef()
     : manager_(NULL)
    {}

    template <typename ItemType>
    inline ManagedItemRef<ItemType>::ManagedItemRef(ManagerType& manager, IndexType index)
     : manager_(&manager), id_(index)
    {
        get().ref_count_.ref();
    }

    template <typename ItemType>
    inline ManagedItemRef<ItemType>::ManagedItemRef(const ManagedItemRef<ItemType>& ref)
     : manager_(ref.manager_), id_(ref.id_)
    {
        if (manager_)
            get().ref_count_.ref();
    }

    template <typename ItemType>
    inline ManagedItemRef<ItemType>::ManagedItemRef(ItemType& item)
     : manager_(item.manager_), id_(item.id_)
    {
        if (manager_)
            get().ref_count_.ref();
    }

    template <typename ItemType>
    inline ManagedItemRef<ItemType>& ManagedItemRef<ItemType>::operator =(const ManagedItemRef<ItemType>& ref) {
        if (manager_)
            unref_();
        manager_ = ref.manager_;
        id_ = ref.id_;
        if (manager_)
            get().ref_count_.ref();
        return *this;
    }

    template <typename ItemType>
    inline ManagedItemRef<ItemType>::~ManagedItemRef() {
        if (manager_)
            unref_();
    }

    template <typename ItemType>
    inline ItemType& ManagedItemRef<ItemType>::get()const {
        assert(manager_);
        return manager_->getItem(id_);
    }

    template <typename ItemType>
    inline void ManagedItemRef<ItemType>::unref_() {
        if (!get().ref_count_.unref()) {
            manager_->removeItem(id_);
        }
    }
}
