#ifndef INDEX_H
#define INDEX_H
#include "functions/defs.h"
#include <iostream>

namespace grynca {

    static constexpr u32 InvalidId() {
        return u32(-1);
    }

    class Index {
    public:
        struct Hasher {
            u64 operator()(const Index& vi) const;
        };

        Index()
         : uid_(u64(-1)) {}
        Index(u32 i, u16 v)
         : version_(v), unused_(0), index_(i) {}

        bool isValid()const;
        static Index Invalid() { return Index(); }

        u16 getVersion()const { return version_; }
        u16 getUnused()const { return unused_; }
        u32 getIndex()const { return index_; }
        u64 getUID()const { return uid_; }

        void setIndex(u32 id) { index_ = id; }
        void setUnused(u16 u) { unused_ = u; }
        void setVersion(u16 v) { version_ = v; }
        void setUID(u64 uid) { uid_ = uid; }
    protected:

        union {
            struct {
                u16 version_;
                u16 unused_;
                u32 index_;
            };
            u64 uid_;
        };
    };

    static inline std::ostream& operator<<(std::ostream& os, const Index & ar) {
        os << "Id[" << ar.getIndex() << ", " << ar.getVersion()<< "]";
        return os;
    }

    static inline bool operator==(const Index& i1, const Index& i2) {
        return i1.getUID() == i2.getUID();
    }

    static inline bool operator!=(const Index& i1, const Index& i2) {
        return i1.getUID() != i2.getUID();
    }

    inline u64 Index::Hasher::operator()(const Index& vi) const {
        return vi.uid_;
    }

    inline bool Index::isValid()const {
        return uid_ != u64(-1);
    }
}

#endif // INDEX_H
