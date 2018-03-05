#include "ItemPtr.h"

#define IP_TPL template <typename CT, typename IT>
#define IP_TYPE ItemPtr<CT, IT>

namespace grynca {

    IP_TPL
    inline IP_TYPE::ItemPtr()
     : container_(NULL)
    {}

    IP_TPL
    inline IP_TYPE::ItemPtr(ContainerType& container, IndexType index)
     : container_(&container), id_(index)
    {}

    IP_TPL
    inline IP_TYPE::ItemPtr(const ItemRefPtr<ContainerType, ItemType>& ref)
     : container_(ref.container_), id_(ref.id_)
    {}

    IP_TPL
    inline IP_TYPE::ItemPtr(const ItemPtr<ContainerType, ItemType>& ptr)
     : container_(ptr.container_), id_(ptr.id_)
    {}

    IP_TPL
    inline IT& IP_TYPE::acc() {
        ASSERT(container_);
        return container_->accItem(id_);
    }

    IP_TPL
    inline IT* IP_TYPE::accPtr() {
        ASSERT(container_);
        return &container_->accItem(id_);
    }

    IP_TPL
    inline const IT& IP_TYPE::get()const {
        ASSERT(container_);
        return container_->getItem(id_);
    }

    IP_TPL
    inline const IT* IP_TYPE::getPtr()const {
        ASSERT(container_);
        return &container_->getItem(id_);
    }

    IP_TPL
    inline typename IP_TYPE::IndexType IP_TYPE::getItemId()const {
        return id_;
    }

    IP_TPL
    inline CT& IP_TYPE::getContainer()const {
        ASSERT(container_);
        return *container_;
    }

    IP_TPL
    inline bool IP_TYPE::isNull()const {
        return container_==NULL;
    }

    IP_TPL
    inline bool IP_TYPE::isValid()const {
        ASSERT(container_);
        return container_->isValidIndex(id_);
    }

    IP_TPL
    inline void IP_TYPE::makeNull() {
        container_ = NULL;
    }

}

#undef IP_TPL
#undef IP_TYPE