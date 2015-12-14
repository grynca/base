#ifndef REFCOUNT_H
#define REFCOUNT_H

#include <atomic>

namespace grynca {
    class RefCount {
    public:
        RefCount() : ref_count_(1) { }
        RefCount(int rc) : ref_count_(rc) { }
        RefCount(const RefCount& rc) :ref_count_(int(rc.ref_count_)) {}

        RefCount& operator=(RefCount&& rc) {
            ref_count_ = int(rc.ref_count_);
            return *this;
        }

        RefCount& operator=(const grynca::RefCount& rc) {
            ref_count_ = int(rc.ref_count_);
            return *this;
        }

        void ref() {
            ++ref_count_;
        }

        bool unref() {
            --ref_count_;
            return ref_count_;
        }

        int get() {
            return ref_count_;
        }
    private:
        std::atomic<int> ref_count_;
    };
}

#endif //REFCOUNT_H
