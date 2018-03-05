#ifndef TEST_EVENT_HANDLERS_H
#define TEST_EVENT_HANDLERS_H

#include "types/EventsHandler.h"

namespace grynca { namespace test_event_handlers {

        enum EventTypes {
            etCommonEvent,
            etWonderousThing,
            etMostUnexpectedStuff,
            etOhNoItJustHappened,

            etCount
        };

        class MyHandlers: public EventsHandler<int&, const ustring&> {
        public:

        };

        class CollisionEvent {
        public:
            CollisionEvent(u32 e1, u32 e2) : ent1(e1), ent2(e2) {}

            u32 ent1, ent2;
        };

        class RandomEvent {
        public:
            RandomEvent(f32 r) : rand(r) {}

            f32 rand;
        };


        class MyReciever: public EventsReciever<MyReciever> {
        public:
            void init(EventsHandlerTyped& handler) {
                EventsReciever::init(handler);
                subscribe<CollisionEvent>();
                subscribe<RandomEvent>();
            }

            bool recieve(CollisionEvent& ce) {
                std::cout << "recieved collision event" << std::endl;
                return true;
            }

            bool recieve(RandomEvent& re) {
                std::cout << "recieved random event" << std::endl;
                // dont want other handlers to react
                return false;
            }
        };

        class EventsEmiter {
        public:
            void emit(MyHandlers& handlers, u32 e_type) {
                ustring msg;
                switch (e_type) {
                    case etCommonEvent:
                        msg = "Just emiting...";
                        break;
                    case etWonderousThing:
                        msg = "Hello.";
                        break;
                    case etMostUnexpectedStuff:
                        msg = "Gotcha.";
                        break;
                    case etOhNoItJustHappened:
                        msg = "This is Madness!";
                        break;
                }
                int num = 0;
                std::cout << msg << std::endl;
                bool forced_end = handlers.emitBreakable(e_type, num, msg);
                std::cout << " handlers " << num;
                if (forced_end)
                    std::cout << ", forced_end";
                std::cout << std::endl << std::endl;
            }
        };

        char constexpr msg1[] = "well you should be";
        char constexpr msg2[] = "good to know";
        char constexpr msg3[] = "Hello to you too!";
        char constexpr msg4[] = "I am Second.";
        char constexpr msg5[] = "I am First.";
        char constexpr msg6[] = "Is someone else handling this ?";
        char constexpr msg7[] = ": Madness? This is SPARTAA!!!";

        template <char const* response, bool rslt>
        struct MyHandler {
            static bool f(void*, int &h, const ustring &msg) {
                std::cout << h++ << ": " << response << std::endl;
                return rslt;
            }
        };

        struct Test {
            static void f(void*, Config::ConfigSectionMap&) {
                MyHandlers handlers;
                fast_vector<ObjFunc<bool(int&, const ustring &)> > fs(7);
                fs[0].bind<MyHandler<msg1, false> >();
                fs[1].bind<MyHandler<msg2, false> >();
                fs[2].bind<MyHandler<msg3, false> >();
                fs[3].bind<MyHandler<msg4, false> >();
                fs[4].bind<MyHandler<msg5, false> >();
                fs[5].bind<MyHandler<msg6, false> >();
                fs[6].bind<MyHandler<msg7, false> >();


                handlers.addCallback(etCommonEvent, fs[0]);
                handlers.addCallback(etCommonEvent, fs[1]);
                handlers.addCallback(etWonderousThing, fs[2]);
                handlers.addCallback(etWonderousThing, 1, fs[3]);
                handlers.addCallback(etWonderousThing, 0, fs[4]);
                handlers.addCallback(etMostUnexpectedStuff, 0, fs[5]);
                handlers.addCallback(etOhNoItJustHappened, 0, fs[6]);

                EventsEmiter emiter;
                for (u32 i=0; i<etCount; ++i) {
                    emiter.emit(handlers, i);
                }
            }
        };

        struct TestTyped {
            static void f(void*, Config::ConfigSectionMap&) {
                EventsHandlerTyped handler;
                MyReciever reciever;
                reciever.init(handler);

                for (u32 i=0; i<4; ++i) {
                    if (randFloat()<0.5f) {
                        CollisionEvent ce{1,2};
                        handler.emitT(ce);
                    }
                    else {
                        RandomEvent re{randFloat()};
                        handler.emitT(re);
                    }
                }
            }
        };
}}

#endif //TEST_EVENT_HANDLERS_H
