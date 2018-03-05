#include "VArray.h"
#include "../Type.h"

#define VA_TPL template <typename BaseClass, typename D, typename PoolType>
#define VA_TYPE VArray<BaseClass, D, PoolType>

namespace grynca {

    VA_TPL
    inline VA_TYPE::VArray()
     : types_count_(0)
    {}

    VA_TPL
    inline VA_TYPE::~VArray() {
        clear();
    }

    VA_TPL
    inline VA_TYPE& VA_TYPE::operator=(const VArray& v) {
        clear();
        new (this) VA_TYPE(v);
        return *this;
    }

    VA_TPL
    template <typename TP>
    inline void VA_TYPE::initTypes() {
        ASSERT_M(types_count_ == 0, "Was already initialized");
        types_count_ = TP::getTypesCount();

        u32 item_size = 0;
        TP::template callOnTypes<TypesInitLooper>(this, item_size);
        pool_.init(item_size);
    }

    VA_TPL
    inline u32 VA_TYPE::getTypesCount()const {
        return types_count_;
    }

    VA_TPL
    template <typename T>
    inline u16 VA_TYPE::getTypeIdOf() {
        // static
        return u16(Type<T, VA_TYPE>::getInternalTypeId());

    }

    VA_TPL
    inline u16 VA_TYPE::getTypeId(Index index) {
        // static
        return index.getAuxIndex();
    }

    VA_TPL
    inline const TypeInfo& VA_TYPE::getTypeInfo(u32 type_id) {
        // static
        return InternalTypes<VA_TYPE>::getInfo(type_id);
    }

    VA_TPL
    template <typename T, typename ...ConstructionArgs>
    inline Index VA_TYPE::add(ConstructionArgs&&... args) {
        u16 type_id = getTypeIdOf<T>();
        ASSERT(type_id < types_count_);

        u8* item_ptr;
        Index item_id = pool_.add(item_ptr, type_id);
        Type<T>::create2(item_ptr, std::forward<ConstructionArgs>(args)...);
        return item_id;
    }

    VA_TPL
    inline Index VA_TYPE::add(u16 type_id) {
        ASSERT(type_id < types_count_);

        u8* item_ptr;
        Index item_id = pool_.add(item_ptr, type_id);
        getTypeInfo(type_id).getDefaultConstr()(item_ptr);
        return item_id;
    }

    VA_TPL
    template <typename T, typename ...ConstructionArgs>
    inline T& VA_TYPE::add2(Index& index_out, ConstructionArgs&&... args) {
        u16 type_id = getTypeIdOf<T>();
        ASSERT(type_id < types_count_);

        u8* item_ptr;
        index_out = pool_.add(item_ptr, type_id);
        Type<T>::create2(item_ptr, std::forward<ConstructionArgs>(args)...);
        return *(T*)item_ptr;
    }

    VA_TPL
    inline BaseClass& VA_TYPE::add2(u16 type_id, Index& index_out) {
        ASSERT(type_id < types_count_);

        u8* item_ptr;
        index_out = pool_.add(item_ptr, type_id);
        getTypeInfo(type_id).getDefaultConstr()(item_ptr);

        return *(BaseClass*)(item_ptr + accOffsetsToBase_()[type_id]);
    }

    VA_TPL
    inline u8* VA_TYPE::add3(u16 type_id, Index& index_out) {
        ASSERT(type_id < types_count_);

        u8* item_ptr;
        index_out = pool_.add(item_ptr, type_id);
        getTypeInfo(type_id).getDefaultConstr()(item_ptr);
        return item_ptr;
    }

    VA_TPL
    inline BaseClass& VA_TYPE::addCloned(Index index_src, Index& index_out) {
        ASSERT(isValidIndex(index_src));

        u16 type_id = getTypeId(index_src);
        u8* new_item_ptr = add2(type_id, index_out);
        const u8* src_item_ptr = pool_.getItemPtr(index_src);
        getTypeInfo(type_id).getCopyFunc()(new_item_ptr, src_item_ptr);
        return *(BaseClass*)(new_item_ptr + accOffsetsToBase_()[type_id]);
    }

    VA_TPL
    template <typename VariantType>
    inline void VA_TYPE::copyToVariant(Index item_id, VariantType& v_out)const {
        ASSERT_M(getItemSize() == VariantType::getDataSize(), "Different space size in variant (probably for different types?)");
        const u8* item_ptr = getPool().getItem(item_id);
        v_out.set(item_id.getAuxIndex(), item_ptr);
    }

    VA_TPL
    template <typename VariantType>
    inline void VA_TYPE::copyFromVariant(Index item_id, const VariantType& v_in) {
        ASSERT_M(getItemSize() == VariantType::getDataSize(), "Different space size in variant (probably for different types?)");
        u8* item_ptr = accPool().accItem(item_id);
        u16 type_id = item_id.getAuxIndex();
        getTypeInfo(type_id).getCopyFunc()(item_ptr, v_in.getData());
    }

    VA_TPL
    inline BaseClass& VA_TYPE::accItem(Index index) {
        u8* item_ptr = pool_.accItem(index);
        return *(BaseClass*)(item_ptr + accOffsetsToBase_()[getTypeId(index)]);
    }

    VA_TPL
    inline BaseClass& VA_TYPE::accItemAtPos(u32 pos) {
        u16 type_id = getTypeId(pool_.getIndexForPos(pos));
        u8* item_ptr = pool_.accItemAtPos(pos);
        return *(BaseClass*)(item_ptr + accOffsetsToBase_()[type_id]);
    }

    VA_TPL
    inline BaseClass& VA_TYPE::accItemAtPos2(u32 pos) {
        u16 type_id = getTypeId(pool_.getIndexForPos(pos));
        u8* item_ptr = pool_.accItemAtPos2(pos);
        return *(BaseClass*)(item_ptr + accOffsetsToBase_()[type_id]);
    }

    VA_TPL
    inline const BaseClass& VA_TYPE::getItem(Index index)const {
        const u8* item_ptr = pool_.getItem(index);
        return *(BaseClass*)(item_ptr + accOffsetsToBase_()[getTypeId(index)]);
    }

    VA_TPL
    inline const BaseClass& VA_TYPE::getItemAtPos(u32 pos)const {
        u16 type_id = getTypeId(pool_.getIndexForPos(pos));
        const u8* item_ptr = pool_.getItemAtPos(pos);
        return *(BaseClass*)(item_ptr + accOffsetsToBase_()[type_id]);
    }

    VA_TPL
    inline const BaseClass& VA_TYPE::getItemAtPos2(u32 pos)const {
        u16 type_id = getTypeId(pool_.getIndexForPos(pos));
        const u8* item_ptr = pool_.getItemAtPos2(pos);
        return *(BaseClass*)(item_ptr + accOffsetsToBase_()[type_id]);
    }

    VA_TPL
    template <typename T>
    inline T& VA_TYPE::accItem(Index index) {
        return *((T*)pool_.accItem(index));
    }

    VA_TPL
    template <typename T>
    inline T& VA_TYPE::accItemAtPos(u32 pos) {
        return *((T*)pool_.accItemAtPos(pos));
    }

    VA_TPL
    template <typename T>
    inline T& VA_TYPE::accItemAtPos2(u32 pos) {
        return *(T*)pool_.accItemAtPos2(pos);
    }

    VA_TPL
    template <typename T>
    inline const T& VA_TYPE::getItem(Index index)const {
        return *((T*)pool_.getItem(index));
    }

    VA_TPL
    template <typename T>
    inline const T& VA_TYPE::getItemAtPos(u32 pos)const {
        return *((T*)pool_.getItemAtPos(pos));
    }

    VA_TPL
    template <typename T>
    inline const T& VA_TYPE::getItemAtPos2(u32 pos)const {
        return *(T*)pool_.getItemAtPos2(pos);
    }

    VA_TPL
    inline bool VA_TYPE::isValidIndex(Index index)const {
        return pool_.isValidIndex(index);
    }

    VA_TPL
    inline void VA_TYPE::removeItem(Index index) {
        u16 type_id = getTypeId(index);
        pool_.removeItem(index, getTypeInfo(type_id).getDestroyFunc());
    }

    VA_TPL
    inline void VA_TYPE::removeItemAtPos(u32 pos) {
        removeItem(pool_.getIndexForPos(pos));
    }

    VA_TPL
    inline void VA_TYPE::reserve(u32 size) {
        pool_.reserve(size);
    }

    VA_TPL
    inline void VA_TYPE::clear() {
        for (u32 i=0; i<getSize(); ++i) {
            u16 type_id = getTypeId(pool_.getIndexForPos(i));
            u8* item_ptr = pool_.accItemAtPos(i);
            if (item_ptr)
                getTypeInfo(type_id).getDestroyFunc()(item_ptr);
        }
        pool_.clear();
    }

    VA_TPL
    inline u32 VA_TYPE::getSize()const {
        return (u32)pool_.size();
    }

    VA_TPL
    inline u32 VA_TYPE::getItemSize()const {
        return pool_.getItemSize();
    }

    VA_TPL
    inline i32* VA_TYPE::accOffsetsToBase_() {
        // static
        static i32 otb[MAX_TYPES] = {0};
        return otb;
    }

    VA_TPL
    template <typename TP, typename T>
    inline void VA_TYPE::TypesInitLooper::f(VArray* vv, u32& item_size_out) {
        // static
        u16 tid = vv->getTypeIdOf<T>();
        ASSERT(tid < MAX_TYPES);
        const TypeInfo& ti = vv->getTypeInfo(tid);
        u32 type_size = u32(ti.getSize());
        if (type_size > item_size_out)
            item_size_out = type_size;

        vv->accOffsetsToBase_()[tid] = calcBaseOffset<T, BaseClass>();
    };
}

#undef VA_TPL
#undef VA_TYPE