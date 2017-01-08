#ifndef SCOPEMEASURE_H
#define SCOPEMEASURE_H

#include <string>
#include <iostream>
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

        void print(const std::string& msg)const {
            std::cout << msg << ": " << calcAvgDt() << " sec." << std::endl;
        }

    protected:
        f32 acc_;
        u64 loops_;
        Timer t_;
    };

    class BlockMeasure : public Measure {
    public:
        BlockMeasure(const std::string& msg)
         : msg_(msg)
        {
            from();
        }

        ~BlockMeasure() {
            to();
            print(msg_);
        }

        std::string msg_;
    };
}

#endif //SCOPEMEASURE_H
