#ifndef SOCKETREACTOR_H
#define SOCKETREACTOR_H

#include <cstdint>
#include <map>
#include <chrono>
#include <vector>

#include "Socket.h"

namespace Net
{
    enum Mode
    {
        Read = 1,
        Write = 2,
        Except = 4
    };
    typedef std::uint32_t Modes;

    class SocketEventHandler
    {
    public:
        virtual ~SocketEventHandler() {}

    public:
        virtual void Handle(const Socket &socket, Modes modes) NOEXCEPT = 0;
        virtual bool Accept(Mode mode) const = 0;
    };

    /* thread NOT safe */
    class SocketReactor
    {
        typedef std::map<Socket, SocketEventHandler *> SocketEventHandlers;

    private:
        bool _isRunning;

        SocketEventHandlers _handlers;

        SocketEventHandlers _aboutAddHandlers;
        std::vector<Socket> _aboutRemoveHandlers;

    public:
        void Run(const std::chrono::milliseconds& timeout);

    public:
        /* take ownership */
        void RegisterHandler(const Socket& socket, SocketEventHandler *handler);

        /* supress the exception even handler is not found */
        void RemoveHandler(const Socket& socket);

    protected:
        void OnRegisterHandler(const Socket& socket, SocketEventHandler *handler);
        void OnRemoveHandler(const Socket& socket);
    };
}

#endif  /* SOCKETREACTOR_H */