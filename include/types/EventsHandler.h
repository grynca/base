#ifndef EVENTS_HANDLER_H
#define EVENTS_HANDLER_H

#include "functions/defs.h"
#include "functions/sort_utils.h"
#include "functions/hash.h"
#include "types/containers/HashMap.h"
#include "types/containers/SortedArray.h"
#include "types/RefCommonPtr.h"
#include "types/ObjFunc.h"

namespace grynca {

    struct EventCallbackId {
        EventCallbackId() : event_id(InvalidId()) {}
        EventCallbackId(u32 ev_id, u32 cb_id) : event_id(ev_id), callback_id(cb_id) {}

        bool isValid()const { return event_id!=InvalidId(); }
        void invalidate() { event_id = InvalidId(); }

        u32 event_id;
        u32 callback_id;
    };


    template<typename> class EventCb;
    template <typename TReturn, typename ...Args>
    class EventCb<TReturn(Args...)> {
    public:
        typedef EventCb<TReturn(Args...)> MyType;
        typedef ObjFunc<TReturn(Args...)> CbFunc;
    public:
        EventCb(const CbFunc& cb);
        EventCb(const CbFunc& cb, u32 priority);
        EventCb(const CbFunc& cb, u32 priority, u32 event_id);

        EventCallbackId getId()const;
        u32 getPriority()const;
        const CbFunc& getFunc()const;

        void setBoundSharedObj(const RefCommonPtr &obj_ptr);
        const RefCommonPtr& getBoundSharedObj()const;
    private:
        template <typename> friend class SortedArray;

        // needed for SortedCallbacks
        u32& accId_() { return cb_id_.callback_id; }
        struct Comparator {
            bool operator()(const MyType& cb1, const MyType& cb2) { return cb1.priority_ < cb2.priority_; }
        };
        // --------------------

        CbFunc cb_;
        RefCommonPtr bound_obj_;
        EventCallbackId cb_id_;
        u32 priority_;
    };

    template<typename> class EventCbTypeFromFunctor;
    template <typename TReturn, typename Arg1, typename ...Args>
    class EventCbTypeFromFunctor<TReturn(Arg1, Args...)> {
    public:
        typedef EventCb<TReturn(Args...)> Type;
    };

    // use for binding to object without ref_counting
    // or for Callback without object
    template <typename Functor, typename TObject = void>
    static constexpr auto createEventCb(TObject *obj = NULL);
    template <typename Functor, typename TObject = void>
    static constexpr auto createEventCb(u32 priority, TObject *obj = NULL);

    // use for binding to object with ref_counting
    template <typename Functor, typename TObject>
    static constexpr auto createEventCbWithObjRC(const RefCommonPtr& obj_ptr);
    template <typename Functor, typename TObject>
    static constexpr auto createEventCbWithObjRC(u32 priority,const RefCommonPtr& obj_ptr);

    template <typename> class SortedCallbacks;
    template <typename TReturn, typename ...Args>
    class SortedCallbacks<TReturn(Args...)> : public SortedArray<EventCb<TReturn(Args...)>> {
    public:
        void operator()(Args&&... args);
    };

    template <typename> class SortedCallbacksBreakable;
    template <typename TReturn, typename ...Args>
    class SortedCallbacksBreakable<TReturn(Args...)> : public SortedArray<EventCb<TReturn(Args...)>> {
    public:
        bool operator()(Args&&... args);
    };


    template <typename ...Args>
    class EventsHandler {
    public:
        typedef ObjFunc<bool(Args...)> CbFunc;
        typedef EventCb<bool(Args...)> EventCbT;
        typedef SortedArray<EventCbT> SortedCallbacks;
    public:
        EventCbT& addCallback(u32 event_id, const CbFunc &cb);
        EventCbT& addCallback(u32 event_id, u32 priority, const CbFunc &cb);
        const EventCbT& getCallback(EventCallbackId id);
        void removeCallback(EventCallbackId id);
        void emit(u32 event_id, Args &&... args)const;
        bool emitBreakable(u32 event_id, Args &&... args)const;     // returns if handlers chain was broken
        bool isValidId(EventCallbackId id)const;
    private:
        SortedCallbacks& getOrCreateCallbacks_(u32 event_id);

        HashMap<SortedCallbacks, u32, Hasher<u32> > callbacks_;
    };

    class EventsHandlerTyped : public EventsHandler<void*> {
        typedef EventsHandler<void*> Base;
        typedef EventCb<bool(void*)> EventCbT;
    public:
        // for typed events
        template <typename EventType>
        EventCbT& addCallbackT(const CbFunc &cb);
        template <typename EventType>
        EventCbT& addCallbackT(u32 priority, const CbFunc &cb);

        template <typename EventType>
        void emitT(EventType& e)const;
        template <typename EventType>
        bool emitBreakableT(EventType& e)const;

        template <typename EventType>
        static u32 getEventId();
    };
}

#include "EventsHandler.inl"

#endif //EVENTS_HANDLER_H
