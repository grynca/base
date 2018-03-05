#ifndef TIGHTPOOL_H
#define TIGHTPOOL_H
#include "fast_vector.h"
#include "../Type.h"
#include "../Index.h"


namespace grynca {

    // does not have holes (moves items to fill them), uses redirect indices internally

    class TightPool {
    public:
        TightPool(u32 item_size);

        Index add(u8*& new_item_out);
        Index add(u8*& new_item_out, u16 aux_id);

        void removeItem(Index index);
        void removeItem(Index index, DestroyFunc destructor);
        void reserve(size_t count);

        u32 getItemPos(const u8* item)const;       // item must be in pool
        u32 getItemPos(Index index)const;
        u8* accItem(Index index);
        const u8* getItem(Index index)const;

        // if you only have index part of Index
        u8* accItemWithInnerIndex(u32 inner_id);
        const u8* getItemWithInnerIndex(u32 inner_id)const;

        Index getIndexForPos(u32 pos)const;      // get index for data at pos
        u32 getInnerIndexForPos(u32 pos)const;   // gets only .index part
        void getIndexForPos2(u32 pos, Index& index_out)const;
        Index getFullIndex(u32 index_id)const;      // if you only have Index.index this will get you corresponding version&aux

        // no need to check for NULL (cant be hole)
        u8* accItemAtPos(u32 pos);
        const u8* getItemAtPos(u32 pos)const;

        // just for consistent API with Pool (same as getAtPos)
        u8* accItemAtPos2(u32 pos);
        const u8* getItemAtPos2(u32 pos)const;

        bool isValidIndex(Index index)const;

        u8* getData();

        u32 size()const;
        bool empty()const;
        void clear();
        void clear(DestroyFunc destructor);
        u32 getItemSize()const { return item_size_; }
    protected:
        bool isFree_(u32 pos)const;
        void setFree_(u32 pos);
        void unsetFree_(u32 pos);
        u32 takeNextFree_(u32 pos);
        void setNextFree_(u32 pos, u32 next_free_pos);

        u32 item_size_;
        u32 first_free_pos_;
        fast_vector<u8> data_;
        // for each item index of its redirect (needed for removal)
        fast_vector<u32> redirect_ids_;

        fast_vector<Index> redirects_;
    };

}

#include "TightPool.inl"
#endif //TIGHTPOOL_H
