#include "EventsHandler.h"

#define ECB_TPL template <typename TReturn, typename ...Args>
#define ECB_TYPE EventCb<TReturn(Args...)>
#define EHS_TPL template <typename ...Args>
#define EHS_TYPE EventsHandler<Args...>

namespace grynca {

    ECB_TPL
    inline ECB_TYPE::EventCb(const CbFunc& cb)
     : cb_(cb), priority_(InvalidId())
    {}

    ECB_TPL
    inline ECB_TYPE::EventCb(const CbFunc& cb, u32 priority)
     : cb_(cb), priority_(priority)
    {
    }

    ECB_TPL
    inline ECB_TYPE::EventCb(const CbFunc& cb, u32 priority, u32 event_id)
     : cb_(cb), priority_(priority)
    {
        cb_id_.event_id = event_id;
    }

    ECB_TPL
    inline EventCallbackId ECB_TYPE::getId()const {
        return cb_id_;
    }

    ECB_TPL
    inline u32 ECB_TYPE::getPriority()const {
        return priority_;
    }

    ECB_TPL
    inline const typename ECB_TYPE::CbFunc& ECB_TYPE::getFunc()const {
        return cb_;
    }

    ECB_TPL
    inline void ECB_TYPE::setBoundSharedObj(const RefCommonPtr &obj_ptr) {
        bound_obj_ = obj_ptr;
        cb_.accObjPtr() = (void*)bound_obj_.getPtr();
    }

    ECB_TPL
    inline const RefCommonPtr& ECB_TYPE::getBoundSharedObj()const {
        return bound_obj_;
    }

    template <typename Functor, typename TObject>
    inline constexpr auto createEventCb(TObject *obj) {
        //static
        typedef typename EventCbTypeFromFunctor<decltype(Functor::f)>::Type EventCbType;
        EventCbType cb_func(createObjFunc<Functor, TObject>(obj));
        return std::move(cb_func);
    }

    template <typename Functor, typename TObject>
    inline constexpr auto createEventCb(u32 priority, TObject *obj) {
        //static
        typedef typename EventCbTypeFromFunctor<decltype(Functor::f)>::Type EventCbType;
        EventCbType cb_func(createObjFunc<Functor, TObject>(obj), priority);
        return std::move(cb_func);
    }

    template <typename Functor, typename TObject>
    inline constexpr auto createEventCbWithObjRC(const RefCommonPtr& obj_ptr) {
        // static
        typedef typename EventCbTypeFromFunctor<decltype(Functor::f)>::Type EventCbType;
        EventCbType cb_func(createObjFunc<Functor, TObject>());
        cb_func.setBoundSharedObj(obj_ptr);
        return std::move(cb_func);
    }

    template <typename Functor, typename TObject>
    inline constexpr auto createEventCbWithObjRC(u32 priority, const RefCommonPtr& obj_ptr) {
        // static
        typedef typename EventCbTypeFromFunctor<decltype(Functor::f)>::Type EventCbType;
        EventCbType cb_func(createObjFunc<Functor, TObject>(), priority);
        cb_func.setBoundSharedObj(obj_ptr);
        return std::move(cb_func);
    }

    template <typename TReturn, typename ...Args>
    inline void SortedCallbacks<TReturn(Args...)>::operator()(Args&&... args) {
        for (u32 i=0; i<this->size(); ++i) {
            this->getItemAtPos(i).getFunc()(std::forward<Args>(args)...);
        }
    }

    template <typename TReturn, typename ...Args>
    bool SortedCallbacksBreakable<TReturn(Args...)>::operator()(Args&&... args) {
        for (u32 i=0; i<this->size(); ++i) {
            if (this->getItemAtPos(i).getFunc()(std::forward<Args>(args)...)) {
                return true;
            }
        }
        return false;
    }

    EHS_TPL
    inline typename EHS_TYPE::EventCbT& EHS_TYPE::addCallback(u32 event_id, const CbFunc &cb) {
        SortedCallbacks& cbs = getOrCreateCallbacks_(event_id);
        return cbs.addLast(EventCbT(cb, InvalidId(), event_id));
    }

    EHS_TPL
    inline typename EHS_TYPE::EventCbT& EHS_TYPE::addCallback(u32 event_id, u32 priority, const CbFunc &cb) {
        SortedCallbacks& cbs = getOrCreateCallbacks_(event_id);
        return cbs.add(EventCbT(cb, priority, event_id));
    }

    EHS_TPL
    inline const typename EHS_TYPE::EventCbT& EHS_TYPE::getCallback(EventCallbackId id) {
        ASSERT(isValidId(id));
        SortedCallbacks* cbs = callbacks_.findItem(id.event_id);
        return cbs->getItem(id.callback_id);
    }

    EHS_TPL
    inline void EHS_TYPE::removeCallback(EventCallbackId id) {
        ASSERT(isValidId(id));
        SortedCallbacks* cbs = callbacks_.findItem(id.event_id);
        cbs->removeItem(id.callback_id);
    }

    EHS_TPL
    inline void EHS_TYPE::emit(u32 event_id, Args &&... args)const {
        const SortedCallbacks* cbs = callbacks_.findItem(event_id);
        if (!cbs)
            // no handlers for this event type
            return;
        for (u32 i=0; i<cbs->size(); ++i) {
            cbs->getItemAtPos(i).getFunc()(std::forward<Args>(args)...);
        }
    }

    EHS_TPL
    inline bool EHS_TYPE::emitBreakable(u32 event_id, Args &&... args)const {
        const SortedCallbacks* cbs = callbacks_.findItem(event_id);
        if (!cbs)
        // no handlers for this event type
            return false;
        for (u32 i=0; i<cbs->size(); ++i) {
            if (cbs->getItemAtPos(i).getFunc()(std::forward<Args>(args)...)) {
                return true;
            }
        }
        return false;
    }

    EHS_TPL
    inline bool EHS_TYPE::isValidId(EventCallbackId id)const {
        const SortedCallbacks* cbs = callbacks_.findItem(id.event_id);
        if (!cbs) {
            return false;
        }

        return cbs->containsItem(id.callback_id);
    }

    EHS_TPL
    inline typename EHS_TYPE::SortedCallbacks& EHS_TYPE::getOrCreateCallbacks_(u32 event_id) {
        bool was_added;
        SortedCallbacks* cbs_out = callbacks_.findOrAddItem(event_id, was_added);
        if (was_added) {
            new (cbs_out) SortedCallbacks();
        }
        return *cbs_out;
    }

    template <typename EventType>
    inline typename EventsHandlerTyped::EventCbT& EventsHandlerTyped::addCallbackT(const CbFunc &cb) {
        return Base::addCallback(getEventId<EventType>(), cb);
    }

    template <typename EventType>
    inline typename EventsHandlerTyped::EventCbT& EventsHandlerTyped::addCallbackT(u32 priority, const CbFunc &cb) {
        return Base::addCallback(getEventId<EventType>(), priority, cb);
    }

    template <typename EventType>
    inline void EventsHandlerTyped::emitT(EventType& e)const {
        Base::emit(getEventId<EventType>(), &e);
    }

    template <typename EventType>
    inline bool EventsHandlerTyped::emitBreakableT(EventType& e)const {
        return Base::emitBreakable(getEventId<EventType>(), &e);
    }

    template <typename EventType>
    inline u32 EventsHandlerTyped::getEventId() {
    // static
        return Type<EventType, EventsHandlerTyped>::getInternalTypeId();
    }
}

#undef ECB_TPL
#undef ECB_TYPE
#undef EHS_TPL
#undef EHS_TYPE