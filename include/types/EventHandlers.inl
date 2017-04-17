#include "EventsHandler.h"

#define EHS_TPL template <typename ...Args>
#define EHS_TYPE EventsHandler<Args...>

namespace grynca {

    EHS_TPL
    inline EventCallbackId EHS_TYPE::addCallback(u32 event_id, const CbFunc &cb) {
        EventCb new_cb;
        new_cb.cb = cb;
        SortedCallbacks* cbs;
        new_cb.cb_id = getNextId_(event_id, cbs);
        // adds to back
        new_cb.priority = cbs->sorted.empty() ? InvalidId() : cbs->sorted.back().priority;
        cbs->id_to_pos[new_cb.cb_id] = u32(cbs->sorted.size());
        cbs->sorted.push_back(new_cb);
        return {event_id, new_cb.cb_id};
    }

    EHS_TPL
    inline EventCallbackId EHS_TYPE::addCallback(u32 event_id, u32 priority, const CbFunc &cb) {
        EventCb new_cb;
        new_cb.cb = cb;
        SortedCallbacks* cbs;
        new_cb.cb_id = getNextId_(event_id, cbs);
        new_cb.priority = priority;
        // find pos
        u32 pos = bisectFindInsert(cbs->sorted, new_cb, typename EventCb::LTE());
        cbs->id_to_pos[new_cb.cb_id] = pos;
        cbs->sorted.insert(cbs->sorted.begin()+pos, new_cb);
        for (u32 i=pos+1; i<u32(cbs->sorted.size()); ++i) {
            u32 cb_id = cbs->sorted[i].cb_id;
            ++cbs->id_to_pos[cb_id];
        }
        return {event_id, new_cb.cb_id};
    }

    EHS_TPL
    inline const typename EHS_TYPE::EventCb& EHS_TYPE::getCallback(EventCallbackId id) {
        ASSERT(isValidId(id));
        SortedCallbacks* cbs = callbacks_.findItem(id.event_id);
        u32 pos = cbs->id_to_pos[id.callback_id];
        return cbs->sorted[pos];
    }

    EHS_TPL
    inline void EHS_TYPE::removeCallback(EventCallbackId id) {
        ASSERT(isValidId(id));
        SortedCallbacks* cbs = callbacks_.findItem(id.event_id);
        u32 pos = cbs->id_to_pos[id.callback_id];
        for (u32 i=pos+1; i<cbs->sorted.size(); ++i) {
            u32 cb_id = cbs->sorted[i].cb_id;
            --cbs->id_to_pos[cb_id];
        }
        cbs->sorted.erase(cbs->sorted.begin()+pos);
        cbs->id_to_pos[id.callback_id] = InvalidId();
        cbs->free_ids.push_back(id.callback_id);
    }

    EHS_TPL
    inline bool EHS_TYPE::emit(u32 event_id, Args &&... args)const {
        const SortedCallbacks* cbs = callbacks_.findItem(event_id);
        if (!cbs)
            // no handlers for this event type
            return false;
        for (u32 i=0; i<u32(cbs->sorted.size()); ++i) {
            bool prevent_next = cbs->sorted[i].cb(std::forward<Args>(args)...);
            if (prevent_next)
                return true;
        }
        return false;
    }

    EHS_TPL
    inline bool EHS_TYPE::isValidId(EventCallbackId id)const {
        const SortedCallbacks* cbs = callbacks_.findItem(id.event_id);
        if (!cbs) {
            return false;
        }

        return (id.callback_id < cbs->id_to_pos.size()
             && cbs->id_to_pos[id.callback_id]!=InvalidId());
    }

    EHS_TPL
    inline u32 EHS_TYPE::getNextId_(u32 event_id, SortedCallbacks*& cbs_out) {
        bool was_added;
        cbs_out = callbacks_.findOrAddItem(event_id, was_added);
        if (was_added) {
            new (cbs_out) SortedCallbacks();
            cbs_out->next_id = 0;
        }
        u32 id;
        if (cbs_out->free_ids.empty()) {
            id = cbs_out->next_id;
            cbs_out->id_to_pos.resize(id+1);
            ++cbs_out->next_id;
        }
        else {
            id = cbs_out->free_ids.back();
            cbs_out->free_ids.pop_back();
        }
        return id;
    }

    template <typename EventType>
    inline EventCallbackId EventsHandlerTyped::addCallbackT(const CbFunc &cb) {
        return Base::addCallback(getEventId<EventType>(), cb);
    }

    template <typename EventType>
    inline EventCallbackId EventsHandlerTyped::addCallbackT(u32 priority, const CbFunc &cb) {
        return Base::addCallback(getEventId<EventType>(), priority, cb);
    }

    template <typename EventType>
    inline bool EventsHandlerTyped::emitT(EventType& e)const {
        return Base::emit(getEventId<EventType>(), &e);
    }

    template <typename EventType>
    inline u32 EventsHandlerTyped::getEventId() {
    // static
        return Type<EventType, EventsHandlerTyped>::getInternalTypeId();
    }
}

#undef EHS_TPL
#undef EHS_TYPE