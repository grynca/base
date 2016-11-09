#ifndef VVECTOR_H
#define VVECTOR_H

#include "fast_vector.h"

namespace grynca {

    // fw
    class TypeInfo;

    // VVector as for Virtual Vector
    // types indexed with domain typeIds
    template <typename Domain = void>
    class VVector {
    public:
        VVector();
        VVector(VVector && v);
        VVector(const VVector & v);
        ~VVector();

        VVector& operator=(const VVector & v);

        template <typename T, typename ... ConstrArgs>
        T& add(ConstrArgs&&... args);     // stores casted to u8*

        template <typename T, typename Base, typename ... ConstrArgs>
        T& addAs(ConstrArgs&&... args);   // stores casted to Base*

        // returns if item was found
        template <typename T>
        bool remove(T& item);

        void remove(u32 id);

        void clear();

        u32 getSize()const;
        u8* get(u32 id);
        u8* const get(u32 id)const;
        u32 getTypeId(u32 id)const;

        void reserve(u32 size);
    private:
        const TypeInfo& getTypeInfo_(u32 id)const;

        fast_vector<u8*> items_;
        fast_vector<u32> type_ids_;

    };

}


#include "VVector.inl"
#endif //VVECTOR_H
