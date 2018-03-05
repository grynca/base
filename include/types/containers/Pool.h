#ifndef POOL_H
#define POOL_H

#include "fast_vector.h"
#include "../Index.h"
#include "../Type.h"

namespace grynca {

    // has holes inside from removed items
    // Index.getIndex() == position in memory

    class Pool {
    public:
        Pool();     // uninitialized, must call init afterwards
        Pool(u32 item_size);
        void init(u32 item_size);

        Index add(u8*& new_item_out);
        Index add(u8*& new_item_out, u16 aux_id);

        void removeItemAtPos(u32 pos);
        void removeItemAtPos(u32 pos, DestroyFunc destructor);
        void removeItem(Index index);
        void removeItem(Index index, DestroyFunc destructor);
        void reserve(size_t count);

        u32 getItemPos(const u8* item)const;       // item must be in pool
        u32 getItemPos(Index index)const;
        u8* accItem(Index index);
        const u8* getItem(Index index)const;

        // if you only have index part of Index
        // (for this pool same as (get/acc)ItemAtPos
        u8* accItemWithInnerIndex(u32 inner_id);
        const u8* getItemWithInnerIndex(u32 inner_id)const;

        // can return NULL if is hole
        u8* accItemAtPos(u32 pos);
        const u8* getItemAtPos(u32 pos)const;

        // if you are sure that pos is not hole use this
        u8* accItemAtPos2(u32 pos);
        const u8* getItemAtPos2(u32 pos)const;

        Index getIndexForPos(u32 pos)const;      // get index for data at pos
        void getIndexForPos2(u32 pos, Index& index_out)const;
        Index getFullIndex(u32 index_id)const;      // if you only have Index.index this will get you corresponding version&aux
                                                    // for this type of pool it is the same as getIndexForPos()

        bool isValidIndex(Index index)const;

        u8* getData();

        u32 size()const;
        u32 calcFreeSlotsCount()const;
        u32 calcOccupiedSize()const;
        bool empty()const;
        void clear();
        void clear(DestroyFunc destructor);
        f32 calcMemoryWaste();
        u32 getItemSize()const { return item_size_; }
    private:
        struct ItemCtx {
            constexpr ItemCtx(u16 aid, u16 v) : version(v), aux_id(aid) {}

            union {
                struct {
                    u16 version;
                    u16 aux_id;
                };
                u32 ver_aux;
            };
        };

        bool isFree_(u32 pos)const;
        void setFree_(u32 pos);
        void unsetFree_(u32 pos);
        u32 getNextFree_(u32 pos)const;
        void setNextFree_(u32 pos, u32 next_free_pos);

        u32 item_size_;
        u32 first_free_pos_;

        fast_vector<ItemCtx> item_ctxs_;        // first bit is 1 when slot is free
        fast_vector<u8> data_;
    };
}

#include "Pool.inl"
#endif //POOL_H
