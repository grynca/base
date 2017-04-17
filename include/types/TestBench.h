#ifndef TESTBENCH_H
#define TESTBENCH_H

#include "containers/fast_vector.h"
#include "Singleton.h"
#include "Config.h"
#include "ObjFunc.h"
#include "../functions/profiling.h"

namespace grynca {

    class TestBenchBase {
        char* empty_cmdline_ = const_cast<char*>("");
    public:
        typedef ObjFunc<void(Config::ConfigSectionMap&)> TestFunc;

        struct TestInfo {
            ustring test_name;
            ObjFunc<void(Config::ConfigSectionMap&)> func;
            ustring cfg_section;
            u32 measure_id;
        };

    public:
        void initClArgs(int argc, char** argv);
        int getArgc();
        char** getArgv();

        Config& accLastTestConfig();
        Config& accTestConfig(u32 test_id);

        const TestBenchBase::TestInfo& getTestInfo(u32 test_id)const;
    protected:
        TestBenchBase();

        int argc_;
        char** argv_;
        Config cfg_;

        fast_vector<TestInfo> tests_;
    };

    class TestBench : public TestBenchBase {
    public:
        template<typename Functor, typename TObject = void>
        void addTestWithoutMeasure(const ustring& name, TObject *obj = NULL, const ustring& cfg_section = "");

        // returns test id
        template<typename Functor, typename TObject = void>
        u32 addTest(const ustring& name, TObject *obj = NULL);     // cfg_section same name as test
        template<typename Functor, typename TObject = void>
        u32 addTest(const ustring& name, const ustring& cfg_section, TObject *obj = NULL);

        template <typename PrintFunc = ProfilingPrinterCout>
        void runAllTests(const PrintFunc& pf = PrintFunc());
        template <typename PrintFunc = ProfilingPrinterCout>
        void runTest(u32 test_id, const PrintFunc& pf = PrintFunc());
    };

    class TestBenchSton : public Singleton<TestBench> {};

#if USE_SDL2 == 1

    // fw
    class SDLTestBench;

    // Both SDLTest & SDLTextBench must remain in constant place in memory (linked with pointers)

    class SDLTest {
    public:
        SDLTest() : tb_(NULL) {}

        SDLTestBench& accTestBench() { return * tb_;}
    protected:
        template <typename T>
        static T loadCfgValue(Config::ConfigSectionMap& cfg, const std::string& name, T def_val) ;
    private:
        friend class SDLTestBench;

        SDLTestBench* tb_;
    };

    class SDLTestBench : public TestBenchBase {
    public:
        SDLTestBench(u32 width = 1024, u32 height = 768, bool accelerated = true);
        ~SDLTestBench();

        // returns test id
        template<typename TObject>
        u32 addTest(const ustring& name, TObject *obj);     // cfg_section same name as test
        template<typename TObject>
        u32 addTest(const ustring& name, const ustring& cfg_section, TObject *obj);

        void runTest(u32 test_id);
        void stopTest();

        u32 getRunningTest()const;       // InvalidId() when not running
        u32 getTestsCount()const;

        void setUpdateMeasurePrintFreq(f32 freq);
        f32 getUpdateMeasurePrintFreq()const;

        void setShowFps(bool val);
        bool getShowFps()const;
        void setShowProfileMeasures(bool val);
        bool getShowProfileMeasures()const;

        SDL_Window* getWindow()const;
        SDL_Renderer* getRenderer()const;
    private:
        template<typename TObject>
        struct RunnerFunctor {
            static void f(TObject* obj, Config::ConfigSectionMap& cfg);
        };

        SDL_Window* window_;
        SDL_Renderer* renderer_;
        u32 running_test_;
        f32 update_measure_print_freq_;
        u8 debug_overlay_flags_;
    };

    class SDLTestBenchSton : public Singleton<SDLTestBench> {};
#endif
}

#include "TestBench.inl"
#endif //TESTBENCH_H
