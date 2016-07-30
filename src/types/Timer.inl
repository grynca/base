#include "Timer.h"

namespace grynca {

    inline Clock Clock::getNow() {
        Clock c;
        c.clock_ = HighResolutionClock::now();
        return c;
    }

    inline Clock& Clock::operator+=(float secs) {
        clock_  = clock_ + MsType(secs*1000);
        return *this;
    }

    inline Clock& Clock::operator-=(float secs) {
        clock_ = clock_ - MsType(secs*1000);
        return *this;
    }

    inline float operator-(const Clock& c1, const Clock& c2) {
        Clock::MsType dur = c1.clock_ - c2.clock_;
        return (float)dur.count()*0.001f;
    }

    inline Clock operator-(const Clock& c, float secs) {
        Clock c2;
        c2.clock_ = c.clock_ - Clock::MsType(secs*1000);
        return c2;
    }

    inline Clock operator+(const Clock& c, float secs) {
        Clock c2;
        c2.clock_ = c.clock_ + Clock::MsType(secs*1000);
        return c2;
    }

    inline bool operator<(const Clock& c1, const Clock& c2) {
        return c1.clock_ < c2.clock_;
    }

    inline bool operator>(const Clock& c1, const Clock& c2) {
        return c1.clock_ > c2.clock_;
    }

    inline bool operator<=(const Clock& c1, const Clock& c2) {
        return c1.clock_ <= c2.clock_;
    }

    inline bool operator>=(const Clock& c1, const Clock& c2) {
        return c1.clock_ >= c2.clock_;
    }

    inline bool operator==(const Clock& c1, const Clock& c2) {
        return c1.clock_ == c2.clock_;
    }

    inline bool operator!=(const Clock& c1, const Clock& c2) {
        return c1.clock_ != c2.clock_;
    }

    inline Timer::Timer()
     : start_(Clock::getNow())
    {}

    inline float Timer::getElapsed() {
        return Clock::getNow() - start_;
    }

    inline void Timer::reset() {
        start_ = Clock::getNow();
    }
}