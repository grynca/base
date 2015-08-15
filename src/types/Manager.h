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
        template<typename TT, typename TTT> friend class ManagerSingletons;

        uint32_t id_;
        ManagerType* manager_;
    public:
        uint32_t getId()const { return id_; }
        ManagerType& getManager()const { return *manager_; }
    };

    template <typename T>
    class ManagedItemVersioned {
    protected:
        typedef T ManagerType;
        template<typename TT> friend class ManagerVersioned;

        VersionedIndex id_;
        ManagerType* manager_;
    public:
        VersionedIndex getId()const { return id_; }
        ManagerType& getManager()const { return *manager_; }
    };

    template <typename ItemType>
    class Manager {
    public:
        template <typename...ConstructionArgs>
        ItemType& addItem(ConstructionArgs&&... args);
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
        template <typename...ConstructionArgs>
        ItemType& addItem(ConstructionArgs&&... args);
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

    // manages singletons derived from ItemType
    template <typename Derived, typename ItemType>
    class ManagerSingletons {
    public:
        virtual ~ManagerSingletons();

        template <typename T>
        T& get();


        ItemType* getById(uint32_t id);

        uint32_t getSize();
    private:
        fast_vector<ItemType*> items_;
    };
}

#include "Manager.inl"
#endif //MANAGER_H
