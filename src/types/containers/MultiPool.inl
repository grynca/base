#include "MultiPool.h"

#define MP_TPL template <uint32_t MAX_COMPS, typename Domain = void>
#define MP_TYPE MultiPool<MAX_COMPS, Domain>

namespace grynca {

    MP_TPL
    inline MP_TYPE::~MultiPool() {
        clear();
    }

    MP_TPL
    template <typename TP>
    inline void MP_TYPE::initComponents() {
        TP::callOnTypes<CompsInitLooper>(component_types_);
    }

    MP_TPL
    inline Index MP_TYPE::add() {
        uint32_t slot_id;
        if (!free_slots_.empty()) {
            slot_id = free_slots_.back();
            free_slots_.pop_back();
        }
        else {
            slot_id = size();
            for (uint32_t i=0; i<component_types_.size(); ++i) {
                uint32_t comp_size = getTypeInfo(i).getSize();
                data_[i].resize((slot_id+1)*comp_size);
            }
            versions_.resize(slot_id+1, (1<<15));
        }
        unsetFree_(slot_id);
        return Index(slot_id, versions_[slot_id]);
    }

    MP_TPL
    inline void MP_TYPE::removeAtPos(uint32_t pos) {
        ASSERT(!isFree(pos));
        free_slots_.push_back(pos);
        ++versions_[pos];
        setFree_(pos);
    }

    MP_TPL
    inline void MP_TYPE::remove(Index index) {
        ASSERT(isValidIndex(index));
        free_slots_.push_back(index.getIndex());
        ++versions_[index.getIndex()];
        setFree_(index.getIndex());
    }

    MP_TPL
    inline void MP_TYPE::reserve(size_t count) {
        versions_.reserve(count);
        for (uint32_t i=0; i<component_types_.size(); ++i) {
            uint32_t comp_size = getTypeInfo(i).getSize();
            data_[i].reserve(comp_size*count);
        }
    }

    MP_TPL
    inline bool MP_TYPE::isFree(uint32_t pos)const {
        ASSERT(pos < versions_.size());
        return (versions_[pos] & uint16_t(1<<15)) != 0;
    }

    MP_TPL
    inline uint8_t* MP_TYPE::get(Index index, uint32_t component_id) {
        uint32_t comp_size = getTypeInfo(component_id).getSize();
        return &data_[component_id][comp_size*component_id];
    }

    MP_TPL
    inline const uint8_t* MP_TYPE::get(Index index, uint32_t component_id)const {
        uint32_t comp_size = getTypeInfo(component_id).getSize();
        return &data_[component_id][comp_size*component_id];
    }

    MP_TPL
    inline uint8_t* MP_TYPE::getAtPos(uint32_t pos, uint32_t component_id) {
        ASSERT(!isFree(pos));
        uint32_t comp_size = getTypeInfo(component_id).getSize();
        return &data_[component_id][pos*comp_size];
    }

    MP_TPL
    inline const uint8_t* MP_TYPE::getAtPos(uint32_t pos, uint32_t component_id)const {
        ASSERT(!isFree(pos));
        uint32_t comp_size = getTypeInfo(component_id).getSize();
        return &data_[component_id][pos*comp_size];
    }

    MP_TPL
    inline Index MP_TYPE::getIndexForPos(uint32_t pos) {
        return Index(pos, versions_[pos]);
    }

    MP_TPL
    inline void MP_TYPE::getIndexForPos2(uint32_t pos, Index& index_out) {
        index_out.setIndex(pos);
        index_out.setVersion(versions_[pos]);
    }

    MP_TPL
    inline const TypeInfo& MP_TYPE::getTypeInfo(uint32_t component_id)const {
        return InternalTypes<Domain>::getInfo(component_types_[component_id]);
    }

    MP_TPL
    inline uint8_t* MP_TYPE::getCompsData(uint32_t component_id) {
        return &data_[component_id][0];
    }

    MP_TPL
    inline bool MP_TYPE::isValidIndex(Index index)const {
        if (index.getIndex() >= size())
            return false;
        return versions_[index.getIndex()] == index.getVersion();
    }

    MP_TPL
    inline uint32_t MP_TYPE::size()const {
        return versions_.size();
    }

    MP_TPL
    inline uint32_t MP_TYPE::occupiedSize()const {
        return size()-free_slots_.size();
    }

    MP_TPL
    inline bool MP_TYPE::empty()const {
        return data_[0].empty();
    }

    MP_TPL
    inline void MP_TYPE::clear() {
        versions_.clear();
        for (uint32_t i=0; i<component_types_.size(); ++i) {
            DestroyFunc destructor = getTypeInfo(i).getDestroyFunc();
            for (uint32_t j=0; j<size(); ++j) {
                uint8_t* ptr = getAtPos(j, i);
                if (ptr) {
                    destructor(ptr);
                }
            }
            data_[i].clear();
        }
    }

    MP_TPL
    inline float MP_TYPE::getMemoryWaste() {
        return ((float)free_slots_.size())/size();
    }

    MP_TPL
    inline void MP_TYPE::setFree_(uint32_t pos) {
        ASSERT(pos < versions_.size());
        versions_[pos] |= 1<<15;
    }

    MP_TPL
    inline void MP_TYPE::unsetFree_(uint32_t pos) {
        ASSERT(pos < versions_.size());
        versions_[pos] &= ~(1<<15);
    }

}

#undef MP_TPL
#undef MP_TYPE