#include "EventHandler.h"

#include <map>

namespace Dispatch
{
    EventType Event::RegisterType(EventType hint)
    {
        static std::map<EventType, EventType> registered{ std::make_pair(NullEvent, 0) };
        auto bigger = registered.rbegin()->first + 1;
        registered.insert(std::make_pair(bigger, 0));

        return bigger;
    }

    Message *EventMessage::Create(Event *e)
    {
        if (e)
        {
            return new EventMessage(e);
        }

        return nullptr;
    }

    EventMessage::~EventMessage()
    {
        delete _;
    }

    EventMessage::EventMessage(Event *e)
        : Base(Base::MessageCategory_Event, EventContainer)
        , _{e}
    {

    }
}

