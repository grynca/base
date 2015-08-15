#ifndef VVECTOR_H
#define VVECTOR_H

#include "fast_vector.h"

namespace grynca {

    // fw
    class TypeInfo;

    template <typename Domain = void>
    class VVector {
    public:
        VVector();
        VVector(VVector && v);
        VVector(const VVector & v);
        ~VVector();

        VVector& operator=(const VVector & v);

        template <typename T, typename ... Args>
        T& add(Args&&... args);

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


#include "VVector.inl"
#endif //VVECTOR_H
