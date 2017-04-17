#include "sort_utils.h"
#include "../types/Index.h"
#include <algorithm>

namespace grynca {

    template <class RandomAccessIterator, class ItemType, class ComparePointers>
    inline void indirectSort(RandomAccessIterator first, RandomAccessIterator last,
                             fast_vector<ItemType*>& pointers_out, ComparePointers) {
        pointers_out.resize(last-first);
        for (size_t i=0; i<pointers_out.size(); ++i) {
            pointers_out[i] = first+i;
        }
        std::sort(pointers_out.begin(), pointers_out.end(), ComparePointers());
    }

    template <typename Container, typename ItemType, typename LessThan, typename EqualTo>
    inline u32 bisectFind(const Container& c, const ItemType& i, LessThan lt, EqualTo eq) {
        int left = 0;
        int right = c.size()-1;
        while (left <= right) {
            int mid  = (left + right)>>1;
            if (eq(i,c[mid])) {
                // found
                return (u32)mid;
            }
            else if (lt(i, c[mid])) {
                right = mid-1;
            }
            else {
                left = mid+1;
            }
        }
        // not found
        return InvalidId();
    }

    template <typename Container, typename ItemType, typename LessThan>
    inline u32 bisectFindInsert(const Container& c, const ItemType& i, LessThan lt) {
        int left = 0;
        int right = c.size()-1;
        while (left <= right) {
            int mid  = (left + right)>>1;
            if (lt(i, c[mid])) {
                right = mid-1;
            }
            else {
                left = mid+1;
            }
        }
        return (u32)left;
    }
}