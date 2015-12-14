#ifndef UNSORTEDVECTOR_H
#define UNSORTEDVECTOR_H

#include <list>
#include <stdint.h>
#include <cassert>
#include "fast_vector.h"

namespace grynca {

    // adds data to back in O(1)
    // removing data also in O(1), (last item is memmoved to created hole)
    //   therefore items are unsorted
    //   and in continuous block of memory without holes (can be looped for fast cache-friendly processing)
    // indices are not invalidated by adding&removing (uses redirect indices)
    // reuses previously freed indices
    template <typename T>
    class UnsortedVector {
    public:
        ~UnsortedVector();

        // returns added item index
        template <typename ...ConstructionArgs>
        uint32_t add(ConstructionArgs&&... args);

        void remove(uint32_t index);
        void reserve(size_t count);

        T& get(uint32_t index);
        T& getAtPos(uint32_t pos);      // good for looping through 0-size
        uint32_t getIndexForPos(uint32_t pos);      // get index for data at pos

        const T& get(uint32_t index)const;
        const T& getAtPos(uint32_t pos)const;

        bool isValidIndex(uint32_t index)const;

        T* getData();

        uint32_t size()const;
        bool empty()const;
    protected:
        fast_vector<uint8_t> items_data_;
        // for each item index of its redirect (needed for removal)
        fast_vector<uint32_t> redirect_ids_;

        fast_vector<uint32_t> redirects_;
        fast_vector<uint32_t> free_redirects_;
    };

}

#include "UnsortedVector.inl"
#endif // UNSORTEDVECTOR_H
