#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <cstdint>

#include "globals.h"
#include "MessageQueue.h"

namespace Dispatch
{
    using EventType = std::uint16_t;

    class Event
    {
    public:
        static const EventType NullEvent = 0;
        static EventType RegisterType(EventType hint = 0);

    public:
        Event(EventType type)
            : _acceptance(1)
            , _isTransmissible(0)
            , _flags(0)
            , _type(type)
        {
        }

        virtual ~Event() = default;

    protected:
        std::uint16_t _acceptance : 1;
        std::uint16_t _isTransmissible : 1;
        std::uint16_t _flags : 14;
        EventType _type;

    public:
        virtual Event *Clone() const { return nullptr; }

    public:
        void Accept() { _acceptance = 1; }
        void Refuse() { _acceptance = 0; }

        inline EventType GetType() { return _type; }

    public:
        bool IsAccepted() const { return 0 != _acceptance; }
        bool IsRefused() const { return 0 == _acceptance; }

    public:
        Event & operator = (const Event& ev);

        bool operator == (const Event& ev){ return _type == ev._type; }
        bool operator !=  (const Event& ev){ return _type != ev._type; }
    };

    class EventMessage : public Message
    {
        using Base = Message;

    public:
        const static std::uint16_t EventContainer = 0;

    private:
        Event *_;

    public:
        static Message *Create(Event *e);

    public:
        /* free the event when destructing */
        ~EventMessage();

    protected:
        EventMessage(Event *e);

    public:
        operator Event&() { return *_; }
    };

    class Context;
    class NOVTABLE EventHandler
    {
    public:
        virtual ~EventHandler() = default;

    public:
        virtual void Handle(Context& context, Event& ev) = 0;
    };
}

/* global */
template<typename ConcreateEvent>
typename std::add_pointer<ConcreateEvent>::type event_cast(Dispatch::Event *ev)
{
    if (ev->GetType() == ConcreateEvent::Type)
    {
        return static_cast<typename std::add_pointer<ConcreateEvent>::type>(ev);
    }
    return nullptr;
}

template<>
inline Dispatch::EventMessage *message_cast<Dispatch::EventMessage>(Dispatch::Message *msg)
{
    if (msg->Category() == Dispatch::Message::MessageCategory_Event && msg->GetType() == Dispatch::EventMessage::EventContainer)
    {
        return static_cast<Dispatch::EventMessage *>(msg);
    }
    return nullptr;
}

#endif  /* EVENTHANDLER_H */