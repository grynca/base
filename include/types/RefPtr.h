#ifndef REFPTR_H
#define REFPTR_H

#include "RefCount.h"
#include "CommonPtr.h"
#include "Type.h"
#include <cassert>

namespace grynca {

    // fw
    class RefCommonPtr;

    template<typename T>
    class RefPtr {
    public:

        RefPtr()
            : ptr_(NULL), ref_count_(new RefCount()) {}

        RefPtr(void *ptr)
            : ptr_((T *) ptr), ref_count_(new RefCount()) {}

        RefPtr(const CommonPtr &ptr)
            : ptr_((T *) ptr.getPtr()), ref_count_(new RefCount()) {}

        RefPtr(const RefCommonPtr &ptr);

        RefPtr(T &t)
            : RefPtr(&t) {}

        RefPtr(const RefPtr &ptr)
            : ptr_(ptr.ptr_), ref_count_(ptr.ref_count_)
        {
            ref_count_->ref();
        }

        RefPtr &operator=(const RefPtr &ptr) {
            unref_();
            ptr_ = ptr.ptr_;
            ref_count_ = ptr.ref_count_;
            ref_count_->ref();
            return *this;
        }

        virtual ~RefPtr() {
            unref_();
        }

        const T *getPtr() const { return ptr_; }

        T *&accPtr() { return ptr_; }

        T &get() {
            ASSERT(ptr_);
            return *ptr_;
        }

        T *operator->() {
            ASSERT(ptr_);
            return ptr_;
        }

        T const *operator->() const {
            ASSERT(ptr_);
            return ptr_;
        }

        bool operator()() const {
            return ptr_ != NULL;
        }

    protected:
        friend class RefCommonPtr;

        void unref_() {
            if (!ref_count_->unref()) {
                delete ref_count_;
                if (ptr_) {
                    delete ptr_;
                }
            }
        }

        T *ptr_;
        RefCount *ref_count_;
    };
}

#include "RefCommonPtr.h"

namespace grynca {

    template<typename T>
    inline RefPtr<T>::RefPtr(const RefCommonPtr& ptr)
     : ptr_((T*)ptr.getPtr()), ref_count_((RefCount*)(&ptr))
    {
        ref_count_->ref();
    }
}

#endif //REFPTR_H
