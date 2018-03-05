#include "Manager.h"
#include "Type.h"

#define MIP_TPL template <typename IT>
#define MIP_TYPE ManagedItemPtr<IT>
#define MIAP_TPL template <typename IT>
#define MIAP_TYPE ManagedItemAutoPtr<IT>
#define MIR_TPL template <typename IT>
#define MIR_TYPE ManagedItemRefPtr<IT>
#define MGR_TPL template <typename T, class AT >
#define MGR_TYPE Manager<T, AT>
#define MS_TPL template <typename BaseType>
#define MS_TYPE ManagerSingletons<BaseType>

namespace grynca {

    MIP_TPL
    inline MIP_TYPE::ManagedItemPtr()
     : Base()
    {}

    MIP_TPL
    inline MIP_TYPE::ManagedItemPtr(ContainerType& container, IndexType index)
     : Base(container, index)
    {}

    MIP_TPL
    inline MIP_TYPE::ManagedItemPtr(const ItemRefPtr<ContainerType, IT>& ref)
     : Base(ref)
    {}

    MIP_TPL
    inline MIP_TYPE::ManagedItemPtr(const ItemPtr<ContainerType, IT>& ptr)
     : Base(ptr)
    {}

    MIP_TPL
    inline MIP_TYPE::ManagedItemPtr(const IT& item)
     : Base(item.getManager(), item.getId())
    {}

    MIP_TPL
    inline MIP_TYPE::ManagedItemPtr(const IT* item)
     : Base(item->getManager(), item->getId())
    {}

    MIAP_TPL
    inline MIAP_TYPE::ManagedItemAutoPtr()
     : Base()
    {}

    MIAP_TPL
    inline MIAP_TYPE::ManagedItemAutoPtr(ContainerType& container, IndexType index)
     : Base(container, index)
    {}

    MIAP_TPL
    inline MIAP_TYPE::ManagedItemAutoPtr(const ItemRefPtr<ContainerType, IT>& ref)
     : Base(ref)
    {}

    MIAP_TPL
    inline MIAP_TYPE::ManagedItemAutoPtr(const ItemPtr<ContainerType, IT>& ptr)
     : Base(ptr)
    {}

    MIAP_TPL
    inline MIAP_TYPE::ManagedItemAutoPtr(ItemAutoPtr<ContainerType, IT>&& ptr)
     : Base(std::forward(ptr))
    {}

    MIAP_TPL
    inline MIAP_TYPE::ManagedItemAutoPtr(const IT& item)
     : Base(item.getManager(), item.getId())
    {}

    MIAP_TPL
    inline MIAP_TYPE::ManagedItemAutoPtr(const IT* item)
     : Base(item->getManager(), item->getId())
    {}

    MIR_TPL
    inline MIR_TYPE::ManagedItemRefPtr()
     : Base()
    {}

    MIR_TPL
    inline MIR_TYPE::ManagedItemRefPtr(ContainerType& container, IndexType index)
     : Base(container, index)
    {}

    MIR_TPL
    inline MIR_TYPE::ManagedItemRefPtr(const ItemRefPtr<ContainerType, IT>& ref)
     : Base(ref)
    {}

    MIR_TPL
    inline MIR_TYPE::ManagedItemRefPtr(const ItemPtr<ContainerType, IT>& ptr)
     : Base(ptr)
    {}

    MIR_TPL
    inline MIR_TYPE::ManagedItemRefPtr(ItemRefPtr<ContainerType, IT>&& ref)
     : Base(std::forward(ref))
    {}

    MIR_TPL
    inline MIR_TYPE::ManagedItemRefPtr(const IT& item)
     : Base(item.getManager(), item.getId())
    {}

    MIR_TPL
    inline MIR_TYPE::ManagedItemRefPtr(const IT* item)
     : Base(item->getManager(), item->getId())
    {}

    MGR_TPL
    template <typename...ConstructionArgs>
    inline T& MGR_TYPE::addItem(ConstructionArgs&&... args) {
        ItemIndexType id;
        ItemType& item = items_.add2(id, std::forward<ConstructionArgs>(args)...);
        item.id_ = id;
        item.manager_ = (typename ItemType::ManagerType*)(this);
        item.afterAdded();
        return item;
    }

    MGR_TPL
    inline T& MGR_TYPE::accItem(ItemIndexType id) {
        return items_.accItem(id);
    }

    MGR_TPL
    inline const T& MGR_TYPE::getItem(ItemIndexType id)const  {
        return items_.getItem(id);
    }

    MGR_TPL
    inline T* MGR_TYPE::accItemAtPos(u32 pos) {
        return items_.accItemAtPos(pos);
    }

    MGR_TPL
    inline const T* MGR_TYPE::getItemAtPos(u32 pos)const {
        return items_.getItemAtPos(pos);
    }

    MGR_TPL
    inline T& MGR_TYPE::accItemAtPos2(u32 pos) {
        return items_.accItemAtPos2(pos);
    }

    MGR_TPL
    inline const T& MGR_TYPE::getItemAtPos2(u32 pos)const {
        return items_.getItemAtPos2(pos);
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
    inline void MGR_TYPE::removeItem(ItemIndexType id) {
        items_.removeItem(id);
    }

    MGR_TPL
    inline void MGR_TYPE::reserveSpaceForItems(size_t count) {
        items_.reserve(count);
    }

    MGR_TPL
    inline bool MGR_TYPE::isValidIndex(ItemIndexType index)const {
        return items_.isValidIndex(index);
    }

    MGR_TPL
    inline void MGR_TYPE::clear() {
        items_.clear();
    }

    MGR_TPL
    inline u32 MGR_TYPE::getItemsCount()const {
        return items_.size();
    }

    MGR_TPL
    inline bool MGR_TYPE::empty() {
        return items_.empty();
    }

    MS_TPL
    inline MS_TYPE::~ManagerSingletons() {
        for (u32 i=0; i<items_.size(); ++i) {
            if (!items_[i].ptr.isNull())
                items_[i].ptr.destroy();
        }
    };

    MS_TPL
    template <typename Types, typename... InitArgs>
    inline void MS_TYPE::initSingletons(InitArgs&&... args) {
        Types::template callOnTypes<TypesInitializer_>(*this, std::forward<InitArgs>(args)...);
    }

    MS_TPL
    template <typename T, typename... InitArgs>
    inline T& MS_TYPE::initSingleton(InitArgs&&... args) {
        u32 tid = addTypeIfNeeded_<T>();
        T& item = *items_[tid].ptr.template accAs<T>();
        item.initSingleton(std::forward<InitArgs>(args)...);
        return item;
    };

    MS_TPL
    template <typename T>
    inline T& MS_TYPE::get() {
        u32 tid = addTypeIfNeeded_<T>();
        return *items_[tid].ptr.template getAs<T>();
    }

    MS_TPL
    template <typename T>
    inline T& MS_TYPE::getFast() {
        u32 tid = Type<T,  Domain>::getInternalTypeId();
        ASSERT(tid < items_.size());
        ASSERT(!items_[tid].ptr.isNull());
        return *items_[tid].ptr.template accAs<T>();
    }

    MS_TPL
    inline const BaseType* MS_TYPE::tryGetById(ItemIndexType id)const {
        if (items_[id].ptr.isNull())
            return NULL;
        return getById(id);
    }

    MS_TPL
    inline BaseType* MS_TYPE::tryGetById(ItemIndexType id) {
        if (items_[id].ptr.isNull())
            return NULL;
        return getById(id);
    }

    MS_TPL
    inline BaseType* MS_TYPE::getById(ItemIndexType id) {
        return (BaseType*)(items_[id].ptr.getPtr() + items_[id].offset_to_base);
    }

    MS_TPL
    inline const BaseType* MS_TYPE::getById(ItemIndexType id)const {
        return (BaseType*)(items_[id].ptr.getPtr() + items_[id].offset_to_base);
    };

    MS_TPL
    const TypeInfo& MS_TYPE::getTypeInfo(ItemIndexType id)const {
        return InternalTypes<Domain>::getInfo(id);
    }

    MS_TPL
    template <typename T>
    inline u32 MS_TYPE::getTypeIdOf() {
        // static
        return Type<T,  Domain>::getInternalTypeId();
    }


    MS_TPL
    inline u32 MS_TYPE::getSize()const {
        return u32(items_.size());
    };

    MS_TPL
    template <typename T>
    inline u32 MS_TYPE::addTypeIfNeeded_() {
        u32 tid = Type<T,  Domain>::getInternalTypeId();
        if (tid >= items_.size()) {
            items_.resize(tid+1);
        }
        if (items_[tid].ptr.isNull()) {
            T* item = new T();
            item->id_ = tid;
            item->manager_ = (typename ItemType::ManagerType*)(this);
            items_[tid].ptr = item;
            items_[tid].offset_to_base = calcBaseOffset<T, BaseType>();
        }
        return tid;
    };

    MS_TPL
    template <typename TP, typename T, typename... InitArgs>
    inline void MS_TYPE::TypesInitializer_::f(ManagerSingletons& mgr, InitArgs&&... args) {
        // static
        mgr.addTypeIfNeeded_<T>();
        mgr.getFast<T>().initSingleton(std::forward<InitArgs>(args)...);
    }
}

#undef MIP_TPL
#undef MIP_TYPE
#undef MIAP_TPL
#undef MIAP_TYPE
#undef MIR_TPL
#undef MIR_TYPE
#undef MGR_TPL
#undef MGR_TYPE
#undef MS_TPL
#undef MS_TYPE