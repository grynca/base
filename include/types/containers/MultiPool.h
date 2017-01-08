#ifndef MULTIPOOL_H
#define MULTIPOOL_H

#include "fast_vector.h"
#include "../Index.h"
#include "../Type.h"

namespace grynca {

    // Pooled data for items with multiple components
    // can contain holes

    template <u32 MAX_COMPS, typename Domain = void>
    class MultiPool {
    public:
        ~MultiPool();

        template <typename TP>
        void initComponents();

        Index add();        // does not call comps constructors (you can get ptrs to comps and call them yourself via placement new)
        Index addAndConstruct();  // calls comps default constructors
        void removeAtPos(u32 pos);
        void remove(Index index);
        void reserve(size_t count);

        bool isFree(u32 pos)const;
        u32 getComponentsCount()const;
        u8* get(Index index, u32 component_id);
        const u8* get(Index index, u32 component_id)const;
        // always check if pos is not free before using it
        u8* getAtPos(u32 pos, u32 component_id);
        const u8* getAtPos(u32 pos, u32 component_id)const;
        Index getIndexForPos(u32 pos);      // get index for data at pos
        void getIndexForPos2(u32 pos, Index& index_out);

        const TypeInfo& getTypeInfo(u32 component_id)const;
        u8* getCompsData(u32 component_id);
        bool isValidIndex(Index index)const;
        u32 size()const;
        u32 occupiedSize()const;
        bool empty()const;
        void clear();
        f32 getMemoryWaste();
    private:
        void setFree_(u32 pos);
        void unsetFree_(u32 pos);

        fast_vector<u16> versions_;        // first bit is 1 when slot is free
        fast_vector<u8> data_[MAX_COMPS];
        fast_vector<u32> component_types_;     // type ids of components
        fast_vector<u32> free_slots_;

        struct CompsInitLooper {
            template <typename TP, typename T>
            static void f(fast_vector<u32>& type_ids_out) {
                type_ids_out.push_back(Type<T, Domain>::getInternalTypeId());
            }
        };
    };

}


#include "MultiPool.inl"
#endif //MULTIPOOL_H
