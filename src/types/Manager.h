#ifndef MANAGER_H
#define MANAGER_H

#include "containers/UnsortedVector.h"
#include "containers/UnsortedVersionedVector.h"
#include "RefCount.h"

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
 * MyStuff::Ref ref = mgr.addItem(10, 0.5).getRef();        // once first ref is taken item will be destroyed when all refs die
 *
 * ...
 */;

namespace grynca {

    // fw
    template <typename IT> class ManagedItemRef;

    template <typename T>
    class ManagedItem {
    public:
        typedef T ManagerType;
        typedef uint32_t IndexType;
    protected:
        template<typename TT> friend class Manager;
        template<typename TT> friend class ManagedItemRef;
        template<typename TT, typename TTT> friend class ManagerSingletons;

        IndexType id_;
        ManagerType* manager_;
        RefCount ref_count_;
    public:
        ManagedItem();

        int getRefCount() { return ref_count_.get(); }
        IndexType getId()const { return id_; }
        ManagerType& getManager()const { return *manager_; }
    };

    template <typename T>
    class ManagedItemVersioned {
    public:
        typedef T ManagerType;
        typedef VersionedIndex IndexType;
    protected:
        template<typename TT> friend class ManagedItemRef;
        template<typename TT> friend class ManagerVersioned;

        IndexType id_;
        ManagerType* manager_;
        RefCount ref_count_;
    public:
        ManagedItemVersioned();

        int getRefCount() { return ref_count_.get(); }
        IndexType getId()const { return id_; }
        ManagerType& getManager()const { return *manager_; }
    };

    template <typename T>
    class Manager {
    public:
        typedef T ItemType;
        typedef uint32_t IndexType;
        typedef ManagedItemRef<ItemType> ItemRef;

        template <typename...ConstructionArgs>
        ItemType& addItem(ConstructionArgs&&... args);
        ItemType& getItem(IndexType id);
        const ItemType& getItem(IndexType id)const;
        ItemType& getItemAtPos(IndexType pos);
        const ItemType& getItemAtPos(IndexType pos)const;
        void removeItem(IndexType id);
        void reserveSpaceForItems(size_t count);

        bool isValidIndex(IndexType index)const;

        uint32_t getItemsCount()const;
        bool empty();
    private:
        UnsortedVector<ItemType> items_;                // items are in-place -> volatile
    };

    template <typename T>
    class ManagerVersioned {
    public:
        typedef T ItemType;
        typedef VersionedIndex IndexType;
        typedef ManagedItemRef<ItemType> ItemRef;

        template <typename...ConstructionArgs>
        ItemType& addItem(ConstructionArgs&&... args);
        ItemType& getItem(IndexType id);
        const ItemType& getItem(IndexType id)const;
        ItemType& getItemAtPos(uint32_t pos);
        const ItemType& getItemAtPos(uint32_t pos)const;
        void removeItem(IndexType id);
        void reserveSpaceForItems(size_t count);

        bool isValidIndex(IndexType index)const;

        uint32_t getItemsCount()const;
        bool empty();
    private:
        UnsortedVersionedVector<ItemType> items_;       // items are in-place -> volatile
    };

    // manages singletons derived from BaseType
    template <typename Derived, typename BaseType>
    class ManagerSingletons {
    public:
        typedef BaseType ItemType;
        typedef uint32_t IndexType;

        virtual ~ManagerSingletons();

        template <typename T>
        T& get();


        BaseType* getById(uint32_t id);
        const BaseType* getById(uint32_t id)const;

        uint32_t getSize();
    private:
        fast_vector<BaseType*> items_;      // items are pointers -> non-volatile
    };

    template <typename ItemType>
    class ManagedItemRef {
        typedef typename ItemType::ManagerType ManagerType;
        typedef typename ItemType::IndexType IndexType;
    public:
        ManagedItemRef();
        ManagedItemRef(ManagerType& manager, IndexType index);
        ManagedItemRef(const ManagedItemRef<ItemType>& ref);
        ManagedItemRef(ItemType& item);

        ManagedItemRef<ItemType>& operator =(const ManagedItemRef<ItemType>& ref);

        ~ManagedItemRef();

        ItemType& get()const;

    protected:
        void unref_();

        typename ItemType::ManagerType* manager_;       // manager must be non-volatile (e.g. not in another manager)
        typename ItemType::IndexType id_;
    };
}

#include "Manager.inl"
#endif //MANAGER_H
