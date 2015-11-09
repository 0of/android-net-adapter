#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H

#include <unordered_map>
#include <set>
#include <memory>

#include "globals.h"
#include "EventHandler.h"

namespace Dispatch
{
    class Context;
    class EventHandler;

    using SharedEventHandler = std::shared_ptr<EventHandler>;
    using UniqueHandlers = std::set<SharedEventHandler>;
    using UniqueEvents = std::set<EventType>;

    /* no throw */
    class EventDispatcher
    {
        using RegisterHandlers = std::unordered_map<EventType, UniqueHandlers>;

    private:
        ThreadID _attachedThreadID;
        RegisterHandlers _handlers;

    public:
        explicit EventDispatcher(ThreadID tid);
        ~EventDispatcher();

    public:
        void Register(EventType event, UniqueHandlers&& handlers);
        void Register(const UniqueEvents& events, const SharedEventHandler& handler);

        void Register(EventType event, const SharedEventHandler& handler);

        /* remove all the handler by specific event */
        void Unregister(EventType event);

        void Unregister(EventType event, const SharedEventHandler& handler);

        /* remove specific handler registered any events */
        void Unregister(const SharedEventHandler& handler);

    public:
        void Dispatch(Context& context, Event& ce) NOEXCEPT;
        bool RegisterAny(EventType t) const;
    };
}

#endif  /* EVENTDISPATCHER_H */