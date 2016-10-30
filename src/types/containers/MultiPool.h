#ifndef MULTIPOOL_H
#define MULTIPOOL_H

#include "fast_vector.h"
#include "../Index.h"
#include "../Type.h"

namespace grynca {

    // Pooled data for items with multiple components
    // can contain holes

    template <uint32_t MAX_COMPS, typename Domain = void>
    class MultiPool {
    public:
        ~MultiPool();

        template <typename TP>
        void initComponents();

        Index add();        // you must get ptrs for components via get() and call constructors on them
        void removeAtPos(uint32_t pos);
        void remove(Index index);
        void reserve(size_t count);

        bool isFree(uint32_t pos)const;
        uint8_t* get(Index index, uint32_t component_id);
        const uint8_t* get(Index index, uint32_t component_id)const;
        // always check if pos is not free before using it
        uint8_t* getAtPos(uint32_t pos, uint32_t component_id);
        const uint8_t* getAtPos(uint32_t pos, uint32_t component_id)const;
        Index getIndexForPos(uint32_t pos);      // get index for data at pos
        void getIndexForPos2(uint32_t pos, Index& index_out);

        const TypeInfo& getTypeInfo(uint32_t component_id)const;
        uint8_t* getCompsData(uint32_t component_id);
        bool isValidIndex(Index index)const;
        uint32_t size()const;
        uint32_t occupiedSize()const;
        bool empty()const;
        void clear();
        float getMemoryWaste();
    private:
        void setFree_(uint32_t pos);
        void unsetFree_(uint32_t pos);

        fast_vector<uint16_t> versions_;        // first bit is 1 when slot is free
        fast_vector<uint8_t> data_[MAX_COMPS];
        fast_vector<uint32_t> component_types_;     // type ids of components
        fast_vector<uint32_t> free_slots_;

        struct CompsInitLooper {
            template <typename TP, typename T>
            static void f(fast_vector<uint32_t>& type_ids_out) {
                type_ids_out.push_back(Type<T, Domain>::getInternalTypeId());
            }
        };
    };

}


#include "MultiPool.inl"
#endif //MULTIPOOL_H
