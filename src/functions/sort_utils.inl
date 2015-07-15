#include "sort_utils.h"
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
}