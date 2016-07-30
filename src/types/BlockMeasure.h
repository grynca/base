#ifndef SCOPEMEASURE_H
#define SCOPEMEASURE_H

#include <string>
#include <iostream>
#include "types/Timer.h"

namespace grynca {

    class Measure {
    public:
        Measure(const std::string& msg)
         : msg_(msg), counter_(0) {}

        void reset() {
            counter_ = 0;
            t_.reset();
        }

        void print() {
            std::cout << msg_ << ": " << calcDt() << " sec." << std::endl;
        }

        float calcDt() {
            if (counter_ == 0)
                return 0.0f;
            return t_.getElapsed()/counter_;
        }

        uint32_t incCounter() {  return ++counter_; };
        uint32_t getCounter() { return counter_; }

    protected:
        uint32_t counter_;
        std::string msg_;
        Timer t_;
    };

    class BlockMeasure : public Measure {
    public:
        BlockMeasure(const std::string& msg)
         : Measure(msg)
        {
            counter_ = 1;
        }

        ~BlockMeasure() {
            print();
        }
    };
}

#endif //SCOPEMEASURE_H
