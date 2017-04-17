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
        return measures_.getItem(id);
    }

    inline Profiling::ProfilingBlock::ProfilingBlock(const ustring& name)
     : name(name)
    {}

    inline f32 Profiling::MeasureCtx::calcDt() {
        if (!samples_cnt)
            return 0.0f;
        return acc_time/samples_cnt;
    }

    inline Profiling::Profiling() {
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

    inline ustring ProfilingSample::print()const {
        std::stringstream ss;
        Profiling& p = Profiling::getRef();

        fast_vector<Profiling::MeasureCtx*> measures;
        measures.reserve(p.blocks_.size()*2);

        u32 base_lvl = 0;
        if (!p.curr_stack_.empty()) {
            ss << p.blocks_[p.curr_stack_.back()].name << ":" << std::endl;
            base_lvl = u32(p.curr_stack_.size());
        }

        for (u32 i=0; i<p.measure_ctxs_.back().getCount(); ++i) {
            Profiling::MeasureCtx& m = p.measure_ctxs_.back().getMeasure(i);
            if (!isSmallerPrefix(p.curr_stack_, m.stack_path))
                continue;
            measures.push_back(&m);
        }
        std::sort(measures.begin(), measures.end(), Profiling::CmpMeasures());


        for (u32 i=0; i<measures.size(); ++i) {
            Profiling::MeasureCtx* mctx = measures[i];
            Profiling::ProfilingBlock& block = p.blocks_[mctx->stack_path.back()];

            ss << ssu::spaces(u32(mctx->stack_path.size()-base_lvl)*2) << block.name << ": cnt="  << mctx->samples_cnt << std::setprecision(4) << ", acc=" << mctx->acc_time
               << " sec, avg=" << mctx->calcDt() << " sec" << std::endl;
        }
        return std::move(ss.str());
    }

    inline ustring ProfilingSample::printWithPerc()const {
        std::stringstream ss;

        Profiling& p = Profiling::getRef();
        fast_vector<Profiling::MeasureCtx*> measures;
        measures.reserve(p.blocks_.size()*2);

        u32 base_lvl = 0;
        if (!p.curr_stack_.empty()) {
            ss << p.blocks_[p.curr_stack_.back()].name << ":" << std::endl;
            base_lvl = u32(p.curr_stack_.size());
        }

        for (u32 i=0; i<p.measure_ctxs_.back().getCount(); ++i) {
            Profiling::MeasureCtx& m = p.measure_ctxs_.back().getMeasure(i);
            if (!isSmallerPrefix(p.curr_stack_, m.stack_path))
                continue;
            measures.push_back(&m);
        }
        std::sort(measures.begin(), measures.end(), Profiling::CmpMeasures());

        f32 parent_acc_time = 0.0f;
        u32 prev_lvl = InvalidId();
        for (u32 i=0; i<measures.size(); ++i) {
            Profiling::MeasureCtx* mctx = measures[i];

            u32 lvl = u32(mctx->stack_path.size())-1;
            bool lvl_changed = (lvl != prev_lvl);
            if (lvl_changed) {
                bool has_parent = mctx->stack_path.size()>1;
                if (has_parent) {
                    u32 chid = mctx->stack_path.back();
                    mctx->stack_path.pop_back();
                    Profiling::MeasureCtx& parent_measure = p.measure_ctxs_.back().findMeasure(mctx->stack_path);
                    parent_acc_time = parent_measure.acc_time;
                    mctx->stack_path.push_back(chid);
                }
                else {
                    parent_acc_time = 0.0f;
                }
                prev_lvl = lvl;
            }

            Profiling::ProfilingBlock& block = p.blocks_[mctx->stack_path.back()];

            ss << ssu::spaces(u32(mctx->stack_path.size()-base_lvl)*2) << block.name << ": cnt="  << mctx->samples_cnt << std::setprecision(4) << ", acc=" << mctx->acc_time
               << " sec, avg=" << mctx->calcDt() << " sec";
            if (parent_acc_time!=0.0f) {
                ss << " (" << ssu::printPerc(mctx->acc_time/parent_acc_time) << "%)";
            }
            ss << std::endl;
        }
        return std::move(ss.str());
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
        p.blocks_[block_id].timer.reset();
        // add new measures ctx
        p.measure_ctxs_.push_back();
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
    inline ProfilingSamplePrint<PrintFunc>::ProfilingSamplePrint(u32 block_id, const PrintFunc& pf)
      : pf_(pf)
    {
        Profiling& p = Profiling::getRef();
        p.curr_stack_.push_back(block_id);
        p.blocks_[block_id].timer.reset();
    }

    template <typename PrintFunc>
    inline ProfilingSamplePrint<PrintFunc>::~ProfilingSamplePrint() {
        Profiling& p = Profiling::getRef();
        p.endCurrBlock_();
        pf_(*this);
        p.curr_stack_.pop_back();
    }
}