#include "Timer.h"

namespace grynca {
    inline Timer::Timer() {
        reset();
    }

    inline float Timer::getElapsed() {
        return std::chrono::duration_cast<MsType>(HighResolutionClock::now() - start_).count()*0.001f;
    }

    inline void Timer::reset() {
        start_ = HighResolutionClock::now();
    }
}