#ifndef PROFILING_H
#define PROFILING_H

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

            f32 acc_time;
            u32 samples_cnt;
            fast_vector<u32> stack_path;
        };

        struct CmpMeasures {
            bool operator()(MeasureCtx* m1, MeasureCtx* m2) {
                u32 s = u32(std::min(m1->stack_path.size(), m2->stack_path.size()));
                for (u32 i=0; i<s; ++i) {
                    if (m1->stack_path[i] < m2->stack_path[i])
                        return true;
                    else if (m1->stack_path[i] > m2->stack_path[i])
                        return false;
                }
                return m1->stack_path.size() < m2->stack_path.size();
            }
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
        friend class ProfilingSample;
        friend class ProfilingSampleSimple;
        template <typename T> friend class ProfilingSampleCond;
        template <typename T> friend class ProfilingSamplePrint;

        void endCurrBlock_();

        fast_vector<ProfilingBlock> blocks_;
        fast_vector<Measures> measure_ctxs_;
        fast_vector<u32> curr_stack_;
    };

    class ProfilingSample {
    public:
        ustring print()const;
        ustring printWithPerc()const;
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
        CondFunc cf_;
    };

    template <typename PrintFunc>
    class ProfilingSamplePrint : public ProfilingSample {
    public:
        ProfilingSamplePrint(u32 block_id, const PrintFunc& pf);
        ~ProfilingSamplePrint();
    private:
        PrintFunc pf_;
    };

    struct ProfilingPrinterCout {
        void operator()(const ProfilingSample& s) { std::cout << s.print(); }
    };

    //  prints accumulated block times and percentage distribution on each level
    struct ProfilingPrinterPercCout {
        void operator()(const ProfilingSample& s) { std::cout << s.printWithPerc(); }
    };
}

#define MEASURE_ID_DEF(NAME) \
        u32 NAME;

#define MEASURE_ID_INIT(NAME, MSG) \
        NAME = Profiling::initBlock(MSG);

#define MEASURE_SAMPLE_C(NAME, COND_FUNC) \
        auto CAT(prof_func, __LINE__) = COND_FUNC; \
        ProfilingSampleCond<decltype(CAT(prof_func, __LINE__))> CAT(prof_sample_, __LINE__) (NAME, CAT(prof_func, __LINE__))

#define MEASURE_SAMPLE(NAME) ProfilingSampleSimple CAT(prof_sample_, __LINE__)(NAME)

#define MEASURE_SAMPLE_P(NAME, PRINT_FUNC) \
        auto CAT(print_func, __LINE__) = PRINT_FUNC; \
        ProfilingSamplePrint<decltype(CAT(print_func, __LINE__))> CAT(prof_sample_, __LINE__) (NAME, CAT(print_func, __LINE__))

// Time is added only if COND_FUNC results to true
#define MEASURE_BLOCK_C(MSG, COND_FUNC) \
        static u32 CAT(prof_bl_, __LINE__) = Profiling::initBlock(MSG); \
        auto CAT(prof_func, __LINE__) = COND_FUNC; \
        ProfilingSampleCond<decltype(CAT(prof_func, __LINE__))> CAT(prof_sample_, __LINE__) (CAT(prof_bl_, __LINE__), CAT(prof_func, __LINE__))

#define MEASURE_BLOCK(MSG) \
        static u32 CAT(prof_bl_, __LINE__) = Profiling::initBlock(MSG); \
        ProfilingSampleSimple CAT(prof_sample_, __LINE__) (CAT(prof_bl_, __LINE__))

// Prints average block loop-times on block destructor
#define MEASURE_BLOCK_P(MSG, PRINT_FUNC) \
        static u32 CAT(prof_bl_, __LINE__) = Profiling::initBlock(MSG); \
        auto CAT(print_func, __LINE__) = PRINT_FUNC; \
        ProfilingSamplePrint<decltype(CAT(print_func, __LINE__))> CAT(prof_sample_, __LINE__) (CAT(prof_bl_, __LINE__), CAT(print_func, __LINE__))

#if defined(PROFILE_BUILD) || defined(PROFILE_IN_RELEASE)
#   define PROFILE_ID_DEF(NAME) MEASURE_ID_DEF(NAME)
#   define PROFILE_ID_INIT(NAME, MSG) MEASURE_ID_INIT(NAME, MSG)
#   define PROFILE_SAMPLE(NAME) MEASURE_SAMPLE(NAME)
#   define PROFILE_SAMPLE_P(NAME, PRINT_FUNC) MEASURE_SAMPLE_P(NAME, PRINT_FUNC)
#   define PROFILE_BLOCK(MSG) MEASURE_BLOCK(MSG)
#   define PROFILE_BLOCK_C(MSG, COND_FUNC) MEASURE_BLOCK_C(MSG, COND_FUNC)
#   define PROFILE_BLOCK_P(MSG, PRINT_FUNC) MEASURE_BLOCK_P(MSG, PRINT_FUNC)
#else
#   define PROFILE_ID_DEF(NAME)
#   define PROFILE_ID_INIT(NAME, MSG)
#   define PROFILE_SAMPLE(NAME)
#   define PROFILE_SAMPLE_P(NAME, PRINT_FUNC)
#   define PROFILE_BLOCK(MSG)
#   define PROFILE_BLOCK_C(MSG, COND_FUNC)
#   define PROFILE_BLOCK_P(MSG, PRINT_FUNC)
#endif

#include "profiling.inl"
#endif //PROFILING_H
