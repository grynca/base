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
        for (u32 i=0; i<v.getSize(); ++i) {
            const TypeInfo& tinfo = v.getTypeInfo_(i);
            u8 *ptr = new u8(tinfo.getSize());
            tinfo.getCopyFunc()(ptr, v.get(i));
            items_.push_back(ptr);
            type_ids_.push_back(v.getTypeId(i));
        }
    }

    template <typename Domain>
    inline VVector<Domain>::~VVector() {
        clear();
    }

    template <typename Domain>
    VVector<Domain>& VVector<Domain>::operator=(const VVector& v) {
        this->~VVector();
        new (this) VVector<Domain>(v);
        return *this;
    }

    template <typename Domain>
    template <typename T, typename ... ConstrArgs>
    inline T& VVector<Domain>::add(ConstrArgs&&... args) {
        T* item = new T(std::forward<ConstrArgs>(args)...);
        items_.push_back((u8*)item);
        type_ids_.push_back(Type<T, Domain>::getTypeInfo().getId());
        return *item;
    }

    template <typename Domain>
    template <typename T, typename Base, typename ... ConstrArgs>
    inline T& VVector<Domain>::addAs(ConstrArgs&&... args) {
        T* item = new T(std::forward<ConstrArgs>(args)...);
        Base* as_base = item;
        items_.push_back((u8*)as_base);
        type_ids_.push_back(Type<T, Domain>::getTypeInfo().getId());
        return *item;
    };

    template <typename Domain>
    template <typename T>
    inline bool VVector<Domain>::remove(T& item) {
        for (u32 i=0; i<items_.size(); ++i) {
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
    void VVector<Domain>::remove(u32 id) {
        getTypeInfo_(id).getDestroyFunc()(items_[id]);
        items_.erase(items_.begin()+id);
        type_ids_.erase(type_ids_.begin()+id);
    }

    template <typename Domain>
    inline void VVector<Domain>::clear() {
        for (u32 i=0; i<items_.size(); ++i) {
            getTypeInfo_(i).getDestroyFunc()(items_[i]);
        }
        items_.clear();
        type_ids_.clear();
    }

    template <typename Domain>
    inline u32 VVector<Domain>::getSize()const {
        return (u32)items_.size();
    }

    template <typename Domain>
    inline u8* VVector<Domain>::get(u32 id) {
        return items_[id];
    }

    template <typename Domain>
    inline u8* const VVector<Domain>::get(u32 id)const {
        return items_[id];
    }

    template <typename Domain>
    inline u32 VVector<Domain>::getTypeId(u32 id)const {
        return type_ids_[id];
    }

    template <typename Domain>
    inline void VVector<Domain>::reserve(u32 size) {
        items_.reserve(size);
        type_ids_.reserve(size);
    }

    template <typename Domain>
    inline const TypeInfo& VVector<Domain>::getTypeInfo_(u32 id)const {
        return TypeInfoManager<Domain>::get(type_ids_[id]);
    }
}