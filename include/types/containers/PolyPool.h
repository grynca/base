#ifndef POLYPOOL_H
#define POLYPOOL_H

#include "ChunkedBuffer.h"
#include "../Index.h"
#include "../Type.h"

namespace grynca {

    // fw
    template <typename CT, typename IT> class ItemPtr;
    template <typename CT, typename IT> class ItemAutoPtr;
    template <typename CT, typename IT> class ItemRefPtr;

    // Stores multiple types with same base class
    // each type in its own container
    //  Index.aux_index_ is used as type id
    template <u32 MAX_TYPES, typename ItemBase, typename MyDerivative /*CRTP*/= void>
    class PolyPool {
        typedef std::is_same<MyDerivative, void> NoDerivativeCond;
        typedef PolyPool<MAX_TYPES, ItemBase, MyDerivative> MyType;
        COND_TYPEDEF(NoDerivativeCond::value, MyType, MyDerivative, ContainerType);
    public:
        typedef Index ItemIndexType;
        typedef ItemPtr<ContainerType, ItemBase> IPtr;
        typedef ItemAutoPtr<ContainerType, ItemBase> IAutoPtr;
        typedef ItemRefPtr<ContainerType, ItemBase> IRefPtr;

        PolyPool();
        ~PolyPool();

        template <typename T>
        void initType();
        template <typename TP>
        void initTypes();

        u32 getTypesCount()const; // all type_ids < getTypesCount() are valid

        template <typename T>
        static u16 getTypeIdOf();
        static u16 getTypeId(Index index);
        static const TypeInfo& getTypeInfo(u32 type_id);

        // creates with default constructor, returns ref to base class
        ItemBase& add(u16 type_id, Index& index_out);
        // creates with default constructor, returns pointer to real type
        u8* add2(u16 type_id, Index& index_out);
        // when you know type
        template <typename T, typename... ConstrArgs>
        T& add(Index& index_out, ConstrArgs&&... args);
        ItemBase& addCloned(Index index_src, Index& index_out);

        // without template argument item is accesed as ItemBase
        ItemBase& accItem(Index index);
        ItemBase& accItemAtPos(u16 type_id, u32 pos);
        const ItemBase& getItem(Index index)const;
        const ItemBase& getItemAtPos(u16 type_id, u32 pos)const;

        // this gets typeless address of real type
        u8* accItemPtr(Index index);
        u8* accItemPtrAtPos(u16 type_id, u32 pos);
        const u8* getItemPtr(Index index)const;
        const u8* getItemPtrAtPos(u16 type_id, u32 pos)const;

        template <typename T>
        T& accItem(Index index);
        template <typename T>
        T& accItemAtPos(u16 type_id, u32 pos);

        template <typename T>
        const T& getItem(Index index)const;
        template <typename T>
        const T& getItemAtPos(u16 type_id, u32 pos)const;

        // LoopFunc(T& item)
        template <typename T, typename LoopFunc>
        void forEach(const LoopFunc& lf)const;
        // LoopFunc(ItemBase& item)
        template <typename LoopFunc>
        void forEach(u16 type_id, const LoopFunc& lf)const;

        // LoopFunc(T& item, const Index& item_id)
        template <typename T, typename LoopFunc>
        void forEachWithId(const LoopFunc& lf)const;

        // LoopFunc(ItemBase& item, const Index& item_id)
        template <typename LoopFunc>
        void forEachWithId(u16 type_id, const LoopFunc& lf)const;

        void removeItem(Index index);
        void removeItemAtPos(u16 type_id, u32 pos);

        bool isValidIndex(Index index)const;
        u32 getSize(u16 type_id)const;
        u32 getOccupiedSize(u16 type_id)const;

        bool isFree(u16 type_id, u32 pos)const;

        u32 calcSize()const;
        u32 calcOccupiedSize()const;

        void reserve(u16 type_id, size_t count);
        bool isEmpty()const;
        void clear();
    private:
        void setFree_(u16 type_id, u32 pos);
        void unsetFree_(u16 type_id, u32 pos);
        u32 getNextFree_(u16 type_id, u32 pos)const;
        void setNextFree_(u16 type_id, u32 pos, u32 next_free_pos);
        u32 addInner_(u16 type_id, Index& index_out);

        struct TypeCtx {
            TypeCtx() : first_free_pos(InvalidId()), free_items_cnt(InvalidId()) {}

            fast_vector<u16> versions;        // first bit is 1 when slot is free
            ChunkedBuffer data;
            u32 first_free_pos;
            u32 free_items_cnt;         // InvalidId() for uninitialized type
            i32 offset_to_base;
        };

        struct TypesInitLooper {
            template <typename TP, typename T>
            static void f(PolyPool* pp);
        };

        TypeCtx types_[MAX_TYPES];
        u32 items_count_;
        u16 types_count_;
    };

}


#include "PolyPool.inl"
#endif //POLYPOOL_H
