#ifndef INDEX_H
#define INDEX_H
#include "functions/defs.h"
#include "functions/hash.h"
#include <iostream>

namespace grynca {

    static constexpr u32 InvalidId() {
        return u32(-1);
    }

    class Index {
    public:
        Index()
         : uid_(u64(-1)) {}
        Index(u32 i, u16 v)
         : version_(v), aux_index_(0), index_(i) {}
        Index(u32 i, u16 v, u16 aux)
         : version_(v), aux_index_(aux), index_(i) {}


        bool isValid()const;
        void makeInvalid();
        static Index Invalid() { return Index(); }

        u16 getVersion()const { return version_; }
        u16 getAuxIndex()const { return aux_index_; }
        u32 getIndex()const { return index_; }
        u32 getVerAux()const { return ver_aux_; }
        u64 getUID()const { return uid_; }

        void setIndex(u32 id) { index_ = id; }
        void setAuxIndex(u16 aid) { aux_index_ = aid; }
        void setVersion(u16 v) { version_ = v; }
        void setVerAux(u32 va) { ver_aux_ = va; }
        void setUID(u64 uid) { uid_ = uid; }
    protected:

        union {
            struct {
                union {
                    struct {
                        u16 version_;
                        u16 aux_index_;
                    };
                    u32 ver_aux_;
                };
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

    inline bool Index::isValid()const {
        return uid_ != u64(-1);
    }

    inline void Index::makeInvalid() {
        uid_ = u64(-1);
    }

    template <> class Hasher<Index> {
    public:
        u32 operator()(const Index& key) const { return calcHash64(key.getUID()); }
    };
}

#endif // INDEX_H
