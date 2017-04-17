#ifndef SCOPEMEASURE_H
#define SCOPEMEASURE_H

#include "Timer.h"
#include "../functions/debug.h"

namespace grynca {

    class Measure {
    public:
        Measure()
         : acc_(0), loops_(0) {}

        void from() {
            t_.reset();
        }

        void to() {
            acc_ += t_.getElapsed();
            ++loops_;
        }

        f32 calcAvgDt()const {
            ASSERT(loops_);
            return acc_/loops_;
        }

        f32 calcAvgDt2()const {
            if (!loops_)
                return 0.0f;
            return acc_/loops_;
        }

        void print(const ustring& msg)const {
            std::cout << msg << ": " << std::setprecision(4) << calcAvgDt() << " sec." << std::endl;
        }

    protected:
        f32 acc_;
        u64 loops_;
        Timer t_;
    };

    class BlockMeasure : public Measure {
    public:
        BlockMeasure(const ustring& msg)
         : msg_(msg)
        {
            from();
        }

        ~BlockMeasure() {
            to();
            print(msg_);
        }

        ustring msg_;
    };
}

#endif //SCOPEMEASURE_H
