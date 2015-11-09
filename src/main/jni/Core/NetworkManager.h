#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <vector>

#include "NetworkMessagesHandler.h"

namespace Except
{
    class Exception;
}

namespace Dispatch
{
    class Message;
    class EventDispatcher;
    class EventHandler;
}

namespace Core
{
    class NetworkContext;
    class RequestStream;
    class OutcomingMiddleware;
    class IncomingMiddleware;

    class NetworkManager
    {
    public:
        class PrivateData;
        class CoreManager;

    private:
        PrivateData *_p;
        CoreManager *_mgr;

        // only for register event handler
        Dispatch::EventDispatcher& _dispatcher;
        std::vector<Dispatch::EventHandler *> _handlers;

    public:
        NetworkManager(NetworkContext& context, Dispatch::EventDispatcher& dispatcher);
        ~NetworkManager();

    public:
        bool Process(Dispatch::Message *msg);

    public:
        void Select();

    public:
        // middleware register
        void Register(IncomingMiddleware *&& /* take the ownership */middleware);
        void Register(OutcomingMiddleware *&& /* take the ownership */middleware);
    };
}

#endif  /*  NETWORKMANAGER_H  */