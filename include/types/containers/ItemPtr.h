#ifndef ITEMREFPTR_H
#define ITEMREFPTR_H

namespace grynca {

    // fw
    template <typename CT, typename IT> class ItemRefPtr;

    template <typename CT, typename IT>
    class ItemPtr {
    public:
        typedef CT ContainerType;
        typedef IT ItemType;
        typedef typename ContainerType::ItemIndexType IndexType;
    public:
        ItemPtr();
        ItemPtr(ContainerType& container, IndexType index);
        ItemPtr(const ItemRefPtr<ContainerType, ItemType>& ref);
        ItemPtr(const ItemPtr<ContainerType, ItemType>& ptr);

        ItemType& acc();
        ItemType* accPtr();

        const ItemType& get()const;
        const ItemType* getPtr()const;

        IndexType getItemId()const;
        ContainerType& getContainer()const;

        bool isNull()const;
        bool isValid()const;
        void makeNull();

        ItemType* operator->() { return &acc(); }
        const ItemType* operator->()const { return &get(); }
    protected:
        friend class ItemRefPtr<ContainerType, ItemType>;

        ContainerType* container_;       // container must be non-volatile (e.g. not in another container)
        IndexType id_;
    };

}

#include "ItemPtr.inl"
#endif //ITEMREFPTR_H
