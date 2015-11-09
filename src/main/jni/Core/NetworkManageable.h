#ifndef NETWORKMANAGEABLE_H
#define NETWORKMANAGEABLE_H

namespace Except
{
    class Exception;
}

namespace Core
{
    class RequestStream;

    enum class Stage
    {
        Unknown, BeforeConnect, Connect, Write, Read, Disconnect
    };

    // hold by socket handlers
    class NetworkManageable
    {
    public:
        virtual ~NetworkManageable() = default;

    public:
        virtual void NotifyConnected(std::int32_t id) = 0;
        virtual void NotifyDisconnected(std::int32_t id) = 0;

    public:
        virtual void NotifySent(std::int32_t id, RequestStream *sentStream) = 0;
        virtual void NotifyRead(std::int32_t id, RequestStream *readStream) = 0;

    public:
        // always called within cache clause
        virtual void NotifyException(std::int32_t id, Stage stage, const Except::Exception& ex) = 0;
    };
}

#endif /*  NETWORKMANAGEABLE_H  */