#include "VVector2.h"
#include "../Type.h"

namespace grynca {

    template <typename TP>
    inline VVector2<TP>::VVector2() {
    }

    template <typename TP>
    inline VVector2<TP>::VVector2(VVector2 && v) {
        items_ = v.items_;
        type_ids_ = v.type_ids_;
        v.items_.clear();
        v.type_ids_.clear();
    }

    template <typename TP>
    inline VVector2<TP>::VVector2(const VVector2 & v) {
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

    template <typename TP>
    inline VVector2<TP>::~VVector2() {
        for (u32 i=0; i<items_.size(); ++i) {
            getTypeInfo_(i).getDestroyFunc()(items_[i]);
        }
    }

    template <typename TP>
    VVector2<TP>& VVector2<TP>::operator=(const VVector2& v) {
        this->~VVector2();
        new (this) VVector2<TP>(v);
        return *this;
    }

    template <typename TP>
    template <typename T, typename ... ConstrArgs>
    inline T& VVector2<TP>::add(ConstrArgs&&... args) {
        T* item = new T(std::forward<ConstrArgs>(args)...);
        items_.push_back((u8*)item);
        type_ids_.push_back(TP::template pos<T>());
        return *item;
    }

    template <typename TP>
    template <typename T, typename Base, typename ... ConstrArgs>
    inline T& VVector2<TP>::addAs(ConstrArgs&&... args) {
        T* item = new T(std::forward<ConstrArgs>(args)...);
        Base* as_base = item;
        items_.push_back((u8*)as_base);
        type_ids_.push_back(TP::template pos<T>());
        return *item;
    };

    template <typename TP>
    template <typename T>
    inline bool VVector2<TP>::remove(T& item) {
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

    template <typename TP>
    void VVector2<TP>::remove(u32 id) {
        getTypeInfo_(id).getDestroyFunc()(items_[id]);
        items_.erase(items_.begin()+id);
        type_ids_.erase(type_ids_.begin()+id);
    }

    template <typename TP>
    inline u32 VVector2<TP>::getSize()const {
        return (u32)items_.size();
    }

    template <typename TP>
    inline u8* VVector2<TP>::get(u32 id) {
        return items_[id];
    }

    template <typename TP>
    inline u8* const VVector2<TP>::get(u32 id)const {
        return items_[id];
    }

    template <typename TP>
    inline u32 VVector2<TP>::getTypeId(u32 id)const {
        return type_ids_[id];
    }

    template <typename TP>
    inline void VVector2<TP>::reserve(u32 size) {
        items_.reserve(size);
        type_ids_.reserve(size);
    }

    template <typename TP>
    inline const TypeInfo& VVector2<TP>::getTypeInfo_(u32 id)const {
        return TP::getTypeInfo(type_ids_[id]);
    }
}