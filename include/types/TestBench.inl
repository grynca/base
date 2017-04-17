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

    template <typename PrintFunc>
    inline void TestBench::runAllTests(const PrintFunc& pf) {
        for (u32 i=0; i<tests_.size(); ++i) {
            runTest(i, pf);
        }
    }

    template <typename PrintFunc>
    inline void TestBench::runTest(u32 test_id, const PrintFunc& pf) {
        if (tests_[test_id].measure_id != InvalidId()) {
            MEASURE_SAMPLE_P(tests_[test_id].measure_id, pf);
            tests_[test_id].func(cfg_.accData(tests_[test_id].cfg_section.cpp_str()));
        }
        else
            tests_[test_id].func(cfg_.accData(tests_[test_id].cfg_section.cpp_str()));
    }

    template <typename T>
    inline T SDLTest::loadCfgValue(Config::ConfigSectionMap& cfg, const std::string& name, T def_val) {
        T rslt = tryGet(cfg, name, CfgValue(def_val));
#ifdef PROFILE_BUILD
        std::cout << name << ": " << rslt << std::endl;
#endif
        return rslt;
    }

#if USE_SDL2 == 1
    inline SDLTestBench::SDLTestBench(u32 width, u32 height, bool accelerated)
     : window_(NULL), renderer_(NULL), running_test_(InvalidId()),
       update_measure_print_freq_(10.0f), debug_overlay_flags_(u8(-1))
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
            while (running_test_ != InvalidId()) {
                tests_[running_test_].func(cfg_.accData(tests_[running_test_].cfg_section.cpp_str()));
            }
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

    inline void SDLTestBench::setUpdateMeasurePrintFreq(f32 freq) {
        update_measure_print_freq_ = freq;
    }

    inline f32 SDLTestBench::getUpdateMeasurePrintFreq()const {
        return update_measure_print_freq_;
    }

    inline void SDLTestBench::setShowFps(bool val) {
        debug_overlay_flags_ = SET_BITV(debug_overlay_flags_, 0, val);
    }

    inline bool SDLTestBench::getShowFps()const {
        return GET_BIT(debug_overlay_flags_, 0);
    }

    inline void SDLTestBench::setShowProfileMeasures(bool val) {
        debug_overlay_flags_ = SET_BITV(debug_overlay_flags_, 1, val);
    }

    inline bool SDLTestBench::getShowProfileMeasures()const {
        return GET_BIT(debug_overlay_flags_, 1);
    }

    inline SDL_Window* SDLTestBench::getWindow()const {
        return window_;
    }

    inline SDL_Renderer* SDLTestBench::getRenderer()const {
        return renderer_;
    }

    template<typename TObject>
    inline void SDLTestBench::RunnerFunctor<TObject>::f(TObject* obj, Config::ConfigSectionMap& cfg) {
        SDLTestBench& tb = obj->accTestBench();
        obj->init(cfg);
        Timer timer;

        struct {
            f32 prev_time;
            f32 update_print_dt;
            f32 last_print_time;
            f32 now;
            u32 frames;

            std::string fps_label;
            std::string measure_label;
        } ctx;

        ctx.prev_time = 0;
        ctx.update_print_dt = 1.0f/tb.getUpdateMeasurePrintFreq();
        ctx.last_print_time = 0.0f;
        ctx.now = 0.0f;
        ctx.frames = 0;
        ctx.fps_label = "FPS: \n";
        ctx.measure_label = "";
        u32 prev_test = tb.getRunningTest();

        auto update_print_f = [&ctx](const ProfilingSample& s) {
            f32 dt = ctx.now - ctx.last_print_time;
            if (dt > ctx.update_print_dt) {
                //std::cout << s.printWithPerc() << std::endl;
                ctx.last_print_time += dt;
                f32 fps = f32(ctx.frames)/dt;
                f32 frame_time = 1000.0f/fps;
                ctx.fps_label = ssu::formatA("FPS: %.3f (%.3fms)", fps, frame_time);
                ctx.frames = 0;
                ctx.measure_label = s.printWithPerc().cpp_str();
            }
        };

        F8x8::SDL2Text fps_lbl(tb.renderer_);
        u32 fps_lbl_y = 0;
        F8x8::SDL2Text measure_lbl(tb.renderer_);
        u32 measure_lbl_y = 0;

        mainLoop([&]() {
            return prev_test == tb.getRunningTest();
        },
        [&]() {
            PROFILE_SAMPLE_P(tb.getTestInfo(tb.getRunningTest()).measure_id, update_print_f);
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
                            if (tb.debug_overlay_flags_)
                                tb.debug_overlay_flags_ = 0;
                            else
                                tb.debug_overlay_flags_ = u8(-1);
                            break;
                        }
                        else if (evt.key.keysym.sym == SDLK_LEFT) {
                            tb.runTest((tb.getRunningTest()-1) % tb.getTestsCount());
                            break;
                        }
                        else if (evt.key.keysym.sym == SDLK_RIGHT) {
                            tb.runTest((tb.getRunningTest()+1) % tb.getTestsCount());
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
            if (tb.getShowFps()) {
                fps_lbl.setText(ctx.fps_label);
                h += 5;
                fps_lbl_y = h;
                h += fps_lbl.getHeight();
                w = std::max(w, fps_lbl.getWidth()+10);
            }
            if (tb.getShowProfileMeasures()) {
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
                if (tb.getShowFps()) {
                    fps_lbl.draw(0, 0, 0, 255, 5, fps_lbl_y);
                }
                if (tb.getShowProfileMeasures()) {
                    measure_lbl.draw(0, 0, 0, 255, 5, measure_lbl_y);
                }
            }


            SDLCall(SDL_RenderPresent(tb.renderer_));
        });

        obj->close();
    }
#endif
}