#include "Notifier.h"

#include "EventDispatcher.h"
#include "Dispatch/CapabilityCategory.h"

namespace Dispatch
{
    void Notifier::Make(Notifier& target, Dispatch::EventDispatcher *dispatcher, Context *context)
    {
        target._dispatcher = dispatcher;
        target._context = context;
    }

    Notifier::Notifier()
        : _dispatcher{ nullptr }
        , _context{ nullptr }
    {}

    Notifier::~Notifier()
    {}

    void Notifier::Notify(Event& ev)
    {
        if (_dispatcher && _context)
        {
            _dispatcher->Dispatch(*_context, ev);
        }
    }
}
