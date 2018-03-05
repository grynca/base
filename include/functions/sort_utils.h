//
// Created by lutza on 15.7.2015.
//

#ifndef SORT_UTILS_H
#define SORT_UTILS_H

#include "../types/containers/fast_vector.h"

namespace grynca {

    // sort pointers
    // comp(f,s) returns true if f should be before s and accepts pointers to ItemType
    template <class RandomAccessIterator, class ItemType, class ComparePointers>
    static void indirectSort(RandomAccessIterator first, RandomAccessIterator last,
                             fast_vector<ItemType*>& pointers_out, const ComparePointers& comp);

    // find in sorted collection
    // returns InvalidId() when not found
    template <typename Container, typename ItemType, typename LessThan = std::less<ItemType>, typename EqualTo = std::equal_to<ItemType>>
    static u32 bisectFind(const Container& c, const ItemType& i, LessThan lt = LessThan(), EqualTo eq = EqualTo());

    // returns pos where to add new item
    template <typename Container, typename ItemType, typename LessThan = std::less<ItemType>>
    static u32 bisectFindInsert(const Container& c, const ItemType& i, LessThan lt = LessThan());


    template <typename ValueType, typename LessThan = std::less<ValueType>>
    static void sortThree(ValueType& a, ValueType& b, ValueType& c, ValueType** order_out, LessThan lt);

    template <typename ValueType, typename LessThan = std::less<ValueType>>
    static void sortFour(ValueType& a, ValueType& b, ValueType& c, ValueType& d, ValueType** order_out, LessThan lt = LessThan());
}


#include "sort_utils.inl"
#endif //SORT_UTILS_H
