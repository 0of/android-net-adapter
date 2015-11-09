#ifndef NETWORKCONTEXT_H
#define NETWORKCONTEXT_H

#include "Dispatch/Context.h"
#include "NetworkMessagesHandler.h"

namespace Core
{
    using ConnectedSocket = std::pair<Net::Socket, SocketEventHandlerImpl *>;
    using Connections = std::map<std::uint32_t, ConnectedSocket>;



    class NetworkContext : public Dispatch::Context
    {
    private:
        Dispatch::EventDispatcher& _dispatcher;

        Connections _connections;
        Net::SocketReactor _reactor;

    public:
        explicit NetworkContext(Dispatch::EventDispatcher& attachedDispatcher);
        ~NetworkContext();

    public:
        virtual void AcquireNotifier(Dispatch::Notifier& notifier);

    public:
        void Add(std::uint32_t id, const ConnectedSocket& conn);
        void Remove(std::uint32_t id);

        bool Find(std::uint32_t id, ConnectedSocket&);

    public:
        void ShutdownAll();

        void Run(const std::chrono::milliseconds& timeout);

    protected:
        // support services:
        //      NetworkService
        virtual void *GetRawCapability(std::uint16_t cap);
    };
}

#endif  /* NETWORKCONTEXT_H */