#include "ItemRefPtr.h"

#define IRP_TPL template <typename CT, typename IT>
#define IRP_TYPE ItemRefPtr<CT, IT>

namespace grynca {

    IRP_TPL
    inline IRP_TYPE::ItemRefPtr()
     : container_(NULL)
    {}

    IRP_TPL
    inline IRP_TYPE::ItemRefPtr(ContainerType& container, IndexType index)
     : container_(&container), id_(index)
    {
        acc().ref();
    }

    IRP_TPL
    inline IRP_TYPE::ItemRefPtr(const IRP_TYPE& ref)
     : container_(ref.container_), id_(ref.id_)
    {
        if (container_)
            acc().ref();
    }

    IRP_TPL
    inline IRP_TYPE::ItemRefPtr(const ItemPtr<ContainerType, ItemType>& ptr)
     : container_(ptr.container_), id_(ptr.id_)
    {
        if (container_)
            acc().ref();
    }

    IRP_TPL
    inline IRP_TYPE::ItemRefPtr(IRP_TYPE&& ref)
     : container_(ref.container_), id_(ref.id_)
    {
        ref.container_ = NULL;
    }

    IRP_TPL
    inline IRP_TYPE& IRP_TYPE::operator =(const IRP_TYPE& ref) {
        if (container_)
            unref_();
        container_ = ref.container_;
        id_ = ref.id_;
        if (container_)
            acc().ref();
        return *this;
    }

    IRP_TPL
    inline IRP_TYPE& IRP_TYPE::operator =(IRP_TYPE&& ref) {
        container_ = ref.container_;
        id_ = ref.id_;
        ref.container_ = NULL;
        return *this;
    }

    IRP_TPL
    inline IRP_TYPE::~ItemRefPtr() {
        if (container_)
            unref_();
    }

    IRP_TPL
    inline const IT& IRP_TYPE::get()const {
        ASSERT(container_);
        return container_->getItem(id_);
    }

    IRP_TPL
    inline const IT* IRP_TYPE::getPtr()const {
        ASSERT(container_);
        return &container_->getItem(id_);
    }

    IRP_TPL
    inline IT& IRP_TYPE::acc() {
        ASSERT(container_);
        return container_->accItem(id_);
    }

    IRP_TPL
    inline IT* IRP_TYPE::accPtr() {
        ASSERT(container_);
        return &container_->accItem(id_);
    }

    IRP_TPL
    inline typename IRP_TYPE::IndexType IRP_TYPE::getItemId()const {
        return id_;
    }

    IRP_TPL
    inline CT& IRP_TYPE::getContainer()const {
        ASSERT(container_);
        return *container_;
    }

    IRP_TPL
    inline bool IRP_TYPE::isNull()const {
        return container_==NULL;
    }

    IRP_TPL
    inline bool IRP_TYPE::isValid()const {
        ASSERT(container_);
        return container_->isValidIndex(id_);
    }

    IRP_TPL
    inline void IRP_TYPE::makeNull() {
        if (container_) {
            unref_();
            container_ = NULL;
        }
    }

    IRP_TPL
    inline void IRP_TYPE::unref_() {
        if (!acc().unref()) {
            container_->removeItem(id_);
        }
    }

}

#undef IRP_TPL
#undef IRP_TYPE