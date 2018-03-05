#ifndef COMMON_H
#define COMMON_H

#include "meta.h"
#include "defs.h"
#include <algorithm>


namespace grynca {

    // "Next Largest Power of 2
    // Given a binary integer value x, the next largest power of 2 can be computed by a SWAR algorithm
    // that recursively "folds" the upper bits into the lower bits. This process yields a bit vector with
    // the same most significant 1 as x, but all 1's below it. Adding 1 to that value yields the next
    // largest power of 2. For a 32-bit value:"
    inline constexpr u32 calcNextPowerOfTwo(u32 x) {
        x |= (x >> 1);
        x |= (x >> 2);
        x |= (x >> 4);
        x |= (x >> 8);
        x |= (x >> 16);
        return x+1;
    }

    inline constexpr u32 calcPrevPowerOfTwo(uint32_t x) {
        x = x | (x >> 1);
        x = x | (x >> 2);
        x = x | (x >> 4);
        x = x | (x >> 8);
        x = x | (x >> 16);
        return x - (x >> 1);
    }

    // e.g for finding out number of bits needed for storing number
    constexpr u32 floorLog2(u32 x) {
        return x < 2 ? x : 1+floorLog2(x >> 1);
    }

    template <typename T>
    inline T clampToRange(const T& val, const T& min, const T& max) {
        return std::max(std::min(val, max), min);
    }

    template <typename T>
    inline int sign(T v) {
        return (v > 0) - (v < 0);
    }

    // wraps index to array (works with negative indices)
    //  (when arr_size is power of two use PMOD macro instead)
    inline u32 wrap(i32 index, i32 arr_size) {
        return (u32)((index % arr_size) + arr_size) % arr_size;
    }

    // the number of 1 bits in a number.
    inline u32 popcount (u32 val) {
        return __builtin_popcount(val);
    }

    inline u32 popcount (u64 val) {
        return __builtin_popcountll(val);
    }

    // count trailing zeros
    inline u32 ctz(u32 w) {
        return __builtin_ctz(w);
    }

    inline u32 ctz(u64 w) {
        return __builtin_ctzll(w);
    }

    // count leading zeros
    inline u32 clz(u32 w) {
        return __builtin_clz(w);
    }

    inline u32 clz(u64 w) {
        return __builtin_clzll(w);
    }

    template <typename Vec>
    inline bool isPrefix(const Vec& prefix, const Vec& c) {
        size_t plen = prefix.size();
        if (plen > c.size())
            return false;
        return std::equal(prefix.begin(), prefix.begin()+plen, c.begin());
    }

    template <typename Vec>
    inline bool isSmallerPrefix(const Vec& prefix, const Vec& c) {
        size_t plen = prefix.size();
        if (plen >= c.size())
            return false;
        return std::equal(prefix.begin(), prefix.begin()+plen, c.begin());
    }

    template <typename Map, typename Key, typename Val>
    inline Val tryGet(const Map& map, const Key& key, const Val& def_value) {
        auto item_ptr = map.findItem(key);
        if(item_ptr)
            return *item_ptr;
        return def_value;
    };

    template <typename FuctType>
    inline void dispatchFunction(void* fp) {
        (*(FuctType*)fp)();
    }

    template <typename T>
    inline T lerp(f32 t, const T& a, const T& b) {
        return (1-t)*a + t*b;
    }

    // same as with vector, shifts elements after removed to fill the gap
    //  caller must update its size variable
    template <typename T>
    inline void arrayErase(T* arr, u32 pos, u32 count, u32 size) {
        ASSERT((pos+count) <= size);
        memmove(arr + pos, arr + pos + count, (size - pos - count) * sizeof(T));
    }

    // creates gap for new items
    template <typename T>
    inline void arrayInsert(T* arr, u32 pos, u32 count, u32 size, u32 max_size) {
        UNUSED(max_size);
        ASSERT((size+count) <= max_size);
        memmove(arr + pos + count, arr + pos, (size - pos) * sizeof(T));
    }
}

#endif //COMMON_H
