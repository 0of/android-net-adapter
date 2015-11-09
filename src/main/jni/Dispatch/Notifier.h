#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <cstddef>

namespace Dispatch
{
    class Event;
    class EventDispatcher;
    class Context;

    class Notifier
    {
    public:
        static void Make(Notifier& target, Dispatch::EventDispatcher *dispatcher, Context *context);

    private:
        Dispatch::EventDispatcher *_dispatcher;
        Context *_context;

    public:
        Notifier();
        ~Notifier();

    public:
        void Notify(Event& ev);

    private:
        Notifier(const Notifier&) = delete;
        Notifier& operator =(const Notifier&) = delete;

    private:
        /* not allow to construct on heap */
        void* operator new(std::size_t) = delete;
        void operator delete(void*) = delete;
    };
}

#endif  /* NOTIFIER_H */