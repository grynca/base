#ifndef TIGHTPOOL_H
#define TIGHTPOOL_H
#include "fast_vector.h"
#include "../Type.h"
#include "../Index.h"
#include <stdint.h>


namespace grynca {

    // does not have holes (moves items to fill them), uses redirect indices internally

    class TightPool {
    public:
        TightPool(uint32_t item_size);

        Index add(uint8_t*& new_item_out);

        void remove(Index index);
        void remove(Index index, DestroyFunc destructor);
        void reserve(size_t count);

        uint8_t* get(Index index);
        uint8_t* getAtPos(uint32_t pos);      // good for looping through
        Index getIndexForPos(uint32_t pos);      // get index for data at pos
        void getIndexForPos2(uint32_t pos, Index& index_out);

        const uint8_t* get(Index index)const;
        const uint8_t* getAtPos(uint32_t pos)const;

        bool isValidIndex(Index index)const;

        uint8_t* getData();

        uint32_t size()const;
        bool empty()const;
        void clear();
        void clear(DestroyFunc destructor);
        uint32_t getItemSize()const { return item_size_; }
    protected:
        uint32_t item_size_;
        fast_vector<uint8_t> items_data_;
        // for each item index of its redirect (needed for removal)
        fast_vector<uint32_t> redirect_ids_;

        fast_vector<Index> redirects_;
        fast_vector<uint32_t> free_redirects_;
    };

}

#include "TightPool.inl"
#endif //TIGHTPOOL_H
