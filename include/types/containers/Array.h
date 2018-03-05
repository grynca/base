#ifndef ARRAY_H
#define ARRAY_H

#include "Pool.h"
#include "TightPool.h"

namespace grynca {

    // fw
    template <typename CT, typename IT> class ItemPtr;
    template <typename CT, typename IT> class ItemAutoPtr;
    template <typename CT, typename IT> class ItemRefPtr;

    // adds data to back in O(1)
    // removing data also in O(1),
    //  - Array has holes inside
    //  - TightArray has no holes (memmoves last item to each hole after remove) and changes order
    //  - Indices are allways valid (uses reindexing internally)

    template <typename T, typename PoolType = Pool>
    class Array {
        typedef Array<T, PoolType> MyType;
    public:
        typedef T ItemType;
        typedef Index ItemIndexType;
        typedef ItemPtr<MyType, ItemType> IPtr;
        typedef ItemAutoPtr<MyType, ItemType> IAutoPtr;
        typedef ItemRefPtr<MyType, ItemType> IRefPtr;

        Array();
        ~Array();

        // returns added item index
        template <typename ...ConstructionArgs>
        Index add(ConstructionArgs&&... args);

        // returns added item
        template <typename ...ConstructionArgs>
        T& add2(Index& index_out, ConstructionArgs&&... args);

        // item must derive from RefCounted
        template <typename ...ConstructionArgs>
        IRefPtr addRC(ConstructionArgs&&... args);

        void removeItem(Index index);
        void removeItemAtPos(u32 pos);
        void reserve(size_t count);

        u32 getItemPos(const T* item)const;
        u32 getItemPos(Index index)const;
        T& accItem(Index index);
        T* accItemAtPos(u32 pos);      // good for looping through 0-size
        T& accItemAtPos2(u32 pos);      // (use when you are sure that pos is not hole)
        Index getIndexForPos(u32 pos)const;      // get index for data at pos
        Index getFullIndex(u32 index_id)const;      // if you only have Index.index this will get you corresponding version&aux

        // if you only have index part of Index
        T& accItemWithInnerIndex(u32 inner_id);
        const T& getItemWithInnerIndex(u32 inner_id)const;

        const T& getItem(Index index)const;
        const T* getItemAtPos(u32 pos)const;

        bool isValidIndex(Index index)const;

        T* getData();

        void clear();

        u32 size()const;
        bool empty()const;

        PoolType& accPool() { return pool_; }
        const PoolType& getPool()const { return pool_; }
    private:
        PoolType pool_;
    };

    template <typename T>
    class TightArray: public Array<T, TightPool> {};
}

#include "Array.inl"
#endif // ARRAY_H
