#ifndef POOL_H
#define POOL_H

#include <stdint.h>
#include "fast_vector.h"
#include "../Index.h"
#include "../Type.h"

namespace grynca {

    // has holes inside from removed items

    class Pool {
    public:
        Pool(uint32_t item_size);

        Index add(uint8_t*& new_item_out);

        void removeAtPos(uint32_t pos);
        void removeAtPos(uint32_t pos, DestroyFunc destructor);
        void remove(Index index);
        void remove(Index index, DestroyFunc destructor);
        void reserve(size_t count);

        uint8_t* get(Index index);
        const uint8_t* get(Index index)const;
        uint8_t* getAtPos(uint32_t pos);        // can return NULL if is hole
        const uint8_t* getAtPos(uint32_t pos)const; // can return NULL if is hole
        Index getIndexForPos(uint32_t pos);      // get index for data at pos
        void getIndexForPos2(uint32_t pos, Index& index_out);

        bool isValidIndex(Index index)const;

        uint8_t* getData();

        uint32_t size()const;
        uint32_t occupiedSize()const;
        bool empty()const;
        void clear();
        void clear(DestroyFunc destructor);
        float getMemoryWaste();
        uint32_t getItemSize()const { return item_size_; }
    private:
        bool isFree_(uint32_t pos)const;
        void setFree_(uint32_t pos);
        void unsetFree_(uint32_t pos);

        uint32_t item_size_;

        fast_vector<uint16_t> versions_;        // first bit is 1 when slot is free
        fast_vector<uint8_t> data_;
        fast_vector<uint32_t> free_slots_;
    };
}

#include "Pool.inl"
#endif //POOL_H
