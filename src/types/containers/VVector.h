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
        T& add(ConstrArgs&&... args);     // stores casted to uint8_t*

        template <typename T, typename Base, typename ... ConstrArgs>
        T& addAs(ConstrArgs&&... args);   // stores casted to Base*

        // returns if item was found
        template <typename T>
        bool remove(T& item);

        void remove(uint32_t id);

        void clear();

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


#include "VVector.inl"
#endif //VVECTOR_H
