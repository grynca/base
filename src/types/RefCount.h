#ifndef REFCOUNT_H
#define REFCOUNT_H

#include <atomic>

namespace grynca {
    class RefCount {
    public:
        RefCount() : ref_count_(1) { }

        void ref() {
            ++ref_count_;
        }

        bool unref() {
            --ref_count_;
            return ref_count_;
        }

    private:
        std::atomic<int> ref_count_;
    };
}

#endif //REFCOUNT_H
