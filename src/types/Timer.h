#ifndef TIMER_H
#define TIMER_H

#include <chrono>

namespace grynca {

    class Clock {
        typedef std::chrono::high_resolution_clock HighResolutionClock;
        typedef std::chrono::duration<double, std::milli> MsType;

        friend float operator-(const Clock&, const Clock&);
        friend Clock operator-(const Clock&, float);
        friend Clock operator+(const Clock&, float);
        friend bool operator<(const Clock&, const Clock&);
        friend bool operator>(const Clock&, const Clock&);
        friend bool operator<=(const Clock&, const Clock&);
        friend bool operator>=(const Clock&, const Clock&);
        friend bool operator==(const Clock&, const Clock&);
        friend bool operator!=(const Clock&, const Clock&);
    public:
        static Clock getNow();


        Clock& operator+=(float secs);
        Clock& operator-=(float secs);
    private:
        std::chrono::time_point<HighResolutionClock, MsType> clock_;
    };

    class Timer {
    public:
        Timer();
        float getElapsed();
        void reset();

    private:
        Clock start_;
    };

}

#include "Timer.inl"
#endif //TIMER_H
