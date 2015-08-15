#include "VVector.h"
#include "../Type.h"

namespace grynca {

    template <typename Domain>
    inline VVector<Domain>::VVector() {

    }

    template <typename Domain>
    inline VVector<Domain>::VVector(VVector && v) {
        items_ = v.items_;
        type_ids_ = v.type_ids_;
        v.items_.clear();
        v.type_ids_.clear();
    }

    template <typename Domain>
    inline VVector<Domain>::VVector(const VVector & v) {
        items_.reserve(v.getSize());
        type_ids_.reserve(v.getSize());
        for (uint32_t i=0; i<v.getSize(); ++i) {
            const TypeInfo& tinfo = v.getTypeInfo_(i);
            uint8_t *ptr = new uint8_t(tinfo.getSize());
            tinfo.getCopyFunc()(ptr, v.get(i));
            items_.push_back(ptr);
            type_ids_.push_back(v.getTypeId(i));
        }
    }

    template <typename Domain>
    inline VVector<Domain>::~VVector() {
        for (uint32_t i=0; i<items_.size(); ++i) {
            getTypeInfo_(i).getDestroyFunc()(items_[i]);
        }
    }

    template <typename Domain>
    VVector<Domain>& VVector<Domain>::operator=(const VVector& v) {
        this->~VVector();
        new (this) VVector<Domain>(v);
        return *this;
    }

    template <typename Domain>
    template <typename T, typename ... Args>
    inline T& VVector<Domain>::add(Args&&... args) {
        T* item = new T(std::forward<Args>(args)...);
        items_.push_back((uint8_t*)item);
        type_ids_.push_back(Type<T, Domain>::getTypeInfo().getId());
        return *item;
    }

    template <typename Domain>
    template <typename T>
    inline bool VVector<Domain>::remove(T& item) {
        for (uint32_t i=0; i<items_.size(); ++i) {
            if (items_[i] == &item) {
                getTypeInfo_(i).getDestroyFunc()(item);
                items_.erase(items_.begin()+i);
                type_ids_.erase(type_ids_.begin()+i);
                return true;
            }
        }
        return false;
    }

    template <typename Domain>
    void VVector<Domain>::remove(uint32_t id) {
        getTypeInfo_(id).getDestroyFunc()(items_[id]);
        items_.erase(items_.begin()+id);
        type_ids_.erase(type_ids_.begin()+id);
    }

    template <typename Domain>
    inline uint32_t VVector<Domain>::getSize()const {
        return (uint32_t)items_.size();
    }

    template <typename Domain>
    inline uint8_t* VVector<Domain>::get(uint32_t id) {
        return items_[id];
    }

    template <typename Domain>
    inline uint8_t* const VVector<Domain>::get(uint32_t id)const {
        return items_[id];
    }

    template <typename Domain>
    inline uint32_t VVector<Domain>::getTypeId(uint32_t id)const {
        return type_ids_[id];
    }

    template <typename Domain>
    inline void VVector<Domain>::reserve(uint32_t size) {
        items_.reserve(size);
        type_ids_.reserve(size);
    }

    template <typename Domain>
    inline const TypeInfo& VVector<Domain>::getTypeInfo_(uint32_t id)const {
        return TypeInfoManager<Domain>::get(type_ids_[id]);
    }
}