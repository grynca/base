#ifndef SCOPEMEASURE_H
#define SCOPEMEASURE_H

#include <string>
#include <iostream>
#include <time.h>

namespace grynca {
    class BlockMeasure {
    public:
        BlockMeasure(const std::string& msg)
                : msg_(msg), t_(clock()), divider_(1.0f) {}

        ~BlockMeasure() {
            float dt = (float)(clock()-t_)/divider_/CLOCKS_PER_SEC;
            std::cout << msg_ << ": " << dt << " sec." << std::endl;
        }

        void setDivider(float d) { divider_ = d; }

    private:
        std::string msg_;
        clock_t t_;
        float divider_;
    };
}

#endif //SCOPEMEASURE_H
