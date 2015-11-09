#ifndef NETWORKEVENTS_H
#define NETWORKEVENTS_H

#include "NetworkMessagesHandler.h"
#include "NetworkManageable.h"
#include "NetworkTransportObject.h"
#include "Dispatch/EventHandler.h"
#include "Dispatch/ExceptionEvent.h"

namespace Core
{
    using SharedResponseStream = std::shared_ptr<ResponseStream>;

    /* trigger this event if udp socket created */
    class ConnectedEvent : public Dispatch::Event
    {
        typedef Event Base;

    public:
        static Dispatch::EventType Type;

    public:
        std::int32_t _connID;

        ConnectedEvent(const std::int32_t& id)
            : Base(Type)
            , _connID{ id }
        {}
    };

    class DisconnectedEvent : public Dispatch::Event
    {
        typedef Event Base;

    public:
        static Dispatch::EventType Type;

    public:
        std::int32_t _connID;

        DisconnectedEvent(const std::int32_t& id)
            : Base(Type)
            , _connID{ id }
        {}
    };

    class NetworkExceptionEvent : public Dispatch::ExceptionEvent
    {
        typedef ExceptionEvent Base;

    public:
        static Dispatch::EventType Type;

    public:
        std::int32_t _connID;
        Stage _stage;

    public:
        // if before connect or unknown stage
        // id is invalid
        NetworkExceptionEvent(std::int32_t id, Stage stage)
            : Base()
            , _connID{ id }
            , _stage{ stage }
        {}
    };

    // TODO modify
    class ReceiveResponseEvent : public Dispatch::Event
    {
        typedef Event Base;

    public:
        std::uint32_t _connID;
        SharedResponseStream _stream;

    public:
        static Dispatch::EventType Type;

    public:
        /* take the ownership */
        /* when finished, */
        ReceiveResponseEvent(const std::uint32_t& id, const SharedResponseStream& stream)
            : Base(Type)
            , _connID{ id }
            , _stream{ stream }
        {}

        ~ReceiveResponseEvent() = default;
    };

    class SentRequestEvent : public Dispatch::Event
    {
        typedef Event Base;

    public:
        std::uint32_t _connID;
        std::uint32_t _count;

    public:
        static Dispatch::EventType Type;

    public:
        SentRequestEvent(const std::uint32_t& id, const std::uint32_t& count)
            : Base(Type)
            , _connID{ id }
            , _count{ count }
        {}

        ~SentRequestEvent() = default;
    };
}

#endif  /* NETWORKEVENTS_H */