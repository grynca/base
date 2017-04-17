#ifndef MANAGER_H
#define MANAGER_H

#include "containers/Array.h"
#include "RefCount.h"

/*
 * EXAMPLE:
 * class StuffManager;
 * class MyStuff : public ManagedItem<StuffManager>
 *     {
 *     public:
 *          void init(int a, f64 b);
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
    template <typename IT> class ManagedItemPtr;

    template <typename T, typename IT = Index>
    class ManagedItem {
    public:
        void init() {}

        typedef T ManagerType;
        typedef IT IndexType;
    protected:
        template<typename TT, typename TTT> friend class Manager;
        template<typename TT> friend class ManagedItemRef;
        template<typename TT> friend class ManagedItemPtr;

        template<typename TT, typename TTT> friend class ManagerSingletons;

        IndexType id_;
        ManagerType* manager_;
        RefCount ref_count_;
    public:
        ManagedItem()
         : manager_(NULL), ref_count_(0) {}

        int getRefCount() { return ref_count_.get(); }
        void ref() { ref_count_.ref(); }
        bool unref() { return ref_count_.unref(); }
        IndexType getId()const { return id_; }
        ManagerType& getManager()const { return *manager_; }
    };

    template <typename T>
    class ManagedItemSingleton : public ManagedItem<T, u32> {
    };

    // do not store pointers or references to items
    //  only indices or ManagedItemRef

    template <typename T, class ArrayType = Array<T> >
    class Manager {
    public:
        typedef T ItemType;
        typedef Index IndexType;
        typedef ManagedItemPtr<ItemType> ItemPtr;
        typedef ManagedItemRef<ItemType> ItemRef;

        template <typename...ConstructionArgs>
        ItemType& addItem(ConstructionArgs&&... args);
        ItemType& getItem(IndexType id);
        const ItemType& getItem(IndexType id)const;

        ItemType* getItemAtPos(u32 pos);
        const ItemType* getItemAtPos(u32 pos)const;

        //use when you are sure that pos is not hole
        ItemType& getItemAtPos2(u32 pos);
        const ItemType& getItemAtPos2(u32 pos)const;
        Index getIndexForPos(u32 pos);

        u32 getItemPos(Index index)const;
        void removeItem(IndexType id);
        void reserveSpaceForItems(size_t count);

        bool isValidIndex(IndexType index)const;

        u32 getItemsCount()const;
        bool empty();
    private:
        ArrayType items_;
    };

    template <typename ItemType>
    class TightManager : public Manager<ItemType, TightArray<ItemType> > {};


    // manages singletons derived from BaseType
    template <typename Derived, typename BaseType>
    class ManagerSingletons {
    public:
        typedef BaseType ItemType;
        typedef u32 IndexType;

        virtual ~ManagerSingletons();

        template <typename T>
        T& get();

        BaseType* getById(IndexType id);
        const BaseType* getById(IndexType id)const;

        const TypeInfo& getTypeInfo(IndexType id)const;

        u32 getSize();
    private:
        fast_vector<BaseType*> items_;      // items are pointers -> non-volatile
    };

    template <typename ItemType>
    class ManagedItemPtr {
    public:
        typedef typename ItemType::ManagerType ManagerType;
        typedef typename ItemType::IndexType IndexType;

        ManagedItemPtr();
        ManagedItemPtr(ManagerType& manager, IndexType index);
        ManagedItemPtr(const ManagedItemRef<ItemType>& ref);
        ManagedItemPtr(const ManagedItemPtr<ItemType>& ptr);
        ManagedItemPtr(ItemType& item);
        ManagedItemPtr(ItemType* item);

        ItemType& get()const;
        ItemType* getPtr()const;

        IndexType getItemId()const;
        ManagerType& getManager()const;

        bool isNull()const;
        bool isValid()const;

        ItemType* operator->() { return &get(); }
        const ItemType* operator->()const { return &get(); }
    protected:
        friend class ManagedItemRef<ItemType>;

        typename ItemType::ManagerType* manager_;       // manager must be non-volatile (e.g. not in another manager)
        typename ItemType::IndexType id_;
    };

    template <typename ItemType>
    class ManagedItemRef {
        typedef typename ItemType::ManagerType ManagerType;
        typedef typename ItemType::IndexType IndexType;
    public:
        ManagedItemRef();
        ManagedItemRef(ManagerType& manager, IndexType index);
        ManagedItemRef(const ManagedItemRef<ItemType>& ref);
        ManagedItemRef(const ManagedItemPtr<ItemType>& ptr);
        ManagedItemRef(ManagedItemRef<ItemType>&& ref);
        ManagedItemRef(ItemType& item);

        ManagedItemRef<ItemType>& operator =(const ManagedItemRef<ItemType>& ref);
        ManagedItemRef<ItemType>& operator =(ManagedItemRef<ItemType>&& ref);

        ~ManagedItemRef();

        ItemType& get()const;
        ItemType* getPtr()const;

        IndexType getItemId()const;
        ManagerType& getManager()const;

        bool isNull()const;
        bool isValid()const;

        ItemType* operator->() { return &get(); }
        const ItemType* operator->()const { return &get(); }
    protected:
        friend class ManagedItemPtr<ItemType>;

        void unref_();

        typename ItemType::ManagerType* manager_;       // manager must be non-volatile (e.g. not in another manager)
        typename ItemType::IndexType id_;
    };
}

#include "Manager.inl"
#endif //MANAGER_H
