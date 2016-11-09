#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include "../functions/defs.h"

namespace grynca {

#ifdef _WIN32
    struct Freq {
        Freq();
        f64 f_;
    };
#endif

    class Clock {
        typedef std::chrono::high_resolution_clock HighResolutionClock;
        typedef std::chrono::duration<double> Dur;
        typedef std::chrono::time_point<HighResolutionClock, Dur> Time;

        friend f32 operator-(const Clock&, const Clock&);
        friend Clock operator-(const Clock&, f32);
        friend Clock operator+(const Clock&, f32);
        friend bool operator<(const Clock&, const Clock&);
        friend bool operator>(const Clock&, const Clock&);
        friend bool operator<=(const Clock&, const Clock&);
        friend bool operator>=(const Clock&, const Clock&);
        friend bool operator==(const Clock&, const Clock&);
        friend bool operator!=(const Clock&, const Clock&);
    public:
        static Clock getNow();


        Clock& operator+=(f32 secs);
        Clock& operator-=(f32 secs);
    private:
#ifdef _WIN32
        static f64 getFreq() {
            static Freq f;
            return f.f_;
        }
        i64 t_;
#endif
        //Time t_;
    };

    class Timer {
    public:
        Timer();
        f32 getElapsed();
        void reset();

    private:
        Clock start_;
    };

}

#include "Timer.inl"
#endif //TIMER_H
