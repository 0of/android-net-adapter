#ifndef NETWORKMESSAGESHANDLER_H
#define NETWORKMESSAGESHANDLER_H

#include "NetworkTransportObject.h"
#include "Network/SocketReactor.h"

#include <list>
#include <map>

namespace Dispatch
{
    class Message;
    class Context;
    class EventDispatcher;
}

namespace Core
{
    class MemoryResponseStream : public ResponseStream
    {
    public:
        std::uint8_t *_buffer;
        std::uint32_t _readableLen;
        std::uint32_t _offset;

    public:
        explicit MemoryResponseStream(std::uint32_t totalLen);
        MemoryResponseStream();

    public:
        virtual std::uint32_t Read(std::uint8_t *buffer, std::uint32_t len);
    };

    class SocketEventHandlerImpl : public Net::SocketEventHandler
    {
    public:
        enum TaskType { Send = 0, Receive = 1 };

        using Stream = RequestStream;
        using Task = std::pair<TaskType, Stream *>;
        using SocketTaskQueue = std::list<Task>;

    protected:
        struct ConnectingToken
        {
            std::int32_t id;
            std::uint16_t domain;
            std::uint16_t port;

            std::string address;

            template<typename T>
            ConnectingToken(std::int32_t id, std::uint16_t domain, std::uint16_t port, T&& addr)
                : id{ id }
                , domain{ domain }
                , port{ port }
                , address( std::forward<T>(addr) )
            {
            }
        } _token;

        SocketTaskQueue _queue;

        std::uint8_t *_buffer;
        std::uint32_t _len;

    protected:
        SocketEventHandlerImpl(std::int32_t connID, std::uint16_t domain, std::uint16_t port, const std::string& addr);

    public:
        virtual ~SocketEventHandlerImpl();

    public:
        virtual bool Accept(Net::Mode mode) const final;

    public:
        void PushTask(const Task& t);

    private:
        SocketEventHandlerImpl(const SocketEventHandlerImpl&) = delete;
        SocketEventHandlerImpl& operator = (const SocketEventHandlerImpl&) = delete;
    };

    class NetworkContext;

    class NetworkMessagesHandler
    {
    public:
        static bool Handle(Dispatch::EventDispatcher& dispatcher, NetworkContext& ctx, Dispatch::Message *msg);
    };
}

#endif  /* NETWORKMESSAGESHANDLER_H */