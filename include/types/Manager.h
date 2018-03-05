#ifndef MANAGER_H
#define MANAGER_H

#include "containers/Array.h"
#include "containers/ItemPtr.h"
#include "containers/ItemRefPtr.h"

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

    template <typename ItemType>
    class ManagedItemPtr : public ItemPtr<typename ItemType::ManagerType, ItemType> {
        typedef typename ItemType::ManagerType ContainerType;
        typedef typename ContainerType::ItemIndexType IndexType;
        typedef ItemPtr<ContainerType, ItemType> Base;
    public:
        ManagedItemPtr();
        ManagedItemPtr(ContainerType& container, IndexType index);
        ManagedItemPtr(const ItemRefPtr<ContainerType, ItemType>& ref);
        ManagedItemPtr(const ItemPtr<ContainerType, ItemType>& ptr);

        ManagedItemPtr(const ItemType& item);
        ManagedItemPtr(const ItemType* item);
    };

    template <typename ItemType>
    class ManagedItemAutoPtr : public ItemAutoPtr<typename ItemType::ManagerType, ItemType> {
        typedef typename ItemType::ManagerType ContainerType;
        typedef typename ContainerType::ItemIndexType IndexType;
        typedef ItemAutoPtr<ContainerType, ItemType> Base;
    public:
        ManagedItemAutoPtr();
        ManagedItemAutoPtr(ContainerType& container, IndexType index);
        ManagedItemAutoPtr(const ItemRefPtr<ContainerType, ItemType>& ref);
        ManagedItemAutoPtr(const ItemPtr<ContainerType, ItemType>& ptr);
        ManagedItemAutoPtr(ItemAutoPtr<ContainerType, ItemType>&& ptr);

        ManagedItemAutoPtr(const ItemType& item);
        ManagedItemAutoPtr(const ItemType* item);
    };

    template <typename ItemType>
    class ManagedItemRefPtr : public ItemRefPtr<typename ItemType::ManagerType, ItemType> {
        typedef typename ItemType::ManagerType ContainerType;
        typedef typename ItemType::IndexType IndexType;
        typedef ItemRefPtr<ContainerType, ItemType> Base;
    public:
        ManagedItemRefPtr();
        ManagedItemRefPtr(ContainerType& container, IndexType index);
        ManagedItemRefPtr(const ItemRefPtr<ContainerType, ItemType>& ref);
        ManagedItemRefPtr(const ItemPtr<ContainerType, ItemType>& ptr);
        ManagedItemRefPtr(ItemRefPtr<ContainerType, ItemType>&& ref);

        ManagedItemRefPtr(const ItemType& item);
        ManagedItemRefPtr(const ItemType* item);
    };

    template <typename T, typename IT = Index>
    class ManagedItem : public RefCounted {
    public:
        typedef T ManagerType;
        typedef IT IndexType;

    protected:
        template<typename TT, typename TTT> friend class Manager;
        template<typename TT> friend class ManagerSingletons;

        // called by manager, override by subclass if needed
        void afterAdded() {}

        IndexType id_;
        ManagerType* manager_;
    public:
        ManagedItem() : manager_(NULL){}

        IndexType getId()const { return id_; }
        ManagerType& getManager()const { return *manager_; }
    };

    template <typename T>
    class ManagedItemSingleton : public ManagedItem<T, u32> {
    public:
        // called by manager, override by subclass if needed
        void initSingleton() {}
    };

    // do not store pointers or references to items
    //  only indices or ManagedItemRef

    template <typename T, class ArrayType = Array<T> >
    class Manager {
        typedef Manager<T,ArrayType> MyType;
    public:
        typedef T ItemType;
        typedef Index ItemIndexType;
        typedef ManagedItemPtr<ItemType> IPtr;
        typedef ManagedItemAutoPtr<ItemType> IAutoPtr;
        typedef ManagedItemRefPtr<ItemType> IRefPtr;

        template <typename...ConstructionArgs>
        ItemType& addItem(ConstructionArgs&&... args);
        ItemType& accItem(ItemIndexType id);
        const ItemType& getItem(ItemIndexType id)const;

        ItemType* accItemAtPos(u32 pos);
        const ItemType* getItemAtPos(u32 pos)const;

        //use when you are sure that pos is not hole
        ItemType& accItemAtPos2(u32 pos);
        const ItemType& getItemAtPos2(u32 pos)const;
        Index getIndexForPos(u32 pos);

        u32 getItemPos(Index index)const;
        void removeItem(ItemIndexType id);
        void reserveSpaceForItems(size_t count);

        bool isValidIndex(ItemIndexType index)const;

        void clear();

        u32 getItemsCount()const;
        bool empty();
    private:
        ArrayType items_;
    };

    template <typename ItemType>
    class TightManager : public Manager<ItemType, TightArray<ItemType> > {};

    // manages singletons derived from BaseType
    template <typename BaseType>
    class ManagerSingletons {
        typedef ManagerSingletons<BaseType> Domain;
    public:
        typedef BaseType ItemType;
        typedef u32 ItemIndexType;

        virtual ~ManagerSingletons();

        template <typename Types, typename... InitArgs>
        void initSingletons(InitArgs&&... args);

        template <typename T, typename... InitArgs>
        T& initSingleton(InitArgs&&... args);

        // creates item lazyly if needed (does not call init)
        template <typename T>
        T& get();

        // does not checks if item was created
        template <typename T>
        T& getFast();

        const BaseType* tryGetById(ItemIndexType id)const;
        BaseType* tryGetById(ItemIndexType id);

        BaseType* getById(ItemIndexType id);
        const BaseType* getById(ItemIndexType id)const;

        const TypeInfo& getTypeInfo(ItemIndexType id)const;

        template <typename T>
        static u32 getTypeIdOf();

        u32 getSize()const;
    private:
        struct TypesInitializer_ {
            template <typename TP, typename T, typename... InitArgs>
            static void f(ManagerSingletons& mgr, InitArgs&&... args);
        };

        struct ItemCtx {
            ItemCtx() : offset_to_base(0) {}

            CommonPtr ptr;
            i32 offset_to_base;
        };

        template <typename T>
        u32 addTypeIfNeeded_();

        fast_vector<ItemCtx> items_;      // items are pointers -> non-volatile
    };
}

#include "Manager.inl"
#endif //MANAGER_H
