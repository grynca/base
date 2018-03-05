#ifndef HASH_H
#define HASH_H

#include "defs.h"
#include "3rdp/ustring.h"

namespace grynca {

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

    template <typename ST>
    inline u64 calcStringHash(const ST& s) {
        static const u32 A =  54059;
        static const u32 B = 76963;
        static const u32 C = 86969;
        static const u32 FIRSTH = 37;

        u64 h = FIRSTH;
        for (auto it = s.begin(); it!=s.end(); ++it) {
            h = (h * A) ^ (*it * B);
        }
        return h%C;
    }

    template <typename T> class Hasher {
    public:
        size_t operator()(const T& key) const {
            std::hash<T> h;
            return h(key);
        }
    };

    template <> class Hasher<u32> {
    public:
        u32 operator()(const u32& key) const { return calcHash32(key); }
    };

    template <> class Hasher<u64> {
    public:
        u32 operator()(const u64& key) const { return calcHash64(key); }
    };

    template <> class Hasher<ustring> {
    public:
        u64 operator()(const ustring& key) const { return calcStringHash(key); }
    };
}

#endif //HASH_H
