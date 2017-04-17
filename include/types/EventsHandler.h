#ifndef EVENTS_HANDLER_H
#define EVENTS_HANDLER_H

#include "functions/defs.h"
#include "functions/sort_utils.h"
#include "types/containers/HashMap.h"
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

    template <typename ...Args>
    class EventsHandler {
    public:

        typedef ObjFunc<bool(Args...)> CbFunc;

        struct EventCb {
            CbFunc cb;
            u32 cb_id;
            u32 priority;

            struct LTE {
                bool operator()(const EventCb& cb1, const EventCb& cb2) {
                    return cb1.priority <= cb2.priority;
                }
            };
        };

        struct SortedCallbacks {
            fast_vector<EventCb> sorted;
            fast_vector<u32> id_to_pos;
            fast_vector<u32> free_ids;
            u32 next_id;
        };

        struct IntHasher {
            u32 operator()(u32 k)const {
                return calcHash32(k);
            }
        };

    public:
        EventCallbackId addCallback(u32 event_id, const CbFunc &cb);
        EventCallbackId addCallback(u32 event_id, u32 priority, const CbFunc &cb);
        const EventCb& getCallback(EventCallbackId id);
        void removeCallback(EventCallbackId id);
        bool emit(u32 event_id, Args &&... args)const;
        bool isValidId(EventCallbackId id)const;
    private:
        u32 getNextId_(u32 event_id, SortedCallbacks*& cbs_out);

        HashMap<SortedCallbacks, u32, IntHasher> callbacks_;
    };

    class EventsHandlerTyped : public EventsHandler<void*> {
        typedef EventsHandler<void*> Base;
    public:
        // for typed events
        template <typename EventType>
        EventCallbackId addCallbackT(const CbFunc &cb);
        template <typename EventType>
        EventCallbackId addCallbackT(u32 priority, const CbFunc &cb);

        template <typename EventType>
        bool emitT(EventType& e)const;

        template <typename EventType>
        static u32 getEventId();
    };
}

#include "EventHandlers.inl"

#endif //EVENTS_HANDLER_H
