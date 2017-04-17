#ifndef TIMER_H
#define TIMER_H

#include "functions/defs.h"

namespace grynca {

    struct Freq {
        Freq();
        f64 f_;
    };

    class Clock {
    public:
        typedef f32 value_type;
    private:

        friend value_type operator-(const Clock&, const Clock&);
        friend Clock operator-(const Clock&, value_type);
        friend Clock operator+(const Clock&, value_type);
        friend bool operator<(const Clock&, const Clock&);
        friend bool operator>(const Clock&, const Clock&);
        friend bool operator<=(const Clock&, const Clock&);
        friend bool operator>=(const Clock&, const Clock&);
        friend bool operator==(const Clock&, const Clock&);
        friend bool operator!=(const Clock&, const Clock&);
    public:
        static Clock getNow();


        Clock& operator+=(value_type secs);
        Clock& operator-=(value_type secs);
    private:
        static f64 getFreq() {
            static Freq f;
            return f.f_;
        }
        i64 t_;
    };

    class Timer {
    public:
        Timer();
        Clock::value_type getElapsed()const;
        void reset();

        Clock& accStart();
    private:
        Clock start_;
    };

}

#include "Timer.inl"
#endif //TIMER_H
