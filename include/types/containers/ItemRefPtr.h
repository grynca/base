#ifndef ITEMREF_H
#define ITEMREF_H

#include "../RefCount.h"

namespace grynca {

    class RefCounted {
    public:
        RefCounted() : ref_count_(0) {}

        int getRefCount() { return ref_count_.get(); }
        void ref() { ref_count_.ref(); }
        bool unref() { return ref_count_.unref(); }
    protected:
        RefCount ref_count_;
    };

    template <typename CT, typename IT>
    class ItemRefPtr {
    public:
        typedef CT ContainerType;
        typedef IT ItemType;
        typedef typename ContainerType::ItemIndexType IndexType;
    public:
        ItemRefPtr();
        ItemRefPtr(ContainerType& container, IndexType index);
        ItemRefPtr(const ItemRefPtr<ContainerType, ItemType>& ref);
        ItemRefPtr(const ItemPtr<ContainerType, ItemType>& ptr);
        ItemRefPtr(ItemRefPtr<ContainerType, ItemType>&& ref);

        ItemRefPtr<ContainerType, ItemType>& operator =(const ItemRefPtr<ContainerType, ItemType>& ref);
        ItemRefPtr<ContainerType, ItemType>& operator =(ItemRefPtr<ContainerType, ItemType>&& ref);

        ~ItemRefPtr();

        const ItemType& get()const;
        const ItemType* getPtr()const;

        ItemType& acc();
        ItemType* accPtr();

        IndexType getItemId()const;
        ContainerType& getContainer()const;

        bool isNull()const;
        bool isValid()const;
        void makeNull();

        ItemType* operator->() { return &acc(); }
        const ItemType* operator->()const { return &get(); }
        ItemType& operator*() { return acc(); }
        const ItemType& operator*()const { return get(); }
    protected:
        friend class ItemPtr<ContainerType, ItemType>;

        void unref_();

        ContainerType* container_;       // container must be non-volatile (e.g. not in another container)
        IndexType id_;
    };

}

#include "ItemRefPtr.inl"
#endif //ITEMREF_H
