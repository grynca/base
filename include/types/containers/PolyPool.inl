#include "PolyPool.h"

#define FREE_BITMASK u16(1<<15)
#define PP_TPL template <u32 MAX_TYPES, typename BaseClass, typename MyDerivative>
#define PP_TYPE PolyPool<MAX_TYPES, BaseClass, MyDerivative>

namespace grynca {

    PP_TPL
    inline PP_TYPE::PolyPool()
     : items_count_(0), types_count_(0)
    {}

    PP_TPL
    inline PP_TYPE::~PolyPool() {
        clear();
    }

    PP_TPL
    template <typename T>
    inline void PP_TYPE::initType() {
        TypesInitLooper::template f<void, T>(this);
    }

    PP_TPL
    template <typename TP>
    inline void PP_TYPE::initTypes() {
        TP::template callOnTypes<TypesInitLooper>(this);
    }

    PP_TPL
    inline u32 PP_TYPE::getTypesCount()const {
        return types_count_;
    }

    PP_TPL
    template <typename T>
    inline u16 PP_TYPE::getTypeIdOf() {
        // static
        return u16(Type<T, PP_TYPE>::getInternalTypeId());
    }

    PP_TPL
    u16 PP_TYPE::getTypeId(Index index) {
        // static
        return index.getAuxIndex();
    }

    PP_TPL
    inline const TypeInfo& PP_TYPE::getTypeInfo(u32 type_id) {
        // static
        return InternalTypes<PP_TYPE>::getInfo(type_id);
    }

    PP_TPL
    inline BaseClass& PP_TYPE::add(u16 type_id, Index& index_out) {
        u32 slot_id = addInner_(type_id, index_out);

        u8* item_ptr = types_[type_id].data.accItem(slot_id);
        const TypeInfo& ti = getTypeInfo(type_id);
        ti.getDefaultConstr()(item_ptr);
        return *(BaseClass*)(item_ptr + types_[type_id].offset_to_base);
    }

    PP_TPL
    inline u8* PP_TYPE::add2(u16 type_id, Index& index_out) {
        u32 slot_id = addInner_(type_id, index_out);

        u8* item_ptr = types_[type_id].data.accItem(slot_id);
        const TypeInfo& ti = getTypeInfo(type_id);
        ti.getDefaultConstr()(item_ptr);
        return item_ptr;
    }

    PP_TPL
    template <typename T, typename... ConstrArgs>
    inline T& PP_TYPE::add(Index& index_out, ConstrArgs&&... args) {
        u16 tid = getTypeIdOf<T>();
        u32 slot_id = addInner_(tid, index_out);

        u8* item_ptr = types_[tid].data.accItem(slot_id);
        Type<T>::create2(item_ptr, std::forward<ConstrArgs>(args)...);
        return *(T*)item_ptr;
    }

    PP_TPL
    inline BaseClass& PP_TYPE::addCloned(Index index_src, Index& index_out) {
        ASSERT(isValidIndex(index_src));

        u16 type_id = getTypeId(index_src);
        u8* new_item_ptr = add2(type_id, index_out);
        const u8* src_item_ptr = getItemPtr(index_src);
        getTypeInfo(type_id).getCopyFunc()(new_item_ptr, src_item_ptr);
        return *(BaseClass*)(new_item_ptr + types_[type_id].offset_to_base);
    }

    PP_TPL
    inline BaseClass& PP_TYPE::accItem(Index index) {
        return accItemAtPos(getTypeId(index), index.getIndex());
    }

    PP_TPL
    inline BaseClass& PP_TYPE::accItemAtPos(u16 type_id, u32 pos) {
        ASSERT(type_id < types_count_)

        TypeCtx& tctx = types_[type_id];
        u8* item_ptr = tctx.data.accItem(pos);
        return *(BaseClass*)(item_ptr + tctx.offset_to_base);
    }

    PP_TPL
    inline const BaseClass& PP_TYPE::getItem(Index index)const {
        return getItemAtPos(getTypeId(index), index.getIndex());
    }

    PP_TPL
    inline const BaseClass& PP_TYPE::getItemAtPos(u16 type_id, u32 pos)const {
        ASSERT(type_id < types_count_)
        const TypeCtx& tctx = types_[type_id];
        const u8* item_ptr = tctx.data.getItem(pos);
        return *(const BaseClass*)(item_ptr + tctx.offset_to_base);
    }

    PP_TPL
    inline u8* PP_TYPE::accItemPtr(Index index) {
        return accItemPtrAtPos(getTypeId(index), index.getIndex());
    }

    PP_TPL
    inline u8* PP_TYPE::accItemPtrAtPos(u16 type_id, u32 pos) {
        ASSERT(type_id < types_count_)
        TypeCtx& tctx = types_[type_id];
        return tctx.data.accItem(pos);
    }

    PP_TPL
    inline const u8* PP_TYPE::getItemPtr(Index index)const {
        return getItemPtrAtPos(getTypeId(index), index.getIndex());
    }

    PP_TPL
    inline const u8* PP_TYPE::getItemPtrAtPos(u16 type_id, u32 pos)const {
        ASSERT(type_id < types_count_)
        const TypeCtx& tctx = types_[type_id];
        return tctx.data.getItem(pos);
    }

    PP_TPL
    template <typename T>
    inline T& PP_TYPE::accItem(Index index) {
        return accItemAtPos<T>(getTypeId(index), index.getIndex());
    }

    PP_TPL
    template <typename T>
    inline T& PP_TYPE::accItemAtPos(u16 type_id, u32 pos) {
        ASSERT(type_id < types_count_)
        TypeCtx& tctx = types_[type_id];
        const u8* item_ptr = tctx.data.accItem(pos);
        return *(T*)item_ptr;
    }

    PP_TPL
    template <typename T>
    inline const T& PP_TYPE::getItem(Index index)const {
        return getItemAtPos<T>(getTypeId(index), index.getIndex());
    }

    PP_TPL
    template <typename T>
    inline const T& PP_TYPE::getItemAtPos(u16 type_id, u32 pos)const {
        ASSERT(type_id < types_count_)
        TypeCtx& tctx = types_[type_id];
        const u8* item_ptr = tctx.data.getItem(pos);
        return *(const T*)item_ptr;
    }

    PP_TPL
    template <typename T, typename LoopFunc>
    inline void PP_TYPE::forEach(const LoopFunc& lf)const {
        u16 type_id = u16(Type<T, PP_TYPE>::getInternalTypeId());
        ASSERT(type_id < types_count_);
        const TypeCtx& tctx = types_[type_id];
        tctx.data.forEachWithId([&](u8* item_ptr, u32 item_pos) {
            if (!isFree(type_id, item_pos)) {
                lf(*(T*)item_ptr);
            }
        });
    }

    PP_TPL
    template <typename LoopFunc>
    inline void PP_TYPE::forEach(u16 type_id, const LoopFunc& lf)const {
        ASSERT(type_id < types_count_);
        const TypeCtx& tctx = types_[type_id];
        tctx.data.forEachWithId([&](u8* item_ptr, u32 item_pos) {
            if (!isFree(type_id, item_pos)) {
                lf(*(BaseClass*)(item_ptr + tctx.offset_to_base));
            }
        });
    }

    PP_TPL
    template <typename T, typename LoopFunc>
    inline void PP_TYPE::forEachWithId(const LoopFunc& lf)const {
        u16 type_id = u16(Type<T, PP_TYPE>::getInternalTypeId());
        ASSERT(type_id < types_count_);
        const TypeCtx& tctx = types_[type_id];
        tctx.data.forEachWithId([&](u8* item_ptr, u32 item_pos) {
            if (!isFree(type_id, item_pos)) {
                Index id;
                id.setAuxIndex(type_id);
                id.setIndex(item_pos);
                id.setVersion(tctx.versions[item_pos]);
                lf(*(T*)item_ptr, id);
            }
        });
    }

    PP_TPL
    template <typename LoopFunc>
    inline void PP_TYPE::forEachWithId(u16 type_id, const LoopFunc& lf)const {
        ASSERT(type_id < types_count_);
        const TypeCtx& tctx = types_[type_id];
        tctx.data.forEachWithId([&](u8* item_ptr, u32 item_pos) {
            if (!isFree(type_id, item_pos)) {
                Index id;
                id.setAuxIndex(type_id);
                id.setIndex(item_pos);
                id.setVersion(tctx.versions[item_pos]);
                lf(*(BaseClass*)(item_ptr + tctx.offset_to_base), id);
            }
        });
    }

    PP_TPL
    inline void PP_TYPE::removeItem(Index index) {
        ASSERT(isValidIndex(index));
        removeItemAtPos(getTypeId(index), index.getIndex());
    }

    PP_TPL
    inline void PP_TYPE::removeItemAtPos(u16 type_id, u32 pos) {
        ASSERT(!isFree(type_id, pos));

        TypeCtx& tctx = types_[type_id];
        u8* item_ptr = tctx.data.accItem(pos);
        const TypeInfo& ti = getTypeInfo(type_id);
        ti.getDestroyFunc()(item_ptr);
        setNextFree_(type_id, pos, tctx.first_free_pos);
        tctx.first_free_pos = pos;
        ++tctx.free_items_cnt;
        ++tctx.versions[pos];
        setFree_(type_id, pos);
        --items_count_;
    }

    PP_TPL
    inline bool PP_TYPE::isValidIndex(Index index)const {
        u16 tid = getTypeId(index);
        ASSERT(tid < types_count_)
        if (index.getIndex() >= getSize(tid))
            return false;
        return types_[tid].versions[index.getIndex()] == index.getVersion();
    }

    PP_TPL
    inline u32 PP_TYPE::getSize(u16 type_id)const {
        ASSERT(type_id < types_count_)
        return u32(types_[type_id].versions.size());
    }

    PP_TPL
    inline u32 PP_TYPE::getOccupiedSize(u16 type_id)const {
        return getSize(type_id)-types_[type_id].free_items_cnt;
    }

    PP_TPL
    inline bool PP_TYPE::isFree(u16 type_id, u32 pos)const {
        ASSERT(pos < types_[type_id].versions.size());
        return (types_[type_id].versions[pos] & u16(FREE_BITMASK)) != 0;
    }

    PP_TPL
    inline u32 PP_TYPE::calcSize()const {
        u32 s = 0;
        for (u16 tid=0; tid<types_count_; ++tid) {
            s += getSize(tid);
        }
        return s;
    }

    PP_TPL
    inline u32 PP_TYPE::calcOccupiedSize()const {
        u32 s = 0;
        for (u16 tid=0; tid<types_count_; ++tid) {
            s += getOccupiedSize(tid);
        }
        return s;
    }

    PP_TPL
    inline void PP_TYPE::reserve(u16 type_id, size_t count) {
        types_[type_id].versions.reserve(count);
        types_[type_id].data.reserve(count);
    }

    PP_TPL
    inline bool PP_TYPE::isEmpty()const {
        return items_count_ == 0;
    }

    PP_TPL
    inline void PP_TYPE::clear() {
        for (u16 tid=0; tid<types_count_; ++tid) {
            DestroyFunc destructor = getTypeInfo(tid).getDestroyFunc();
            for (u32 j=0; j<types_[tid].data.getSize(); ++j) {
                if (isFree(tid, j))
                    continue;
                u8* ptr = types_[tid].data.accItem(j);
                destructor(ptr);
            }
            types_[tid].data.clear();
            types_[tid].versions.clear();
            types_[tid].first_free_pos = InvalidId();
            types_[tid].free_items_cnt = 0;
        }
        items_count_ = 0;
    }

    PP_TPL
    inline void PP_TYPE::setFree_(u16 type_id, u32 pos) {
        ASSERT(pos < types_[type_id].versions.size());
        types_[type_id].versions[pos] |= FREE_BITMASK;
    }

    PP_TPL
    inline void PP_TYPE::unsetFree_(u16 type_id, u32 pos) {
        ASSERT(pos < types_[type_id].versions.size());
        types_[type_id].versions[pos] &= ~FREE_BITMASK;
    }

    PP_TPL
    inline u32 PP_TYPE::getNextFree_(u16 type_id, u32 pos)const {
        return *(u32*)(types_[type_id].data.getItem(pos));
    }

    PP_TPL
    inline void PP_TYPE::setNextFree_(u16 type_id, u32 pos, u32 next_free_pos) {
        *(u32*)(types_[type_id].data.accItem(pos)) = next_free_pos;
    }

    PP_TPL
    inline u32 PP_TYPE::addInner_(u16 type_id, Index& index_out) {
        ASSERT(type_id < types_count_)

        u32 slot_id;
        TypeCtx& tctx = types_[type_id];
        if (tctx.first_free_pos != InvalidId()) {
            slot_id = tctx.first_free_pos;
            tctx.first_free_pos = getNextFree_(type_id, tctx.first_free_pos);
            --tctx.free_items_cnt;
        }
        else {
            slot_id = u32(tctx.versions.size());
            tctx.data.resize(slot_id+1);
            tctx.versions.push_back(FREE_BITMASK);
        }
        unsetFree_(type_id, slot_id);

        index_out.setAuxIndex(type_id);
        index_out.setVersion(tctx.versions[slot_id]);
        index_out.setIndex(slot_id);

        ++items_count_;
        return slot_id;
    }

    PP_TPL
    template <typename TP, typename T>
    inline void PP_TYPE::TypesInitLooper::f(PolyPool* pp) {
        // static
        u32 tid = Type<T, PP_TYPE>::getInternalTypeId();
        ASSERT(tid < MAX_TYPES);
        if (tid >= pp->types_count_)
            pp->types_count_ = u16(tid+1);

        pp->types_[tid].offset_to_base = calcBaseOffset<T, BaseClass>();
        pp->types_[tid].free_items_cnt = 0;
        pp->types_[tid].data.init(sizeof(T));
    }

}

#undef FREE_BITMASK
#undef PP_TPL
#undef PP_TYPE