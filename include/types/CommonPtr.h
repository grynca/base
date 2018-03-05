#ifndef COMMONPTR_H
#define COMMONPTR_H

#include "Type.h"

namespace grynca {
    // only pointer that remembers its destructor
    //  its owner must still call .destroy()
    class CommonPtr {
    public:
        CommonPtr() : place_(NULL), type_id_(u32(-1)) {}

        template <typename T>
        CommonPtr(T* ptr)
          : place_((u8*)ptr), type_id_(Type<T>::getInternalTypeId())
        {}

        CommonPtr(const CommonPtr& ptr)
          : place_(ptr.place_), type_id_(ptr.type_id_)
        {}

        virtual ~CommonPtr() {}

        const u8* getPtr()const {
            return place_;
        }

        u8* accPtr() {
            return place_;
        }

        template <typename T>
        T* accAs() {
            return (T*)place_;
        }

        template <typename T>
        const T* getAs()const {
            return (const T*)place_;
        }

        void destroy() {
            ASSERT_M(!isNull(), "Cant destroy null CommonPtr.");
            InternalTypes<>::getInfo(type_id_).getDestroyFunc()(place_);
        }

        u32 getTypeId()const {
            return type_id_;
        }

        const TypeInfo& getTypeInfo()const {
            return InternalTypes<>::getInfo(type_id_);
        }

        bool isNull()const { return place_==NULL; }

    protected:
        u8 *place_;
        u32 type_id_;
    };
}

#endif //COMMONPTR_H
