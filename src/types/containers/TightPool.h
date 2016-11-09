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

        void remove(Index index);
        void remove(Index index, DestroyFunc destructor);
        void reserve(size_t count);

        u8* get(Index index);
        u8* getAtPos(u32 pos);      // good for looping through
        Index getIndexForPos(u32 pos);      // get index for data at pos
        void getIndexForPos2(u32 pos, Index& index_out);

        const u8* get(Index index)const;
        const u8* getAtPos(u32 pos)const;

        bool isValidIndex(Index index)const;

        u8* getData();

        u32 size()const;
        bool empty()const;
        void clear();
        void clear(DestroyFunc destructor);
        u32 getItemSize()const { return item_size_; }
    protected:
        u32 item_size_;
        fast_vector<u8> items_data_;
        // for each item index of its redirect (needed for removal)
        fast_vector<u32> redirect_ids_;

        fast_vector<Index> redirects_;
        fast_vector<u32> free_redirects_;
    };

}

#include "TightPool.inl"
#endif //TIGHTPOOL_H
