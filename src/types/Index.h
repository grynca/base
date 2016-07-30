#ifndef INDEX_H
#define INDEX_H
#include <stdint.h>
#include <iostream>
#include <stdint.h>

namespace grynca {

    static constexpr uint32_t InvalidId() {
        return uint32_t(-1);
    }

    class Index {
    public:
        struct Hasher {
            uint64_t operator()(const Index& vi) const;
        };

        Index()
         : uid_(uint64_t(-1)) {}
        Index(uint32_t i, uint16_t v)
         : version_(v), index_(i), unused_(0) {}

        bool isValid()const;
        static Index Invalid() { return Index(); }

        uint16_t getVersion()const { return version_; }
        uint16_t getUnused()const { return unused_; }
        uint32_t getIndex()const { return index_; }
        uint64_t getUID()const { return uid_; }

        void setIndex(uint32_t id) { index_ = id; }
        void setUnused(uint16_t u) { unused_ = u; }
        void setVersion(uint16_t v) { version_ = v; }
        void setUID(uint64_t uid) { uid_ = uid; }
    protected:

        union {
            struct {
                uint16_t version_;
                uint16_t unused_;
                uint32_t index_;
            };
            uint64_t uid_;
        };
    };

    static std::ostream& operator<<(std::ostream& os, const Index & ar) {
        os << "VI[" << ar.getIndex() << ", " << ar.getVersion()<< "]";
        return os;
    }

    static bool operator==(const Index& i1, const Index& i2) {
        return i1.getUID() == i2.getUID();
    }

    static bool operator!=(const Index& i1, const Index& i2) {
        return i1.getUID() != i2.getUID();
    }

    inline uint64_t Index::Hasher::operator()(const Index& vi) const {
        return vi.uid_;
    }

    inline bool Index::isValid()const {
        return uid_ != uint64_t(-1);
    }
}

#endif // INDEX_H
