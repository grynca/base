#ifndef MULTIPOOL_H
#define MULTIPOOL_H

#include "ChunkedBuffer.h"
#include "../Index.h"
#include "../Type.h"

namespace grynca {

    // Pooled data for items with multiple components
    // can contain holes


    /*
     *  components in row arrays, indexed compositions on cols
     *           1   2   3   4   5   6
     *  COMP1 : |X| |X| |X| |X| | | |X| ...
     *  COMP2 : |X| |X| |X| |X| | | |X| ...
     *  COMP3 : |X| |X| |X| |X| | | |X| ...
     *  ...
     *                      hole-^
     */

    template <u32 MAX_COMPS, typename Domain = void>
    class CompsPool {
    public:
        CompsPool();
        ~CompsPool();

        template <typename TP>
        void initComponents();

        Index add();        // does not call comps constructors (you can get ptrs to comps and call them yourself via placement new)
        Index addAndConstruct();  // calls comps default constructors
        void removeItemAtPos(u32 pos);
        void removeItem(Index index);
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
        bool isValidIndex(Index index)const;
        u32 getSize()const;
        u32 getOccupiedSize()const;
        bool isEmpty()const;
        void clear();
        f32 getMemoryWaste();

        ChunkedBuffer& accInnerBuffer(u32 component_id);
        const ChunkedBuffer& getInnerBuffer(u32 component_id)const;
    private:
        void setFree_(u32 pos);
        void unsetFree_(u32 pos);

        fast_vector<u16> versions_;        // first bit is 1 when slot is free
        fast_vector<u32> free_slots_;
        ChunkedBuffer data_[MAX_COMPS];
        u32 component_type_ids_[MAX_COMPS];     // type ids of components
        u32 component_types_cnt_;

        struct CompsInitLooper {
            template <typename TP, typename T>
            static void f(u32* type_ids) {
                u32 pos = TP::template pos<T>();
                ASSERT_M(type_ids[pos] == InvalidId(), "Duplicite component?");
                type_ids[pos] = Type<T, Domain>::getInternalTypeId();
            }
        };
    };

}


#include "CompsPool.inl"
#endif //MULTIPOOL_H
