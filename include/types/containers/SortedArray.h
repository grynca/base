#ifndef SORTEDARRAY_H
#define SORTEDARRAY_H

#include "fast_vector.h"

namespace grynca {

    // fw
    template <typename CT, typename IT> class ItemPtr;
    template <typename CT, typename IT> class ItemAutoPtr;
    template <typename CT, typename IT> class ItemRefPtr;

    // ItemType class must implement these, accessible to SortedArray:
    /*
     *  u32& accId_() { ... }
     *  struct Comparator {
     *      // returns if i1 is before i2
            bool operator()(const ItemT& i1, const ItemT& i2) { ... }
        };
     */


    // slow add and remove, memmoves items in memory in order to maintain sorting
    template <typename T>
    class SortedArray {
        typedef SortedArray<T> MyType;
    public:
        typedef T ItemType;
        typedef u32 ItemIndexType;
        typedef ItemPtr<MyType, ItemType> IPtr;
        typedef ItemAutoPtr<MyType, ItemType> IAutoPtr;
        typedef ItemRefPtr<MyType, ItemType> IRefPtr;

        SortedArray();

        void reserve(u32 count);

        ItemType& addLast(const ItemType& item);        // use when you know item should be at end
        ItemType& add(const ItemType& item);
        void removeItem(u32 item_id);
        const ItemType& getItem(u32 item_id)const;
        const ItemType* tryGetItem(u32 item_id)const;
        ItemType& accItem(u32 item_id);
        ItemType* tryAccItem(u32 item_id);
        IRefPtr getRef(u32 item_id);

        bool containsItem(u32 item_id)const;

        const ItemType& getItemAtPos(u32 pos)const;
        ItemType& accItemAtPos(u32 pos);
        u32 getItemPos(u32 item_id)const;
        u32 size()const;
        bool empty()const;
        void clear();

        // void LoopFunc(ItemType& item)
        template <typename LoopFunc>
        void loopItems(const LoopFunc& lf)const;

        // bool LoopFunc(ItemType& item), returns true if should break
        template <typename LoopFunc>
        void loopItemsBreakable(const LoopFunc& lf)const;
    private:
        bool isFree_(u32 id)const;
        u32 getNextId_();

        fast_vector<ItemType> items_;
        fast_vector<u32> id_to_pos_;
        u32 first_free_id_;
    };

}

#include "SortedArray.inl"
#endif //SORTEDARRAY_H
