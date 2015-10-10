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

        void remove(uint32_t id);

        uint32_t getSize()const;
        uint8_t* get(uint32_t id);
        uint8_t* const get(uint32_t id)const;
        uint32_t getTypeId(uint32_t id)const;

        void reserve(uint32_t size);
    private:
        const TypeInfo& getTypeInfo_(uint32_t id)const;

        fast_vector<uint8_t*> items_;
        fast_vector<uint32_t> type_ids_;

    };

}

#include "VVector2.inl"
#endif //VVECTOR2_H_H
