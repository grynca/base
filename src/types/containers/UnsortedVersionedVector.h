#ifndef UNSORTEDVECTORVERSIONED_H
#define UNSORTEDVECTORVERSIONED_H

#include <list>
#include <stdint.h>
#include "fast_vector.h"
#include "../VersionedIndex.h"

namespace grynca {

    // same as UnsortedVector but with unique versioned indices
    //  (indices can be checked for validity)

    template <typename T>
    class UnsortedVersionedVector {
    public:
        ~UnsortedVersionedVector();

        template <typename ...ConstructionArgs>
        VersionedIndex add(ConstructionArgs&&... args);

        void remove(VersionedIndex index);
        void reserve(size_t count);

        T& get(VersionedIndex index);
        T& getAtPos(uint32_t pos);      // good for looping through 0-size

        // can check if index is valid (current and not already removed)
        bool isValidIndex(VersionedIndex index);

        T* getData();
        uint32_t size()const;
        bool empty();
    private:
        fast_vector<uint8_t> items_data_;
        // for each item index of its redirect
        fast_vector<uint32_t> redirect_ids_;
        fast_vector<VersionedIndex> redirects_;
        fast_vector<uint32_t> free_redirects_;
    };

}

#include "UnsortedVersionedVector.inl"
#endif // UNSORTEDVECTORVERSIONED_H
