#ifndef TIMER_H
#define TIMER_H

#include <chrono>

namespace grynca {

    class Timer {
        typedef std::chrono::high_resolution_clock HighResolutionClock;
		typedef std::chrono::duration<float, std::milli> MsType;
    public:
        Timer();
        float getElapsed();
        void reset();

    private:
        std::chrono::time_point<HighResolutionClock> start_;
    };

}

#include "Timer.inl"
#endif //TIMER_H
