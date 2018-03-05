#ifndef EMSC_UTILS_H
#define EMSC_UTILS_H

#include "common.h"

namespace grynca {

    // CondFunc is evaluated at start of each cycle, if it returns false loop ends
    template <typename CondFunc, typename LoopFunc, typename CloseFunc = void(*)() >
    inline void mainLoop(const CondFunc& cond_f, const LoopFunc& loop_f, const CloseFunc& close_f = []{}) {
#ifdef WEB
        auto ff = [&cond_f, &loop_f, &close_f]() {
            if (!cond_f()) {
                emscripten_cancel_main_loop();
                close_f();
                return;
            }
            loop_f();
        };

        emscripten_set_main_loop_arg(dispatchFunction<decltype(ff)>, &ff,
                                     0 /* use browser's framerate */,
                                     1 /* simulate infinite loop */);
#else
        while (cond_f()) {
            loop_f();
        }
        close_f();
#endif
    }

}

#endif //EMSC_UTILS_H
