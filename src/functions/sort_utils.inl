#include "sort_utils.h"
#include "../types/Index.h"
#include <algorithm>

namespace grynca {

    template <class RandomAccessIterator, class Compare>
    inline void indirectSort(RandomAccessIterator first, RandomAccessIterator last,
                             fast_vector<unsigned int>& order_out, Compare comp) {
        order_out.resize(last-first);
        for (size_t i=0; i<order_out.size(); ++i) {
            order_out[i] = i;
        }

        std::sort(order_out.begin(), order_out.end(),
                  [&first, &comp](const unsigned int& i1, const unsigned int& i2) {
                      return comp(*(first+i1), *(first+i2));
                  });
    }

    template <typename Container, typename ItemType, typename LessThan, typename EqualTo>
    inline uint32_t bisectFind(Container c, ItemType i, LessThan lt, EqualTo eq) {
        int left = 0;
        int right = c.size()-1;
        while (left <= right) {
            int mid  = (left + right)>>1;
            if (eq(i,c[mid])) {
                // found
                return (uint32_t)mid;
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

    template <typename Container, typename ItemType, typename LessThan, typename EqualTo>
    inline uint32_t bisectFindInsert(Container c, ItemType i, LessThan lt, EqualTo eq) {
        int left = 0;
        int right = c.size()-1;
        while (left <= right) {
            int mid  = (left + right)>>1;
            if (eq(i,c[mid])) {
                // found
                return (uint32_t)mid;
            }
            else if (lt(i, c[mid])) {
                right = mid-1;
            }
            else {
                left = mid+1;
            }
        }
        // not found
        return (uint32_t)left;
    }
}