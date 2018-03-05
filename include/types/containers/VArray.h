#ifndef VARRAY_H
#define VARRAY_H

#include "Pool.h"
#include "TightPool.h"

namespace grynca {

    // fw
    class TypeInfo;
    template <typename CT, typename IT> class ItemPtr;
    template <typename CT, typename IT> class ItemAutoPtr;
    template <typename CT, typename IT> class ItemRefPtr;

    // Virtual Array
    // Items are in single continuous pool
    // each item takes place as largest possible type defined via initTypes<ItemTypes>()
    // so be aware of wasted space
    template <typename ItemBase, typename MyDerivative /*CRTP*/ = void, typename PoolType = Pool>
    class VArray {
        typedef std::is_same<MyDerivative, void> NoDerivativeCond;
        typedef VArray<ItemBase, MyDerivative, PoolType> MyType;
        COND_TYPEDEF(NoDerivativeCond::value, MyType, MyDerivative, ContainerType);
    public:
        typedef ItemBase ItemType;
        typedef Index ItemIndexType;
        typedef ItemPtr<ContainerType, ItemType> IPtr;
        typedef ItemAutoPtr<ContainerType, ItemType> IAutoPtr;
        typedef ItemRefPtr<ContainerType, ItemType> IRefPtr;

        static constexpr u32 MAX_TYPES = 256;

        VArray();
        ~VArray();

        VArray& operator=(const VArray & v);

        // TP should be all potencial derivatives of ItemBase
        template <typename TP>
        void initTypes();

        u32 getTypesCount()const; // all type_ids < getTypesCount() are valid

        template <typename T>
        static u16 getTypeIdOf();
        static u16 getTypeId(Index index);
        static const TypeInfo& getTypeInfo(u32 type_id);

        // returns added item index
        template <typename T, typename ...ConstructionArgs>
        Index add(ConstructionArgs&&... args);
        Index add(u16 type_id);     // def. construction

        // returns added item
        template <typename T, typename ...ConstructionArgs>
        T& add2(Index& index_out, ConstructionArgs&&... args);
        ItemBase& add2(u16 type_id, Index& index_out);     // def.construction
        u8* add3(u16 type_id, Index& index_out);        // def.construction, returns pointer to real type
        ItemBase& addCloned(Index index_src, Index& index_out);

        // variant must have same type indices
        template <typename VariantType>
        void copyToVariant(Index item_id, VariantType& v_out)const;
        template <typename VariantType>
        void copyFromVariant(Index item_id, const VariantType& v_in);

        // without template argument item is accesed as ItemBase
        ItemBase& accItem(Index index);
        ItemBase& accItemAtPos(u32 pos);
        ItemBase& accItemAtPos2(u32 pos);     // faster (use when you are sure that pos is not hole)
        const ItemBase& getItem(Index index)const;
        const ItemBase& getItemAtPos(u32 pos)const;
        const ItemBase& getItemAtPos2(u32 pos)const;      // faster (use when you are sure that pos is not hole)

        // when you know item real type
        template <typename T>
        T& accItem(Index index);
        template <typename T>
        T& accItemAtPos(u32 pos);
        template <typename T>
        T& accItemAtPos2(u32 pos);      // faster (use when you are sure that pos is not hole)
        template <typename T>
        const T& getItem(Index index)const;
        template <typename T>
        const T& getItemAtPos(u32 pos)const;
        template <typename T>
        const T& getItemAtPos2(u32 pos)const;      // faster (use when you are sure that pos is not hole)


        bool isValidIndex(Index index)const;

        void removeItem(Index index);
        void removeItemAtPos(u32 pos);
        void reserve(u32 size);
        void clear();

        u32 getSize()const;
        u32 getItemSize()const;

        PoolType& accPool() { return pool_; }
        const PoolType& getPool()const { return pool_; }
    private:
        struct TypesInitLooper {
            template <typename TP, typename T>
            static void f(VArray* vv, u32& item_size_out);
        };

        static i32* accOffsetsToBase_();

        u32 types_count_;
        PoolType pool_;
    };


    template <typename BaseClass, typename MyDerivative = void>
    class TightVArray: public VArray<BaseClass, MyDerivative, TightPool> {};
}


#include "VArray.inl"
#endif //VARRAY_H
