#ifndef VERSIONEDINDEX_H
#define VERSIONEDINDEX_H
#include <stdint.h>
#include <iostream>

namespace grynca {
    struct VersionedIndex {
        uint32_t version;
        uint32_t index;
    };

    static std::ostream& operator<<(std::ostream& os, const VersionedIndex& ar) {
        os << "VI[" << ar.index << ", " << ar.version << "]";
        return os;
    }
}

#endif // VERSIONEDINDEX_H
