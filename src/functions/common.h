#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

namespace grynca {

    // "Next Largest Power of 2
    // Given a binary integer value x, the next largest power of 2 can be computed by a SWAR algorithm
    // that recursively "folds" the upper bits into the lower bits. This process yields a bit vector with
    // the same most significant 1 as x, but all 1's below it. Adding 1 to that value yields the next
    // largest power of 2. For a 32-bit value:"
    inline uint32_t calcNextPowerOfTwo(uint32_t x) {
        x |= (x >> 1);
        x |= (x >> 2);
        x |= (x >> 4);
        x |= (x >> 8);
        x |= (x >> 16);
        return x+1;
    }

    // Thomas Wang's hash
    inline uint32_t calcHash32(uint32_t key)
    {
        key += ~(key << 15);
        key ^=  (key >> 10);
        key +=  (key << 3);
        key ^=  (key >> 6);
        key += ~(key << 11);
        key ^=  (key >> 16);
        return key;
    }

    inline uint32_t calcHash64(uint64_t key)
    {
        key += ~(key << 32);
        key ^= (key >> 22);
        key += ~(key << 13);
        key ^= (key >> 8);
        key += (key << 3);
        key ^= (key >> 15);
        key += ~(key << 27);
        key ^= (key >> 31);
        return uint32_t(key);
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
}

#endif //COMMON_H
