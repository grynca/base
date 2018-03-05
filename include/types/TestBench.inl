#include "TestBench.h"
#include "Timer.h"
#include "SimpleFont.h"

namespace grynca {

    inline void TestBenchBase::initClArgs(int argc, char** argv) {
        argc_ = argc;
        argv_ = argv;
    }

    inline int TestBenchBase::getArgc() {
        return argc_;
    }

    inline char** TestBenchBase::getArgv() {
        return argv_;
    }

    inline Config& TestBenchBase::accLastTestConfig() {
        if (!tests_.empty()) {
            cfg_.setCurrentSectionName(tests_.back().cfg_section.cpp_str());
        }
        return cfg_;
    }

    inline Config& TestBenchBase::accTestConfig(u32 test_id) {
        ASSERT(test_id < tests_.size());
        cfg_.setCurrentSectionName(tests_[test_id].cfg_section.cpp_str());
        return cfg_;
    }

    inline const TestBenchBase::TestInfo& TestBenchBase::getTestInfo(u32 test_id)const {
        return tests_[test_id];
    }

    inline TestBenchBase::TestBenchBase()
     : argc_(0), argv_(&empty_cmdline_)
    {}

    template<typename Functor, typename TObject>
    inline void TestBench::addTestWithoutMeasure(const ustring& name, TObject *obj, const ustring& cfg_section) {
        u32 measure_id = InvalidId();
        tests_.push_back({name, TestFunc::create<Functor>(obj), cfg_section, measure_id});
    }


    template<typename Functor, typename TObject>
    inline u32 TestBench::addTest(const ustring& name, TObject *obj) {
        return addTest<Functor>(name, name, obj);
    };

    template<typename Functor, typename TObject>
    inline u32 TestBench::addTest(const ustring& name, const ustring& cfg_section, TObject *obj) {
        MEASURE_ID_DEF(measure_id);
        MEASURE_ID_INIT(measure_id, name);
        u32 test_id = u32(tests_.size());
        tests_.push_back({name, TestFunc::create<Functor>(obj), cfg_section, measure_id});
        return test_id;
    }

    template <typename ProfilingFunc>
    inline void TestBench::runAllTests(const ProfilingFunc& pf) {
        for (u32 i=0; i<tests_.size(); ++i) {
            runTest(i, pf);
        }
    }

    template <typename ProfilingFunc>
    inline void TestBench::runTest(u32 test_id, const ProfilingFunc& pf) {
        if (tests_[test_id].measure_id != InvalidId()) {
            MEASURE_SAMPLE_F(tests_[test_id].measure_id, pf);
            tests_[test_id].func(cfg_.accData(tests_[test_id].cfg_section.cpp_str()));
        }
        else
            tests_[test_id].func(cfg_.accData(tests_[test_id].cfg_section.cpp_str()));
    }

#if USE_SDL2 == 1

    template <typename T>
    inline T SDLTest::loadCfgValue(Config::ConfigSectionMap& cfg, const std::string& name, T def_val) {
        T rslt = tryGet(cfg, name, CfgValue(def_val));
#ifdef PROFILE_BUILD
        std::cout << name << ": " << rslt << std::endl;
#endif
        return rslt;
    }

    inline SDLTestBench::SDLTestBench(u32 width, u32 height, bool accelerated)
     : window_(NULL), renderer_(NULL), running_test_(InvalidId()),
       update_print_frames_(10), overlay_print_mode_(u8(-1))
    {
        i32 rslt = SDLCall(SDL_Init(SDL_INIT_VIDEO));
        if (rslt != 0) {
            throw SDL_Exception("SDL_Init:");
        }
        window_ = SDLCall(SDL_CreateWindow("Grynca TestBench", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0));
        if (!window_) {
            throw SDL_Exception("SDL_CreateWindow:");
        }
        if (accelerated) {
            renderer_ = SDLCall(SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED));
        }
        if (!renderer_) {
            // try software renderer
            renderer_ = SDLCall(SDL_CreateRenderer(window_, -1, SDL_RENDERER_SOFTWARE));
            if (!renderer_) {
                throw SDL_Exception("SDL_CreateRenderer:");
            }
        }
        SDLCall(SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND));
    }

    inline SDLTestBench::~SDLTestBench() {
        std::cout << "quiting SDL" << std::endl;
        SDLCall(SDL_Quit());
    }

    template<typename TObject>
    inline u32 SDLTestBench::addTest(const ustring& name, TObject *obj) {
        return addTest(name, name, obj);
    }

    template<typename TObject>
    inline u32 SDLTestBench::addTest(const ustring& name, const ustring& cfg_section, TObject *obj) {
        MEASURE_ID_DEF(measure_id);
        MEASURE_ID_INIT(measure_id, name);
        obj->tb_ = this;
        u32 test_id = u32(tests_.size());
        tests_.push_back({name, TestFunc::create<RunnerFunctor<TObject> >(obj), cfg_section, measure_id});
        return test_id;
    }

    inline void SDLTestBench::runTest(u32 test_id) {
        bool running = (running_test_ != InvalidId());
        running_test_ = test_id;

        if (!running) {
            runCurrentTest_();
        }
    }

    inline void SDLTestBench::stopTest() {
        running_test_ = InvalidId();
    }

    inline u32 SDLTestBench::getRunningTest()const {
        return running_test_;
    }

    inline u32 SDLTestBench::getTestsCount() const {
        return u32(tests_.size());
    }

    inline void SDLTestBench::setUpdateMeasurePrintFreq(u32 frames) {
        update_print_frames_ = frames;
    }

    inline u32 SDLTestBench::getUpdateMeasurePrintFreq()const {
        return update_print_frames_;
    }

    inline void SDLTestBench::setOverlayPrintMode(u32 opm) {
        overlay_print_mode_ = opm;
    }

    inline u32 SDLTestBench::getOverlayPrintMode()const {
        return overlay_print_mode_;
    }

    inline SDL_Window* SDLTestBench::getWindow()const {
        return window_;
    }

    inline SDL_Renderer* SDLTestBench::getRenderer()const {
        return renderer_;
    }

    inline void SDLTestBench::runCurrentTest_() {
        tests_[running_test_].func(cfg_.accData(tests_[running_test_].cfg_section.cpp_str()));
    }

    template<typename TObject>
    inline void SDLTestBench::RunnerFunctor<TObject>::f(TObject* obj, Config::ConfigSectionMap& cfg) {
        SDLTestBench& tb = obj->accTestBench();
        obj->init(cfg);
        Timer timer;

        struct {
            f32 prev_time;
            f32 last_print_time;
            f32 now;
            u32 update_print_frames;
            u32 frames;

            std::string fps_label;
            std::string measure_label;
        } ctx;

        ctx.prev_time = 0;
        ctx.update_print_frames = tb.getUpdateMeasurePrintFreq();
        ctx.last_print_time = 0.0f;
        ctx.now = 0.0f;
        ctx.frames = 0;
        ctx.fps_label = "FPS: \n";
        ctx.measure_label = "";
        u32 prev_test = tb.getRunningTest();

#ifdef PROFILE_BUILD
        auto update_print_f = [&ctx](const ProfilingSample& s) {
            if (ctx.frames == ctx.update_print_frames) {
                f32 dt = ctx.now - ctx.last_print_time;
                ctx.last_print_time += dt;
                f32 fps = f32(ctx.frames)/dt;
                f32 frame_time = 1000.0f/fps;
                ctx.fps_label = ssu::formatA("FPS: %.3f (%.3fms)", fps, frame_time);
                // TODO: barvickama oddelit levely nejak + moznost prepinat mezi acc a avg
                ctx.measure_label = s.simplePrint(ctx.frames).cpp_str();
                s.clearMeasureTimes();
                ctx.frames = 0;
            }
        };
#else
        auto update_print_f = [&ctx]() {
            if (ctx.frames == ctx.update_print_frames) {
                f32 dt = ctx.now - ctx.last_print_time;
                ctx.last_print_time += dt;
                f32 fps = f32(ctx.frames)/dt;
                f32 frame_time = 1000.0f/fps;
                ctx.fps_label = ssu::formatA("FPS: %.3f (%.3fms)", fps, frame_time);
                ctx.frames = 0;
            }
        };
#endif
        F8x8::SDL2Text fps_lbl(tb.renderer_);
        fps_lbl.setColor(0, 0, 0, 255);
        u32 fps_lbl_y = 0;
        F8x8::SDL2Text measure_lbl(tb.renderer_);
        measure_lbl.setColor(0, 0, 0, 255);
        u32 measure_lbl_y = 0;

        auto cond_f = [&]() {
            return prev_test == tb.getRunningTest();
        };

        auto close_f = [&]() {
            obj->close();
            // run next test
            if (tb.getRunningTest() != InvalidId()) {
                tb.runCurrentTest_();
            }
        };

        auto loop_f = [&]() {
            PROFILE_SAMPLE_F(tb.getTestInfo(tb.getRunningTest()).measure_id, update_print_f);
#ifndef PROFILE_BUILD
            update_print_f();
#endif
            prev_test = tb.getRunningTest();

            SDL_Event evt;
            while (SDL_PollEvent(&evt)) {
                switch (evt.type) {
                    case (SDL_QUIT):
                        obj->accTestBench().stopTest();
                        break;
                    case (SDL_KEYUP):
                        if (evt.key.keysym.sym == SDLK_ESCAPE) {
                            obj->accTestBench().stopTest();
                            break;
                        }
                        else if (evt.key.keysym.sym == SDLK_o) {
                            // toggle debug overlay
                            if (tb.overlay_print_mode_) {
                                tb.overlap_print_mode_stored_ = tb.overlay_print_mode_;
                                tb.overlay_print_mode_ = opmNone;
                            }
                            else {
                                tb.overlay_print_mode_ = tb.overlap_print_mode_stored_;
                            }
                            break;
                        }
                        else if (evt.key.keysym.sym == SDLK_LEFT) {
                            tb.runTest(wrap(tb.getRunningTest()-1, tb.getTestsCount()));
                            break;
                        }
                        else if (evt.key.keysym.sym == SDLK_RIGHT) {
                            tb.runTest(wrap(tb.getRunningTest()+1, tb.getTestsCount()));
                            break;
                        }
                        // dont break
                    default:
                        obj->handleEvent(evt);
                }
            }
            ctx.now = timer.getElapsed();
            f32 dt = ctx.now - ctx.prev_time;
            ctx.prev_time += dt;
            ++ctx.frames;

            obj->update(tb.renderer_, dt);

            u32 h = 0;
            u32 w = 0;
            // print debug overlay
            u32 opm = tb.getOverlayPrintMode();
            if (opm) {
                if (opm&opmFPS) {
                    fps_lbl.setText(ctx.fps_label);
                    h += 5;
                    fps_lbl_y = h;
                    h += fps_lbl.getHeight();
                    w = std::max(w, fps_lbl.getWidth()+10);
                }
                if (opm&opmProfileMeasures) {
                    h += 5;
                    measure_lbl_y = h;
                    measure_lbl.setText(ctx.measure_label);
                    h += measure_lbl.getHeight();
                    w = std::max(w, measure_lbl.getWidth()+10);
                }

                if (w && h) {
                    SDL_Rect overlay_rect{0, 0, i32(w), i32(h)};
                    SDL_SetRenderDrawColor(tb.renderer_, 255, 255, 255, 200);
                    SDLCall(SDL_RenderFillRect(tb.renderer_, &overlay_rect));
                    if (opm&opmFPS) {
                        fps_lbl.draw(5, fps_lbl_y);
                    }
                    if (opm&opmProfileMeasures) {
                        measure_lbl.draw(5, measure_lbl_y);
                    }
                }
            }

            SDLCall(SDL_RenderPresent(tb.renderer_));
        };

        mainLoop(cond_f, loop_f, close_f);
    }
#endif
}