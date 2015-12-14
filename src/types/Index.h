#ifndef INDEX_H
#define INDEX_H
#include <stdint.h>
#include <iostream>
#include <stdint.h>

namespace grynca {

    namespace Index {
        static constexpr uint32_t Invalid() {
            return uint32_t(-1);
        }
    }

    struct VersionedIndex {
        uint32_t version;
        uint32_t index;

        static constexpr VersionedIndex Invalid();
    };

    static std::ostream& operator<<(std::ostream& os, const VersionedIndex & ar) {
        os << "VI[" << ar.index << ", " << ar.version << "]";
        return os;
    }

    static bool operator==(const VersionedIndex& i1, const VersionedIndex& i2) {
        return *(uint64_t*)&i1 == *(uint64_t*)&i2;
    }


    inline constexpr VersionedIndex VersionedIndex::Invalid() {
        return {Index::Invalid(), Index::Invalid()};
    }
}

#endif // INDEX_H
