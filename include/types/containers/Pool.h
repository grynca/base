#ifndef POOL_H
#define POOL_H

#include "fast_vector.h"
#include "../Index.h"
#include "../Type.h"

namespace grynca {

    // has holes inside from removed items
    // Index.getIndex() == position in memory

    class Pool {
    public:

        Pool(u32 item_size);

        Index add(u8*& new_item_out);

        void removeAtPos(u32 pos);
        void removeAtPos(u32 pos, DestroyFunc destructor);
        void remove(Index index);
        void remove(Index index, DestroyFunc destructor);
        void reserve(size_t count);

        u32 getItemPos(const u8* item)const;       // item must be in pool
        u32 getItemPos(Index index)const;
        u8* get(Index index);
        const u8* get(Index index)const;

        // can return NULL if is hole
        u8* getAtPos(u32 pos);
        const u8* getAtPos(u32 pos)const;

        // if you are sure that pos is not hole use this
        u8* getAtPos2(u32 pos);
        const u8* getAtPos2(u32 pos)const;


        Index getIndexForPos(u32 pos);      // get index for data at pos
        void getIndexForPos2(u32 pos, Index& index_out);

        bool isValidIndex(Index index)const;

        u8* getData();

        u32 size()const;
        u32 calcFreeSlotsCount()const;
        u32 calcOccupiedSize()const;
        bool empty()const;
        void clear();
        void clear(DestroyFunc destructor);
        f32 calcMemoryWaste();
        u32 getItemSize()const { return item_size_; }
    private:
        bool isFree_(u32 pos)const;
        void setFree_(u32 pos);
        void unsetFree_(u32 pos);
        u32 getNextFree_(u32 pos)const;
        void setNextFree_(u32 pos, u32 next_free_pos);

        u32 item_size_;
        u32 first_free_pos_;

        fast_vector<u16> versions_;        // first bit is 1 when slot is free
        fast_vector<u8> data_;
    };
}

#include "Pool.inl"
#endif //POOL_H
