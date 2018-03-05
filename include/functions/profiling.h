#ifndef PROFILING_H
#define PROFILING_H

#include "functions/hash.h"
#include "types/Singleton.h"
#include "types/containers/fast_vector.h"
#include "types/containers/HashMap.h"
#include "types/Timer.h"
#include "ssu.h"

namespace grynca {

    class Profiling : public Singleton<Profiling> {
    private:
        struct MeasureCtx {
            MeasureCtx(const fast_vector<u32>& sp);

            f32 calcDt();
            f32 calcDt(f32 divider);

            f32 acc_time;
            u32 samples_cnt;
            fast_vector<u32> stack_path;
        };

        struct CmpMeasuresByAvg {
            bool operator()(MeasureCtx* m1, MeasureCtx* m2);
        };

        struct CmpMeasuresByAcc {
            bool operator()(MeasureCtx* m1, MeasureCtx* m2);
        };

        struct PathHasher {
            u64 operator()(const fast_vector<u32>& stack_path)const {
                return calcIntsHash(stack_path);
            }
        };

        class Measures {
        public:
            Measures();
            MeasureCtx& findMeasure(const fast_vector<u32>& path);

            u32 getCount()const;
            MeasureCtx& getMeasure(u32 id);
        private:
            HashMap<MeasureCtx, fast_vector<u32>/*stack path*/, PathHasher> measures_;
        };

        struct ProfilingBlock {
            ProfilingBlock(const ustring& name);

            ustring name;
            Timer timer;
        };
    public:
        Profiling();

        // return block id
        static u32 initBlock(const ustring &name);
    private:
        friend class ProfilingBlock;
        friend class ProfilingSample;
        friend class ProfilingSampleSimple;
        template <typename T> friend class ProfilingSampleCond;
        template <typename T> friend class ProfilingSampleFunc;

        void endCurrBlock_();

        fast_vector<ProfilingBlock> blocks_;
        // sinking path of measure context, root ctx is first,
        //   after that there can be multiple ctxs for conditional measures
        fast_vector<Measures> measure_ctxs_;
        fast_vector<u32> curr_stack_;
        fast_vector<Profiling::MeasureCtx*> tmp_print_;
        u32 longest_block_name_;
    };

    class ProfilingSample {
    public:
        ProfilingSample();

        template <typename Sorter = Profiling::CmpMeasuresByAcc>
        ustring simplePrint(f32 avg_divider = 1.0f)const;

        template <typename Sorter = Profiling::CmpMeasuresByAcc>
        void clearMeasureTimes()const;

        void getBlockInfo(ustring& name_out, u32& lvl_out)const;
    private:
        template <typename Sorter = Profiling::CmpMeasuresByAcc>
        fast_vector<Profiling::MeasureCtx*>& lazyGetMeasures_()const;

        mutable u32 sorter_tid_;
    };

    class ProfilingSampleSimple : public ProfilingSample {
    public:
        ProfilingSampleSimple(u32 block_id);
        ~ProfilingSampleSimple();
    };

    template <typename CondFunc>
    class ProfilingSampleCond : public ProfilingSample {
    public:
        ProfilingSampleCond(u32 block_id, const CondFunc& cf);
        ~ProfilingSampleCond();

    private:
        const CondFunc& cf_;
    };

    template <typename PrintFunc>
    class ProfilingSampleFunc : public ProfilingSample {
    public:
        ProfilingSampleFunc(u32 block_id, const PrintFunc& pf);
        ~ProfilingSampleFunc();
    private:
        const PrintFunc& pf_;
    };

    struct ProfilingPrinterCout {
        void operator()(const ProfilingSample& s)const { std::cout << s.simplePrint(); }
    };
}

#define MEASURE_ID_DEF(NAME) \
        u32 NAME;

#define MEASURE_ID_INIT(NAME, MSG) \
        NAME = Profiling::initBlock(MSG);

#define MEASURE_SAMPLE_C(NAME, COND_FUNC) \
        ProfilingSampleCond<decltype(COND_FUNC)> CAT(prof_sample_, __LINE__) (NAME, COND_FUNC)

#define MEASURE_SAMPLE(NAME) ProfilingSampleSimple CAT(prof_sample_, __LINE__)(NAME)

#define MEASURE_SAMPLE_F(NAME, PRINT_FUNC) \
        ProfilingSampleFunc<decltype(PRINT_FUNC)> CAT(prof_sample_, __LINE__) (NAME, PRINT_FUNC)

// Time is added only if COND_FUNC results to true
#define MEASURE_BLOCK_C(MSG, COND_FUNC) \
        static u32 CAT(prof_bl_, __LINE__) = Profiling::initBlock(MSG); \
        ProfilingSampleCond<decltype(COND_FUNC)> CAT(prof_sample_, __LINE__) (CAT(prof_bl_, __LINE__), COND_FUNC)

#define MEASURE_BLOCK(MSG) \
        static u32 CAT(prof_bl_, __LINE__) = Profiling::initBlock(MSG); \
        ProfilingSampleSimple CAT(prof_sample_, __LINE__) (CAT(prof_bl_, __LINE__))

// Prints average block loop-times on block destructor
#define MEASURE_BLOCK_F(MSG, PRINT_FUNC) \
        static u32 CAT(prof_bl_, __LINE__) = Profiling::initBlock(MSG); \
        ProfilingSampleFunc<decltype(PRINT_FUNC)> CAT(prof_sample_, __LINE__) (CAT(prof_bl_, __LINE__), PRINT_FUNC)

#if defined(PROFILE_BUILD)
#   define PROFILE_ID_DEF(NAME) MEASURE_ID_DEF(NAME)
#   define PROFILE_ID_INIT(NAME, MSG) MEASURE_ID_INIT(NAME, MSG)
#   define PROFILE_SAMPLE(NAME) MEASURE_SAMPLE(NAME)
#   define PROFILE_SAMPLE_F(NAME, PRINT_FUNC) MEASURE_SAMPLE_F(NAME, PRINT_FUNC)
#   define PROFILE_BLOCK(MSG) MEASURE_BLOCK(MSG)
#   define PROFILE_BLOCK_C(MSG, COND_FUNC) MEASURE_BLOCK_C(MSG, COND_FUNC)
#   define PROFILE_BLOCK_F(MSG, PRINT_FUNC) MEASURE_BLOCK_F(MSG, PRINT_FUNC)
#else
#   define PROFILE_ID_DEF(NAME)
#   define PROFILE_ID_INIT(NAME, MSG)
#   define PROFILE_SAMPLE(NAME)
#   define PROFILE_SAMPLE_F(NAME, PRINT_FUNC)
#   define PROFILE_BLOCK(MSG)
#   define PROFILE_BLOCK_C(MSG, COND_FUNC)
#   define PROFILE_BLOCK_F(MSG, PRINT_FUNC)
#endif

#include "profiling.inl"
#endif //PROFILING_H
