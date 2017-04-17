#ifndef EMSC_UTILS_H
#define EMSC_UTILS_H

#include "common.h"

namespace grynca {

    // CondFunc is evaluated at start of each cycle, if it returns false loop ends
    template <typename CondFunc, typename LoopFunc>
    inline void mainLoop(const CondFunc& cf, const LoopFunc& lf) {
#ifdef WEB
        auto ff = [&cf, &lf]() {
            if (!cf()) {
                emscripten_cancel_main_loop();
                return;
            }
            lf();
        };

        emscripten_set_main_loop_arg(dispatchFunction<decltype(ff)>, &ff,
                                     0 /* use browser's framerate */,
                                     1 /* simulate infinite loop */);
#else
        while (cf()) {
            lf();
        }
#endif
    }

}

#endif //EMSC_UTILS_H
