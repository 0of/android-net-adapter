#include "EventDispatcher.h"
#include "EventHandler.h"

namespace Dispatch
{
    EventDispatcher::EventDispatcher(ThreadID tid)
        : _attachedThreadID{ tid }
        , _handlers{}
    {
    }

    EventDispatcher::~EventDispatcher()
    {

    }

    void EventDispatcher::Register(std::uint16_t event, UniqueHandlers&& handlers)
    {
        auto found = _handlers.find(event);

        /* remove null handler */
        handlers.erase(nullptr);

        if (found != _handlers.end())
        {
            found->second.insert(handlers.begin(), handlers.end());
        }
        else
        {
            _handlers.insert(std::make_pair(event, std::move(handlers)));
        }
    }

    void EventDispatcher::Register(const UniqueEvents& events, const SharedEventHandler& handler)
    {
        for (auto ev : events)
        {
            Register(ev, handler);
        }
    }

    void EventDispatcher::Register(std::uint16_t event, const SharedEventHandler& handler)
    {
        if (!handler) return;

        auto found = _handlers.find(event);

        if (found != _handlers.end())
        {
            found->second.insert(handler);
        }
        else
        {
            _handlers.insert(std::make_pair(event, UniqueHandlers{handler}));
        }
    }

    void EventDispatcher::Unregister(std::uint16_t event)
    {
        _handlers.erase(event);
    }

    void EventDispatcher::Unregister(std::uint16_t event, const SharedEventHandler& handler)
    {
        auto found = _handlers.find(event);

        if (found != _handlers.end())
        {
            found->second.erase(handler);
        }
    }

    void EventDispatcher::Unregister(const SharedEventHandler& handler)
    {
        for (auto eachEventHandlers : _handlers)
        {
            eachEventHandlers.second.erase(handler);
        }
    }

    void EventDispatcher::Dispatch(Context& context, Event& ce) NOEXCEPT
    {
        auto found = _handlers.find(ce.GetType());

        if (found != _handlers.end())
        {
            auto clone = found->second;

            for (auto eachEventHandlers : clone)
            {
                eachEventHandlers->Handle(context, ce);
            }
        }
    }

    bool EventDispatcher::RegisterAny(EventType t) const
    {
        return _handlers.end() != _handlers.find(t);
    }
}