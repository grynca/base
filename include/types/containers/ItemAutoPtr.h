#ifndef ITEMAUTOPTR_H
#define ITEMAUTOPTR_H

#include "ItemPtr.h"

namespace grynca {

    template <typename CT, typename IT>
    class ItemAutoPtr : public ItemPtr<CT, IT> {
        typedef ItemPtr<CT, IT> Base;
    public:
        typedef CT ContainerType;
        typedef IT ItemType;
        typedef typename ContainerType::ItemIndexType IndexType;
    public:
        ItemAutoPtr() : Base() {}
        ItemAutoPtr(ContainerType& container, IndexType index) : Base(container, index) {}
        ItemAutoPtr(const ItemRefPtr<ContainerType, ItemType>& ref) : Base(ref) {}
        ItemAutoPtr(const ItemPtr<ContainerType, ItemType>& ptr) : Base(ptr) {}
        ItemAutoPtr(ItemAutoPtr&& ptr) : Base(ptr.container_, ptr.id_) {
            ptr.makeNull();
        }

        ~ItemAutoPtr() {
            if (!this->isNull()) {
                this->getContainer()->removeItem(this->getItemId());
            }
        }
    };

}

#endif //ITEMAUTOPTR_H
