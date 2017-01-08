//
// Created by lutza on 15.7.2015.
//

#ifndef SORT_UTILS_H
#define SORT_UTILS_H

#include "../types/containers/fast_vector.h"

namespace grynca {

    // sort to indices vector
    // first will be indexed as 0 in order_out
    // comp(f,s) returns true if f should be before s
    template <class RandomAccessIterator, class Compare>
    static void indirectSort(RandomAccessIterator first, RandomAccessIterator last,
                             fast_vector<unsigned int>& order_out, Compare comp);

    // find in sorted collection
    // returns InvalidId() when not found
    template <typename Container, typename ItemType, typename LessThan = std::less<ItemType>, typename EqualTo = std::equal_to<ItemType>>
    static u32 bisectFind(const Container& c, const ItemType& i, LessThan lt = LessThan(), EqualTo eq = EqualTo());

    // returns pos where to add new item
    template <typename Container, typename ItemType, typename LessThan = std::less<ItemType>, typename EqualTo = std::equal_to<ItemType>>
    static u32 bisectFindInsert(const Container& c, const ItemType& i, LessThan lt = LessThan(), EqualTo eq = EqualTo());
}


#include "sort_utils.inl"
#endif //SORT_UTILS_H
