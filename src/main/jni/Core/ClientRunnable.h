#ifndef CLIENTRUNNABLE_H
#define CLIENTRUNNABLE_H

#include <memory>

#include "Dispatch/MessageQueue.h"
#include "Dispatch/MessageQueueManager.h"

namespace Dispatch
{
    class RequestPacketQueue;
    class EventDispatcher;
}

using SharedRequestPacketQueue = std::shared_ptr<Dispatch::RequestPacketQueue>;

namespace Core
{
    class InitSynchronizer;
    class ClientRunnable;
    class NetworkContext;

    using SharedNetworkContext = std::shared_ptr<NetworkContext>;

    /* thread stack */
    class InitNotifier
    {
    protected:
        InitSynchronizer *_sync;

    public:
        explicit InitNotifier(InitSynchronizer *);
        virtual ~InitNotifier() = default;

    public:
        void Initialized(ClientRunnable& runnable);

    protected:
        virtual void OnInitialized(ClientRunnable& runnable);
    };

    class ClientRunnable
    {
    private:
        SharedRequestPacketQueue _queue;

        SharedNetworkContext _context;

        /* thread stack object */
        Dispatch::EventDispatcher *_dispatcher;

    public:
        ClientRunnable();
        ~ClientRunnable();

    public:
        void Run(InitNotifier &initNotifier);

        /* when initialized, those values are valid */
        Dispatch::SharedContext Context() const;
        Dispatch::QueueIdentifier RegisteredQueueIdentifier() const;
        Dispatch::EventDispatcher *Dispatcher() const { return _dispatcher; }

    protected:
        void OnRun();

    private:
        ClientRunnable(const ClientRunnable&) = delete;
        ClientRunnable& operator = (const ClientRunnable&) = delete;
    };
}

#endif /* CLIENTRUNNABLE_H */