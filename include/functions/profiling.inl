#include "profiling.h"
#include <iostream>
#include <iomanip>

namespace grynca {

    inline Profiling::MeasureCtx::MeasureCtx(const fast_vector<u32>& sp)
     : acc_time(0.0f), samples_cnt(0), stack_path(sp)
    {}


    inline Profiling::Measures::Measures() {
        measures_.reserve(100);
    }

    inline Profiling::MeasureCtx& Profiling::Measures::findMeasure(const fast_vector<u32>& path) {
        bool was_added;
        Profiling::MeasureCtx* path_measure = measures_.findOrAddItem(path, was_added);
        if (was_added) {
            new (path_measure) Profiling::MeasureCtx(path);
        }
        return *path_measure;
    }

    inline u32 Profiling::Measures::getCount()const {
        return measures_.getItemsCount();
    }

    inline Profiling::MeasureCtx& Profiling::Measures::getMeasure(u32 id) {
        return measures_.accItem(id);
    }

    inline Profiling::ProfilingBlock::ProfilingBlock(const ustring& name)
     : name(name)
    {
        Profiling& p = Profiling::getRef();
        u32 nl = u32(name.length());
        if (p.longest_block_name_ < nl) {
            p.longest_block_name_ = nl;
        }
    }

    inline f32 Profiling::MeasureCtx::calcDt() {
        if (!samples_cnt)
            return 0.0f;
        return acc_time/samples_cnt;
    }

    inline f32 Profiling::MeasureCtx::calcDt(f32 divider) {
        if (!samples_cnt || !divider)
            return 0.0f;
        return acc_time/(samples_cnt*divider);
    }

    inline bool Profiling::CmpMeasuresByAvg::operator()(MeasureCtx* m1, MeasureCtx* m2) {
        u32 s1 = u32(m1->stack_path.size());
        u32 s2 = u32(m2->stack_path.size());
        u32 s(std::min(s1, s2));

        u32 i;
        for (i=0; i<s-1; ++i) {
            if (m1->stack_path[i] < m2->stack_path[i])
                return true;
            else if (m1->stack_path[i] > m2->stack_path[i])
                return false;
        }

        if (s1 == s2) {
            return m1->calcDt() > m2->calcDt();
        }

        if (m1->stack_path[i] < m2->stack_path[i])
            return true;
        else if (m1->stack_path[i] > m2->stack_path[i])
            return false;

        return (s1 < s2);
    }

    inline bool Profiling::CmpMeasuresByAcc::operator()(MeasureCtx* m1, MeasureCtx* m2) {
        u32 s1 = u32(m1->stack_path.size());
        u32 s2 = u32(m2->stack_path.size());
        u32 s(std::min(s1, s2));

        u32 i;
        for (i=0; i<s-1; ++i) {
            if (m1->stack_path[i] < m2->stack_path[i])
                return true;
            else if (m1->stack_path[i] > m2->stack_path[i])
                return false;
        }

        if (s1 == s2) {
            return m1->acc_time > m2->acc_time;
        }

        if (m1->stack_path[i] < m2->stack_path[i])
            return true;
        else if (m1->stack_path[i] > m2->stack_path[i])
            return false;

        return (s1 < s2);
    }

    inline Profiling::Profiling()
     : longest_block_name_(0)
    {
        measure_ctxs_.push_back();
    }

    inline u32 Profiling::initBlock(const ustring &name) {
        // static
        static Profiling& me = get();
        u32 i = 0;
        for (; i<me.blocks_.size(); ++i) {
            if (me.blocks_[i].name == name)
                break;
        }
        if (i == me.blocks_.size())
            me.blocks_.emplace_back(name);
        return i;
    }

    inline void Profiling::endCurrBlock_() {
        Timer& t = blocks_[curr_stack_.back()].timer;
        f32 elapsed = t.getElapsed();
        Profiling::MeasureCtx& m = measure_ctxs_.back().findMeasure(curr_stack_);
        ++m.samples_cnt;
        m.acc_time += elapsed;
    }

    inline ProfilingSample::ProfilingSample()
     : sorter_tid_(InvalidId())
    {}

    template <typename Sorter>
    inline ustring ProfilingSample::simplePrint(f32 avg_divider)const {
        auto& measures = lazyGetMeasures_();

        std::stringstream ss;
        u32 base_lvl;
        ustring my_blockname;
        getBlockInfo(my_blockname, base_lvl);

        ss << my_blockname << ":" << std::endl;

        if (measures.empty())
            return std::move(ss.str());

        Profiling& p = Profiling::getRef();

        u32 i = 0;
        u32 lvl = u32(measures[i]->stack_path.size()-1);
        Profiling::MeasureCtx* mctx = measures[i];
        u32 left_pad_size = lvl-base_lvl;
        ustring left_pad = ssu::spacesA(left_pad_size);

        u32 max_lvl = u32(measures.back()->stack_path.size());
        f32 name_col_size = max_lvl + p.longest_block_name_;

        f32 avg_divider_i = 1.0f/avg_divider;
        while (1) {
            Profiling::ProfilingBlock& block = p.blocks_[mctx->stack_path.back()];
            f32 name_size = block.name.length();
            i32 right_pad_size = i32(std::ceil(name_col_size - name_size - left_pad_size));
            ASSERT(right_pad_size >= 0);

            ss << left_pad << block.name << ssu::spacesA(u32(right_pad_size))
               << "| acc=" << FLOAT_TO_STR(mctx->acc_time*avg_divider_i, 11, 7)
               << " | avg=" << FLOAT_TO_STR(mctx->calcDt(avg_divider), 11, 7)
               << std::endl;

            ++i;
            if (i>=measures.size())
                break;
            mctx = measures[i];
            u32 next_lvl = u32(mctx->stack_path.size())-1;
            bool lvl_changed = (lvl != next_lvl);
            if (lvl_changed) {
                lvl = next_lvl;
                left_pad_size = lvl-base_lvl;
                left_pad = ssu::spaces(left_pad_size);
            }
        }
        return std::move(ss.str());
    }

    template <typename Sorter>
    inline void ProfilingSample::clearMeasureTimes()const {
        auto& measures = lazyGetMeasures_();
        for (u32 i=0; i < measures.size(); ++i) {
            measures[i]->acc_time = 0.0f;
            measures[i]->samples_cnt = 0;
        }
    }

    inline void ProfilingSample::getBlockInfo(ustring& name_out, u32& lvl_out)const {
        Profiling& p = Profiling::getRef();
        lvl_out = u32(p.curr_stack_.size());
        name_out = p.blocks_[p.curr_stack_.back()].name;
    }

    template <typename Sorter>
    inline fast_vector<Profiling::MeasureCtx*>& ProfilingSample::lazyGetMeasures_()const {
        u32 sorter_tid = Type<Sorter, ProfilingSample>::getInternalTypeId();
        if (sorter_tid_ == sorter_tid) {
            // already sorted by this sorter
            return Profiling::getRef().tmp_print_;
        }

        sorter_tid_ = sorter_tid;

        Profiling& p = Profiling::getRef();
        fast_vector<Profiling::MeasureCtx*>& measures = p.tmp_print_;
        measures.clear();
        measures.reserve(p.blocks_.size()*2);

        for (u32 i=0; i<p.measure_ctxs_.back().getCount(); ++i) {
            Profiling::MeasureCtx& m = p.measure_ctxs_.back().getMeasure(i);
            if (!isSmallerPrefix(p.curr_stack_, m.stack_path))
                continue;
            measures.push_back(&m);
        }

        std::sort(measures.begin(), measures.end(), Sorter());
        return measures;
    }

    inline ProfilingSampleSimple::ProfilingSampleSimple(u32 block_id) {
        Profiling &p = Profiling::getRef();
        p.curr_stack_.push_back(block_id);
        p.blocks_[block_id].timer.reset();
    }

    inline ProfilingSampleSimple::~ProfilingSampleSimple() {
        Profiling& p = Profiling::getRef();
        p.endCurrBlock_();
        p.curr_stack_.pop_back();
    }

    template <typename CondFunc>
    inline ProfilingSampleCond<CondFunc>::ProfilingSampleCond(u32 block_id, const CondFunc& cf)
     : cf_(cf)
    {
        Profiling& p = Profiling::getRef();
        p.curr_stack_.push_back(block_id);
        // add new measures ctx
        p.measure_ctxs_.push_back();
        p.blocks_[block_id].timer.reset();
    }

    template <typename CondFunc>
    inline ProfilingSampleCond<CondFunc>::~ProfilingSampleCond() {
        Profiling& p = Profiling::getRef();
        Timer& t = p.blocks_[p.curr_stack_.back()].timer;
        f32 elapsed = t.getElapsed();
        if (cf_(elapsed)) {
            // condition is fulfilled -> transfer times to parent ctx
            Profiling::Measures& mctx = p.measure_ctxs_.back();
            Profiling::Measures& parent_mctx = p.measure_ctxs_[p.measure_ctxs_.size()-2];
            for (u32 i=0; i<mctx.getCount(); ++i) {
                Profiling::MeasureCtx& src_m = mctx.getMeasure(i);
                Profiling::MeasureCtx& dst_m = parent_mctx.findMeasure(src_m.stack_path);
                dst_m.acc_time += src_m.acc_time;
                dst_m.samples_cnt += src_m.samples_cnt;
            }
            p.measure_ctxs_.pop_back();
            p.endCurrBlock_();
        }
        else {
            p.measure_ctxs_.pop_back();
        }
        p.curr_stack_.pop_back();
    }

    template <typename PrintFunc>
    inline ProfilingSampleFunc<PrintFunc>::ProfilingSampleFunc(u32 block_id, const PrintFunc& pf)
      : pf_(pf)
    {
        Profiling& p = Profiling::getRef();
        p.curr_stack_.push_back(block_id);
        p.blocks_[block_id].timer.reset();
    }

    template <typename PrintFunc>
    inline ProfilingSampleFunc<PrintFunc>::~ProfilingSampleFunc() {
        Profiling& p = Profiling::getRef();
        p.endCurrBlock_();
        pf_(*this);
        p.curr_stack_.pop_back();
    }
}