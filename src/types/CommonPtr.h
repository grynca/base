#ifndef COMMONPTR_H
#define COMMONPTR_H

#include "Type.h"

namespace grynca {
    // only pointer that remembers its destructor
    //  its owner must still call .destroy()
    class CommonPtr {
    public:
        CommonPtr() : place_(NULL), destructor_(NULL) {}

        template <typename T>
        CommonPtr(T* ptr)
                : place_(ptr),
                  destructor_(Type<T>::destroy)
        {}

        CommonPtr(const CommonPtr& ptr)
                : place_(ptr.place_),
                  destructor_(ptr.destructor_)
        {}

        virtual ~CommonPtr() {}

        template <typename T>
        T* getAs() {
            return (T*)place_;
        }

        void destroy() {
            ASSERT(!isNull(), "Cant destroy null CommonPtr.");
            destructor_(place_);
        }

        bool isNull() { return place_==NULL; }

    protected:
        void *place_;
        DestroyFunc destructor_;
    };
}

#endif //COMMONPTR_H
