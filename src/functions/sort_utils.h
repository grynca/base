//
// Created by lutza on 15.7.2015.
//

#ifndef SORT_UTILS_H
#define SORT_UTILS_H

#include "../types/containers/fast_vector.h"

namespace grynca {

    // sort to indices vector
    // first will be indexed as 0 in order_out
    template <class RandomAccessIterator, class Compare>
    static void indirectSort(RandomAccessIterator first, RandomAccessIterator last,
                             fast_vector<unsigned int>& order_out, Compare comp);

}


#include "sort_utils.inl"
#endif //SORT_UTILS_H
