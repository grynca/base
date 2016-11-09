#ifndef VVECTOR2_H_H
#define VVECTOR2_H_H

#include "fast_vector.h"

namespace grynca {

    // fw
    class TypeInfo;

    // VVector as for Virtual Vector
    // types indexed with type pos
    template <typename TP>
    class VVector2 {
    public:
        VVector2();
        VVector2(VVector2 && v);
        VVector2(const VVector2 & v);
        ~VVector2();

        VVector2& operator=(const VVector2 & v);

        template <typename T, typename ... ConstrArgs>
        T& add(ConstrArgs&&... args);

        template <typename T, typename Base, typename ... ConstrArgs>
        T& addAs(ConstrArgs&&... args);

        // returns if item was found
        template <typename T>
        bool remove(T& item);

        void remove(u32 id);

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

#include "VVector2.inl"
#endif //VVECTOR2_H_H
