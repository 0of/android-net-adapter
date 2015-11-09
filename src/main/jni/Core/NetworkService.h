#ifndef NETWORKSERVICE_H
#define NETWORKSERVICE_H

#include <memory>
#include <string>

#include "globals.h"
#include "Dispatch/CapabilityCategory.h"

namespace Dispatch
{
    class ExceptionEvent;
}

namespace Core
{
    class RequestStream;

    class NOVTABLE OnConnection
    {
    public:
        virtual ~OnConnection() = default;

    public:
        virtual void OnConnected() = 0;
        virtual void OnError(Dispatch::ExceptionEvent& exceptionEv) = 0;
        virtual void OnTimeout(std::int32_t timespan) = 0;
    };

    class NOVTABLE OnDisconnection
    {
     public:
        virtual ~OnDisconnection() = default;

    public:
        virtual void OnDisconnected() = 0;
        virtual void OnError(Dispatch::ExceptionEvent& exceptionEv) = 0;
    };

    struct ConnectionString
    {
        bool isIpv6;
        bool isTcp;
        std::uint16_t port;
        std::string ip;
    };

    // id conflict is not allowed
    class NOVTABLE NetworkService
    {
    public:
        virtual ~NetworkService() = default;

    public:
        virtual void request(std::int32_t id, const ConnectionString& connectString, OnConnection *&& onConnection) = 0;
        virtual void request(std::int32_t id, OnDisconnection *&& onDisconnection) = 0;
    };
}

/* decl */
CAPABILITY_DECL(Core::NetworkService, 0xF001, "Network")

#endif  /* NETWORKSERVICE_H */