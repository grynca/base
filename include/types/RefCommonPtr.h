#ifndef REFCOMMONPTR_H
#define REFCOMMONPTR_H

#include "RefCount.h"
#include "CommonPtr.h"

namespace grynca {

    // keeps refcount and destroys itself when it reaches 0
    class RefCommonPtr : public CommonPtr {
    public:
        RefCommonPtr()
                : CommonPtr(),
                  ref_count_(new RefCount())
        {}

        template <typename T>
        RefCommonPtr(T* ptr)
                : CommonPtr(ptr),
                  ref_count_(new RefCount())
        {}

        RefCommonPtr(CommonPtr& ptr)
                : CommonPtr(ptr),
                  ref_count_(new RefCount())
        {}

        RefCommonPtr(const RefCommonPtr& rptr)
                : CommonPtr((const CommonPtr&)rptr),
                  ref_count_(rptr.ref_count_)
        {
            ref_count_->ref();
        }

        template <typename T>
        RefCommonPtr(const RefPtr<T>& ptr)
            : CommonPtr(ptr.getPtr()), ref_count_(ptr.ref_count_)
        {
            ref_count_->ref();
        }

        RefCommonPtr& operator =(const RefCommonPtr& ptr) {
            unref_();
            place_ = ptr.place_;
            type_id_ = ptr.type_id_;
            ref_count_ = ptr.ref_count_;
            ref_count_->ref();
            return *this;
        }

        virtual ~RefCommonPtr() {
            unref_();
        }

    private:
        template <typename> friend class RefPtr;
        void unref_() {
            if (!ref_count_->unref()) {
                delete ref_count_;
                if (!isNull()) {
                    destroy();
                }
            }
        }

        RefCount* ref_count_;
    };
}

#endif //REFCOMMONPTR_H
