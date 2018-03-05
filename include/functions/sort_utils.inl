#include "sort_utils.h"
#include "../types/Index.h"
#include <algorithm>

namespace grynca {

    template <class RandomAccessIterator, class ItemType, class ComparePointers>
    inline void indirectSort(RandomAccessIterator first, RandomAccessIterator last,
                             fast_vector<ItemType*>& pointers_out, const ComparePointers& comp) {
        // static
        pointers_out.resize(last-first);
        for (size_t i=0; i<pointers_out.size(); ++i) {
            pointers_out[i] = first+i;
        }
        std::sort(pointers_out.begin(), pointers_out.end(), comp);
    }

    template <typename Container, typename ItemType, typename LessThan, typename EqualTo>
    inline u32 bisectFind(const Container& c, const ItemType& i, LessThan lt, EqualTo eq) {
        // static
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
        // static
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

    template <typename ValueType, typename LessThan>
    inline void sortThree(ValueType& a, ValueType& b, ValueType& c, ValueType** order_out, LessThan lt) {
        // static
        if(lt(b, a)) {
            order_out[0] = &b;
            order_out[1] = &a;
        }
        else {
            order_out[0] = &a;
            order_out[1] = &b;
        }

        if(lt(c, *order_out[0])) {
            order_out[2] = order_out[0];
            order_out[0] = &c;
        }
        else {
            order_out[2] = &c;
        }

        if(lt(*order_out[2], *order_out[1])) {
            std::swap(order_out[1], order_out[2]);
        }
    };

    template <typename ValueType, typename LessThan>
    inline void sortFour(ValueType& a, ValueType& b, ValueType& c, ValueType& d, ValueType** order_out, LessThan lt) {
        // static
        ValueType *low1, *high1, *low2, *high2;

        if (lt(a, b)) {
            low1 = &a;
            high1 = &b;
        }
        else {
            low1 = &b;
            high1 = &a;
        }

        if (lt(c, d)) {
            low2 = &c;
            high2 = &d;
        }
        else {
            low2 = &d;
            high2 = &c;
        }

        if (lt(*low1, *low2)) {
            order_out[0] = low1;
            order_out[1] = low2;
        }
        else {
            order_out[0] = low2;
            order_out[1] = low1;
        }

        if (lt(*high1, *high2)) {
            order_out[2] = high1;
            order_out[3] = high2;
        }
        else {
            order_out[2] = high2;
            order_out[3] = high1;
        }

        if (lt(*order_out[2], *order_out[1])) {
            std::swap(order_out[2], order_out[1]);
        }
    };
}