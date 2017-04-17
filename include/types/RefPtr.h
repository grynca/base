#ifndef REFPTR_H
#define REFPTR_H

#include "RefCount.h"
#include "Type.h"
#include <cassert>

namespace grynca {

    template <typename T>
    class RefPtr
    {
    public:
        // todo: interoperability with common ptrs

        RefPtr()
                : ptr_(NULL), ref_count_(new RefCount())
        {}

        RefPtr(void* ptr)
                : ptr_((T*)ptr), ref_count_(new RefCount())
        {}

        RefPtr(T& t)
            : RefPtr(&t)
        {}

        RefPtr(const RefPtr& ptr)
                : ptr_(ptr.ptr_), ref_count_(ptr.ref_count_)
        {
            ref_count_->ref();
        }

        RefPtr& operator =(const RefPtr& ptr) {
            unref_();
            ptr_ = ptr.ptr_;
            ref_count_ = ptr.ref_count_;
            ref_count_->ref();
            return *this;
        }

        virtual ~RefPtr() {
            unref_();
        }

        T* getPtr() { return ptr_; }
        T& get() {
            ASSERT(ptr_);
            return *ptr_;
        }

        T* operator->() {
            ASSERT(ptr_);
            return ptr_;
        }

        T const* operator->()const {
            ASSERT(ptr_);
            return ptr_;
        }

        bool operator()()const {
            return ptr_ != NULL;
        }

    protected:
        void unref_() {
            if (!ref_count_->unref()) {
                delete ref_count_;
                if (ptr_) {
                    delete ptr_;
                }
            }
        }

        T * ptr_;
        RefCount* ref_count_;
    };
}

#endif //REFPTR_H
