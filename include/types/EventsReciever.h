#ifndef EVENTSRECIEVER_H
#define EVENTSRECIEVER_H

#include "types/Type.h"
#include "EventsHandler.h"

namespace grynca {

    // handler <-> reciever link created via pointers
    // make sure they stay valid !

    template <typename EventType, typename Derived>
    struct EventRecieveHelper {

        // returns true if other handlers are to be prevented
        static bool f(Derived* obj, void* ptr) {
            EventType& e = *(EventType *)ptr;
            return obj->recieve(e);
        }
    };

    template <typename Derived>
    class EventsReciever {
    public:
        EventsReciever() : handler_(NULL) {}

        ~EventsReciever() {
            for (u32 i=0; i<subscribed_.size(); ++i) {
                handler_->removeCallback(subscribed_[i]);
            }
        }

        void init(EventsHandlerTyped& handler) {
            handler_ = &handler;
        }

        template <typename EventType>
        void subscribe() {
            ObjFunc<bool(void*)> f;
            f.bind<EventRecieveHelper<EventType, Derived> >((Derived *) this);
            auto& event_cb = handler_->addCallbackT<EventType>(f);
            subscribed_.push_back(event_cb.getId());
        }

        template <typename EventType>
        void unsubscribe() {
            u32 event_type_id = handler_->getEventId<EventType>();
            for (u32 i=0; i<subscribed_.size(); ++i) {
                if (subscribed_[i].event_id== event_type_id) {
                    handler_->removeCallback(subscribed_[i]);
                    break;
                }
            }
        }

    private:

        EventsHandlerTyped* handler_;
        fast_vector<EventCallbackId> subscribed_;
    };


}

#endif //EVENTSRECIEVER_H
