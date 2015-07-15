#ifndef MANAGER_H
#define MANAGER_H

#include "containers/UnsortedVector.h"
#include "containers/UnsortedVersionedVector.h"

/*
 * EXAMPLE:
 * class StuffManager;
 * class MyStuff : public ManagedItem<StuffManager>
 *     {
 *     public:
 *          void init(int a, double b);
 *          ...
 *     };
 * class StuffManager : public Manager<MyStuff>
 *     {
 *     public:
 *          ...
 *     };
 *
 * StuffManager mgr;
 * mgr.addItem(10, 0.5);
 * ...
 */;

namespace grynca {

    template <typename T>
    class ManagedItem {
    protected:
        typedef T ManagerType;
        template<typename TT> friend class Manager;

        uint32_t id_;
        ManagerType* manager_;
    public:
        uint32_t getId() { return id_; }
        ManagerType& getManager() { return *manager_; }
    };

    template <typename T>
    class ManagedItemVersioned {
    protected:
        typedef T ManagerType;
        template<typename TT> friend class ManagerVersioned;

        VersionedIndex id_;
        ManagerType* manager_;
    public:
        VersionedIndex getId() { return id_; }
        ManagerType& getManager() { return *manager_; }
    };

    template <typename ItemType>
    class Manager {
    public:
        ItemType& addItem();    // adds default-constructed item to manager
        ItemType& getItem(uint32_t id);
        ItemType& getItemAtPos(uint32_t pos);
        void removeItem(uint32_t id);
        void reserveSpaceForItems(size_t count);

        uint32_t getItemsCount();
        bool empty();
    private:
        UnsortedVector<ItemType> items_;
    };

    template <typename ItemType>
    class ManagerVersioned {
    public:
        ItemType& addItem();  // adds default-constructed item to manager
        ItemType& getItem(VersionedIndex id);
        ItemType& getItemAtPos(uint32_t pos);
        void removeItem(VersionedIndex id);
        void reserveSpaceForItems(size_t count);

        bool isValidIndex(VersionedIndex index);

        uint32_t getItemsCount();
        bool empty();
    private:
        UnsortedVersionedVector<ItemType> items_;
    };
}

#include "Manager.inl"
#endif //MANAGER_H
