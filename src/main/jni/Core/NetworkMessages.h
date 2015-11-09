#ifndef NETWORKMESSAGES_H
#define NETWORKMESSAGES_H

#include "Dispatch/MessageQueue.h"

namespace Core
{
    class OnConnection;
    class OnDisconnection;

    class RequestStream;

    /* request socket connect */
    /* though UDP is connectionless, we tend to treat it as connection-base socket */
    class ConnectionRequestMessage : public Dispatch::Message
    {
        typedef Message Base;

    public:
        enum Type : std::uint8_t
        {
            TCP = 0, UDP = 1
        };

        enum Domain : std::uint8_t
        {
            INET = 0, INET6 = 1
        };

    public:
        Domain _domain;
        Type _type;
        std::uint16_t _port;

        std::int32_t _connID;
        std::string _ip;

        OnConnection *_onConnection;

    public:
        static Dispatch::MessageType Type;

    public:
        template<typename T>
        ConnectionRequestMessage(const std::int32_t& id, T&& ip, std::uint16_t port, bool isIpv6, bool isTcp, OnConnection *onConn)
            : Base{ MessageCategory_User, Type }
            , _connID{ id }
            , _ip( ip )
            , _port( port )
            , _domain{ isIpv6 ? INET6 : INET }
            , _type{ isTcp ? TCP : UDP }
            , _onConnection{ onConn }
        {}

        ~ConnectionRequestMessage() = default;
    };

    /* send data via socket */
    class SendRequestMessage : public Dispatch::Message
    {
        typedef Message Base;

    public:
        std::uint32_t _reqID;
        RequestStream *_stream;

    public:
        static Dispatch::MessageType Type;

    public:
        /* take the ownership */
        /* when finished, */
        SendRequestMessage(const std::uint32_t& id, RequestStream *&&stream)
            : Base(MessageCategory_User, Type)
            , _reqID{ id }
        , _stream{ stream }
        {}

        ~SendRequestMessage() = default;
    };

    class DisconnectionRequestMessage : public Dispatch::Message
    {
        typedef Message Base;

    private:
        std::int32_t _connID;
        OnDisconnection *_onDisconnection;

    public:
        static Dispatch::MessageType Type;

    public:
        DisconnectionRequestMessage(const std::int32_t& id, OnDisconnection *onDisconn)
            : Base{ MessageCategory_User, Type }
            , _connID{ id }
            , _onDisconnection{ onDisconn }
        {}

        ~DisconnectionRequestMessage() = default;
    };
}

#endif  /* NETWORKMESSAGES_H */