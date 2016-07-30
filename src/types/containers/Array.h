#ifndef ARRAY_H
#define ARRAY_H

#include "Pool.h"
#include "TightPool.h"

namespace grynca {

    // adds data to back in O(1)
    // removing data also in O(1),
    //  - Array has holes inside
    //  - TightArray has no holes (memmoves last item to each hole after remove) and changes order
    //  - Indices are allways valid (uses reindexing internally_

    template <typename T, typename PoolType = Pool>
    class Array {
    public:
        Array();
        ~Array();

        // returns added item index
        template <typename ...ConstructionArgs>
        Index add(ConstructionArgs&&... args);

        void remove(Index index);
        void removeAtPos(uint32_t pos);
        void reserve(size_t count);

        T& get(Index index);
        T* getAtPos(uint32_t pos);      // good for looping through 0-size
        Index getIndexForPos(uint32_t pos);      // get index for data at pos

        const T& get(Index index)const;
        const T* getAtPos(uint32_t pos)const;

        bool isValidIndex(Index index)const;

        T* getData();

        uint32_t size()const;
        bool empty()const;

        PoolType& getPool() { return pool_; }
    private:
        PoolType pool_;
    };

    template <typename T>
    class TightArray: public Array<T, TightPool> {};
}

#include "Array.inl"
#endif // ARRAY_H
