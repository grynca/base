#include "Timer.h"
#ifdef _WIN32
#   include "windows.h"
#endif

namespace grynca {

    inline Freq::Freq() {
        LARGE_INTEGER li;
        QueryPerformanceFrequency(&li);
        f_ = f64(li.QuadPart);
    }

    inline Clock Clock::getNow() {
        Clock c;
        LARGE_INTEGER li;
        QueryPerformanceCounter(&li);
        c.t_ = li.QuadPart;
        return c;
    }

    inline Clock& Clock::operator+=(f32 secs) {
        t_ += LONGLONG(secs*getFreq());
        return *this;
    }

    inline Clock& Clock::operator-=(f32 secs) {
        t_ -= LONGLONG(secs*getFreq());
        return *this;
    }

    inline f32 operator-(const Clock& c1, const Clock& c2) {
        return f32((c1.t_ - c2.t_)/Clock::getFreq());
    }

    inline Clock operator-(const Clock& c, f32 secs) {
        Clock c2;
        c2.t_ = c.t_ - LONGLONG(secs*Clock::getFreq());
        return c2;
    }

    inline Clock operator+(const Clock& c, f32 secs) {
        Clock c2;
        c2.t_ = c.t_ + LONGLONG(secs*Clock::getFreq());
        return c2;
    }

    inline bool operator<(const Clock& c1, const Clock& c2) {
        return c1.t_ < c2.t_;
    }

    inline bool operator>(const Clock& c1, const Clock& c2) {
        return c1.t_ > c2.t_;
    }

    inline bool operator<=(const Clock& c1, const Clock& c2) {
        return c1.t_ <= c2.t_;
    }

    inline bool operator>=(const Clock& c1, const Clock& c2) {
        return c1.t_ >= c2.t_;
    }

    inline bool operator==(const Clock& c1, const Clock& c2) {
        return c1.t_ == c2.t_;
    }

    inline bool operator!=(const Clock& c1, const Clock& c2) {
        return c1.t_ != c2.t_;
    }

    inline Timer::Timer()
     : start_(Clock::getNow())
    {}

    inline f32 Timer::getElapsed() {
        return Clock::getNow() - start_;
    }

    inline void Timer::reset() {
        start_ = Clock::getNow();
    }
}