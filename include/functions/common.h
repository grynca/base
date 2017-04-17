#ifndef COMMON_H
#define COMMON_H

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

    // Thomas Wang's hash
    inline u32 calcHash32(u32 key)
    {
        key += ~(key << 15);
        key ^=  (key >> 10);
        key +=  (key << 3);
        key ^=  (key >> 6);
        key += ~(key << 11);
        key ^=  (key >> 16);
        return key;
    }

    inline u32 calcHash64(u64 key)
    {
        key += ~(key << 32);
        key ^= (key >> 22);
        key += ~(key << 13);
        key ^= (key >> 8);
        key += (key << 3);
        key ^= (key >> 15);
        key += ~(key << 27);
        key ^= (key >> 31);
        return u32(key);
    }

    // calc integers hash
    template <typename IntegersVec>
    inline u64 calcIntsHash(const IntegersVec& ints) {
        u64 seed = ints.size();
        for (auto& i : ints) {
            seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }

    template <typename T>
    inline T clampToRange(const T& val, const T& min, const T& max) {
        return std::max(std::min(val, max), min);
    }

    template <typename T>
    inline int sign(T v) {
        return (v > 0) - (v < 0);
    }

    template <typename T>
    inline T wrap(T a, T b) {
        return a < 0 ? a % b + b : a % b;
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
        auto it = map.find(key);
        if(it != map.end())
            return it->second;
        return def_value;
    };

    template <typename FuctType>
    inline void dispatchFunction(void* fp) {
        (*(FuctType*)fp)();
    }
}

#endif //COMMON_H
