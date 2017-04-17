#include "MultiPool.h"

#define MP_TPL template <u32 MAX_COMPS, typename Domain>
#define MP_TYPE MultiPool<MAX_COMPS, Domain>

namespace grynca {

    MP_TPL
    inline MP_TYPE::~MultiPool() {
        clear();
    }

    MP_TPL
    template <typename TP>
    inline void MP_TYPE::initComponents() {
        ASSERT(TP::getTypesCount() <= MAX_COMPS);
        TP::template callOnTypes<CompsInitLooper>(component_types_);
        for (u32 i=0; i<component_types_.size(); ++i) {
            data_[i].init(u32(getTypeInfo(i).getSize()));
        }
    }

    MP_TPL
    inline Index MP_TYPE::add() {
        u32 slot_id;
        if (!free_slots_.empty()) {
            slot_id = free_slots_.back();
            free_slots_.pop_back();
        }
        else {
            slot_id = size();
            for (u32 i=0; i<component_types_.size(); ++i) {
                data_[i].resize(slot_id+1);
            }
            versions_.resize(slot_id+1, (1<<15));
        }
        unsetFree_(slot_id);
        return Index(slot_id, versions_[slot_id]);
    }

    MP_TPL
    inline Index MP_TYPE::addAndConstruct() {
        Index id = add();
        for (u32 i=0; i<component_types_.size(); ++i) {
            const TypeInfo& ti = getTypeInfo(i);
            u8* comp_ptr = data_[i].accItem(id.getIndex());
            ti.getDefaultConstr()(comp_ptr);
        }
        return id;
    }


    MP_TPL
    inline void MP_TYPE::removeAtPos(u32 pos) {
        ASSERT(!isFree(pos));
        // call components' destructors
        for (u32 i=0; i<component_types_.size(); ++i) {
            const TypeInfo& ti = getTypeInfo(i);
            u8* comp_ptr = data_[i].accItem(pos);
            ti.getDestroyFunc()(comp_ptr);
        }
        free_slots_.push_back(pos);
        ++versions_[pos];
        setFree_(pos);
    }

    MP_TPL
    inline void MP_TYPE::remove(Index index) {
        ASSERT(isValidIndex(index));
        removeAtPos(index.getIndex());
    }

    MP_TPL
    inline void MP_TYPE::reserve(size_t count) {
        versions_.reserve(count);
        for (u32 i=0; i<component_types_.size(); ++i) {
            data_[i].reserve(count);
        }
    }

    MP_TPL
    inline bool MP_TYPE::isFree(u32 pos)const {
        ASSERT(pos < versions_.size());
        return (versions_[pos] & u16(1<<15)) != 0;
    }

    MP_TPL
    inline u32 MP_TYPE::getComponentsCount()const {
        return u32(component_types_.size());
    }

    MP_TPL
    inline u8* MP_TYPE::get(Index index, u32 component_id) {
        return data_[component_id].accItem(index.getIndex());
    }

    MP_TPL
    inline const u8* MP_TYPE::get(Index index, u32 component_id)const {
        return data_[component_id].getItem(index.getIndex());
    }

    MP_TPL
    inline u8* MP_TYPE::getAtPos(u32 pos, u32 component_id) {
        ASSERT(!isFree(pos));
        return data_[component_id].accItem(pos);
    }

    MP_TPL
    inline const u8* MP_TYPE::getAtPos(u32 pos, u32 component_id)const {
        ASSERT(!isFree(pos));
        return data_[component_id].getItem(pos);

    }

    MP_TPL
    inline Index MP_TYPE::getIndexForPos(u32 pos) {
        return Index(pos, versions_[pos]);
    }

    MP_TPL
    inline void MP_TYPE::getIndexForPos2(u32 pos, Index& index_out) {
        index_out.setIndex(pos);
        index_out.setVersion(versions_[pos]);
    }

    MP_TPL
    inline const TypeInfo& MP_TYPE::getTypeInfo(u32 component_id)const {
        return InternalTypes<Domain>::getInfo(component_types_[component_id]);
    }

//    MP_TPL
//    inline u8* MP_TYPE::getCompsData(u32 component_id) {
//        return &data_[component_id][0];
//    }

    MP_TPL
    inline bool MP_TYPE::isValidIndex(Index index)const {
        if (index.getIndex() >= size())
            return false;
        return versions_[index.getIndex()] == index.getVersion();
    }

    MP_TPL
    inline u32 MP_TYPE::size()const {
        return u32(versions_.size());
    }

    MP_TPL
    inline u32 MP_TYPE::occupiedSize()const {
        return size()-u32(free_slots_.size());
    }

    MP_TPL
    inline bool MP_TYPE::empty()const {
        return data_[0].empty();
    }

    MP_TPL
    inline void MP_TYPE::clear() {
        versions_.clear();
        for (u32 i=0; i<component_types_.size(); ++i) {
            DestroyFunc destructor = getTypeInfo(i).getDestroyFunc();
            for (u32 j=0; j<size(); ++j) {
                u8* ptr = getAtPos(j, i);
                if (ptr) {
                    destructor(ptr);
                }
            }
            data_[i].clear();
        }
    }

    MP_TPL
    inline f32 MP_TYPE::getMemoryWaste() {
        return ((f32)free_slots_.size())/size();
    }

    MP_TPL
    inline void MP_TYPE::setFree_(u32 pos) {
        ASSERT(pos < versions_.size());
        versions_[pos] |= 1<<15;
    }

    MP_TPL
    inline void MP_TYPE::unsetFree_(u32 pos) {
        ASSERT(pos < versions_.size());
        versions_[pos] &= ~(1<<15);
    }

}

#undef MP_TPL
#undef MP_TYPE