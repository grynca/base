#include "Timer.h"
#ifdef WEB
#   include <emscripten/emscripten.h>
#elif _WIN32
#   include <windows.h>
#else
#   include <time.h>
#endif

namespace grynca {
    inline Freq::Freq() {
#ifdef _WIN32
        LARGE_INTEGER li;
        QueryPerformanceFrequency(&li);
        f_ = f64(li.QuadPart);
#else
        f_ = f64(1e9);
#endif
    }

    inline Clock Clock::getNow() {
        Clock c;
#ifdef WEB
        f64 ms = emscripten_get_now();
        c.t_ = i64(ms*1e6);      // make ns from ms
#elif _WIN32
        LARGE_INTEGER li;
        QueryPerformanceCounter(&li);
        c.t_ = li.QuadPart;
#else
        struct timespec t;
        clock_gettime(CLOCK_MONOTONIC, &t);
        c.t_ = t.tv_sec*1000000000 + t.tv_nsec;
#endif
        return c;
    }

    inline Clock& Clock::operator+=(f32 secs) {
        t_ += i64(secs*getFreq());
        return *this;
    }

    inline Clock& Clock::operator-=(f32 secs) {
        t_ -= i64(secs*getFreq());
        return *this;
    }

    inline f32 operator-(const Clock& c1, const Clock& c2) {
        return f32((c1.t_ - c2.t_)/Clock::getFreq());
    }

    inline Clock operator-(const Clock& c, f32 secs) {
        Clock c2;
        c2.t_ = c.t_ - i64(secs*Clock::getFreq());
        return c2;
    }

    inline Clock operator+(const Clock& c, f32 secs) {
        Clock c2;
        c2.t_ = c.t_ + i64(secs*Clock::getFreq());
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