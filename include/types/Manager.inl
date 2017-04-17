#include "Manager.h"
#include "Type.h"

#define MGR_TPL template <typename T, class AT >
#define MGR_TYPE Manager<T, AT>

namespace grynca {

    MGR_TPL
    template <typename...ConstructionArgs>
    inline T& MGR_TYPE::addItem(ConstructionArgs&&... args) {
        IndexType id;
        ItemType& item = items_.add2(id, std::forward<ConstructionArgs>(args)...);
        item.id_ = id;
        item.manager_ = (typename ItemType::ManagerType*)(this);
        item.init();
        return item;
    }

    MGR_TPL
    inline T& MGR_TYPE::getItem(IndexType id) {
        return items_.get(id);
    }

    MGR_TPL
    inline const T& MGR_TYPE::getItem(IndexType id)const  {
        return items_.get(id);
    }

    MGR_TPL
    inline T* MGR_TYPE::getItemAtPos(u32 pos) {
        return items_.getAtPos(pos);
    }

    MGR_TPL
    inline const T* MGR_TYPE::getItemAtPos(u32 pos)const {
        return items_.getAtPos(pos);
    }

    MGR_TPL
    inline T& MGR_TYPE::getItemAtPos2(u32 pos) {
        return items_.getAtPos2(pos);
    }

    MGR_TPL
    inline const T& MGR_TYPE::getItemAtPos2(u32 pos)const {
        return items_.getAtPos2(pos);
    }

    MGR_TPL
    inline Index MGR_TYPE::getIndexForPos(u32 pos) {
        return items_.getIndexForPos(pos);
    }

    MGR_TPL
    inline u32 MGR_TYPE::getItemPos(Index index)const {
        return items_.getItemPos(index);
    }

    MGR_TPL
    inline void MGR_TYPE::removeItem(IndexType id) {
        items_.remove(id);
    }

    MGR_TPL
    inline void MGR_TYPE::reserveSpaceForItems(size_t count) {
        items_.reserve(count);
    }

    MGR_TPL
    inline bool MGR_TYPE::isValidIndex(IndexType index)const {
        return items_.isValidIndex(index);
    }

    MGR_TPL
    inline u32 MGR_TYPE::getItemsCount()const {
        return items_.size();
    }

    MGR_TPL
    inline bool MGR_TYPE::empty() {
        return items_.empty();
    }

    template <typename Derived, typename BaseType>
    inline ManagerSingletons<Derived, BaseType>::~ManagerSingletons() {
        for (u32 i=0; i<items_.size(); ++i) {
            if (items_[i])
                delete items_[i];
        }
    };

    template <typename Derived, typename BaseType>
    template <typename T>
    inline T& ManagerSingletons<Derived, BaseType>::get() {
        u32 tid = Type<T,  Derived>::getInternalTypeId();
        if (tid >= items_.size()) {
            items_.resize(tid+1, NULL);
        }
        if (!items_[tid]) {
            T* item = new T();
            item->id_ = tid;
            item->manager_ = (typename ItemType::ManagerType*)(this);
            item->init();
            items_[tid] = item;
        }
        return *(T*)items_[tid];
    }

    template <typename Derived, typename BaseType>
    inline BaseType* ManagerSingletons<Derived, BaseType>::getById(IndexType id) {
        return items_[id];
    }

    template <typename Derived, typename BaseType>
    inline const BaseType* ManagerSingletons<Derived, BaseType>::getById(IndexType id)const {
        return items_[id];
    };

    template <typename Derived, typename BaseType>
    const TypeInfo& ManagerSingletons<Derived, BaseType>::getTypeInfo(IndexType id)const {
        return InternalTypes<Derived>::getInfo(id);
    }


    template <typename Derived, typename BaseType>
    inline u32 ManagerSingletons<Derived, BaseType>::getSize() {
        return u32(items_.size());
    };

    template <typename ItemType>
    inline ManagedItemPtr<ItemType>::ManagedItemPtr()
     : manager_(NULL)
    {}

    template <typename ItemType>
    inline ManagedItemPtr<ItemType>::ManagedItemPtr(ManagerType& manager, IndexType index)
     : manager_(&manager), id_(index)
    {}

    template <typename ItemType>
    inline ManagedItemPtr<ItemType>::ManagedItemPtr(const ManagedItemRef<ItemType>& ref)
     : manager_(ref.manager_), id_(ref.id_)
    {}

    template <typename ItemType>
    inline ManagedItemPtr<ItemType>::ManagedItemPtr(const ManagedItemPtr<ItemType>& ptr)
     : manager_(ptr.manager_), id_(ptr.id_)
    {}

    template <typename ItemType>
    inline ManagedItemPtr<ItemType>::ManagedItemPtr(ItemType& item)
     : manager_(item.manager_), id_(item.id_)
    {}

    template <typename ItemType>
    inline ManagedItemPtr<ItemType>::ManagedItemPtr(ItemType* item)
     : manager_(item->manager_), id_(item->id_)
    {}


    template <typename ItemType>
    inline ItemType& ManagedItemPtr<ItemType>::get()const {
        ASSERT(manager_);
        return manager_->getItem(id_);
    }

    template <typename ItemType>
    inline ItemType* ManagedItemPtr<ItemType>::getPtr()const {
        ASSERT(manager_);
        return &manager_->getItem(id_);
    }

    template <typename ItemType>
    inline typename ItemType::IndexType ManagedItemPtr<ItemType>::getItemId()const {
        return id_;
    }

    template <typename ItemType>
    inline typename ItemType::ManagerType& ManagedItemPtr<ItemType>::getManager()const {
        ASSERT(manager_);
        return *manager_;
    }

    template <typename ItemType>
    inline bool ManagedItemPtr<ItemType>::isNull()const {
        return manager_!=NULL;
    }

    template <typename ItemType>
    inline bool ManagedItemPtr<ItemType>::isValid()const {
        ASSERT(manager_);
        return manager_->isValidIndex(id_);
    }

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
    inline ManagedItemRef<ItemType>::ManagedItemRef(const ManagedItemPtr<ItemType>& ptr)
     : manager_(ptr.manager_), id_(ptr.id_)
    {
        if (manager_)
            get().ref_count_.ref();
    }

    template <typename ItemType>
    inline ManagedItemRef<ItemType>::ManagedItemRef(ManagedItemRef<ItemType>&& ref)
     : manager_(ref.manager_), id_(ref.id_)
    {
        ref.manager_ = NULL;
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
    inline ManagedItemRef<ItemType>& ManagedItemRef<ItemType>::operator =(ManagedItemRef<ItemType>&& ref) {
        manager_ = ref.manager_;
        id_ = ref.id_;
        ref.manager_ = NULL;
        return *this;
    }

    template <typename ItemType>
    inline ManagedItemRef<ItemType>::~ManagedItemRef() {
        if (manager_)
            unref_();
    }

    template <typename ItemType>
    inline ItemType& ManagedItemRef<ItemType>::get()const {
        ASSERT(manager_);
        return manager_->getItem(id_);
    }

    template <typename ItemType>
    inline ItemType* ManagedItemRef<ItemType>::getPtr()const {
        ASSERT(manager_);
        return &manager_->getItem(id_);
    }

    template <typename ItemType>
    inline typename ItemType::IndexType ManagedItemRef<ItemType>::getItemId()const {
        return id_;
    }

    template <typename ItemType>
    typename ItemType::ManagerType& ManagedItemRef<ItemType>::getManager()const {
        ASSERT(manager_);
        return *manager_;
    }

    template <typename ItemType>
    inline bool ManagedItemRef<ItemType>::isNull()const {
        return manager_!=NULL;
    }

    template <typename ItemType>
    inline bool ManagedItemRef<ItemType>::isValid()const {
        ASSERT(manager_);
        return manager_->isValidIndex(id_);
    }

    template <typename ItemType>
    inline void ManagedItemRef<ItemType>::unref_() {
        if (!get().ref_count_.unref()) {
            manager_->removeItem(id_);
        }
    }
}

#undef MGR_TPL
#undef MGR_TYPE