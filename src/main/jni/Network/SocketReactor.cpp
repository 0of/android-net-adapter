#include "SocketReactor.h"

#include <limits>
#include <cassert>

#include "Socket.h"
#include "Exception/Exception.h"

namespace Net
{
    class Autobool
    {
    private:
        bool& _autobool;

    public:
        Autobool(bool& b)
            : _autobool{ b }
        {
            _autobool = true;
        }
        ~Autobool()
        {
            _autobool = false;
        }
    };

#define ACCEPT(mode) \
    if (handler->Accept(mode)) \
    { \
        FD_SET(fd, &fd##mode); \
        hasSet = true; \
    }

    void SocketReactor::Run(const std::chrono::milliseconds& timeout)
    {
        fd_set fdRead;
        fd_set fdWrite;
        fd_set fdExcept;

        SOCKET nfd = 0;
        FD_ZERO(&fdRead);
        FD_ZERO(&fdWrite);
        FD_ZERO(&fdExcept);

        for (auto eachpair : _handlers)
        {
            SOCKET fd = eachpair.first;
            auto handler = eachpair.second;

            bool hasSet = false;

            ACCEPT(Read)
            ACCEPT(Write)
            ACCEPT(Except)

            if (hasSet && fd > nfd)
                nfd = fd;
        }

        /* no fd is available */
        if (nfd == 0)
            return;

        auto result = 0;
        /* block */
        if (timeout == std::chrono::milliseconds::zero())
        {
            result = ::select(nfd + 1, &fdRead, &fdWrite, &fdExcept, nullptr);
        }
        else
        {
            struct timeval tv;

#pragma push_macro("max")
#undef max
            std::chrono::milliseconds::rep maxTimespan = std::numeric_limits<decltype(tv.tv_sec)>::max();
#pragma pop_macro("max")

            if (timeout.count() > maxTimespan)
            {
                /* too large */
                throw Except::InvalidArgumentException{ LOGIC_EXCEPT("timeout too large") };
            }

            tv.tv_sec = static_cast<decltype(tv.tv_sec)>(timeout.count() / 1000);
            tv.tv_usec = (timeout.count() % 1000) * 1000 ;

            result = ::select(nfd + 1, &fdRead, &fdWrite, &fdExcept, &tv);
        }

        /* */
        if (result == NET_SOCKET_ERROR)
        {
            Socket::OnSocketError();
            return;
        }

        /* timeout */
        if (result == 0)
        {
            throw Except::TimeoutException{};
        }

        /* on run */
        {
            Autobool isRunning{ _isRunning };

            for (auto eachpair : _handlers)
            {
                SOCKET fd = eachpair.first;

                Modes modes = 0;

                if (FD_ISSET(fd, &fdRead))
                    modes |= Read;

                if (FD_ISSET(fd, &fdWrite))
                    modes |= Write;

                if (FD_ISSET(fd, &fdExcept))
                    modes |= Except;

                eachpair.second->Handle(eachpair.first, modes);
            }
        }

        for (auto add : _aboutAddHandlers)
            OnRegisterHandler(add.first, add.second);

        for (auto remove : _aboutRemoveHandlers)
            OnRemoveHandler(remove);
    }

    void SocketReactor::RegisterHandler(const Socket& socket, SocketEventHandler *handler)
    {
        if (!socket.IsValid() || nullptr == handler)
        {
            /* throw */
            throw Except::InvalidArgumentException{ LOGIC_EXCEPT("Either is invalid") };
        }

        if (_isRunning)
        {
            _aboutAddHandlers.insert(std::make_pair(socket, handler));
        }
        else
        {
            for (auto add : _aboutAddHandlers)
                OnRegisterHandler(add.first, add.second);

            OnRegisterHandler(socket, handler);
        }
    }

    void SocketReactor::RemoveHandler(const Socket& socket)
    {
        if (_isRunning)
        {
            _aboutRemoveHandlers.push_back(socket);
        }
        else
        {
            for (auto remove : _aboutRemoveHandlers)
                OnRemoveHandler(remove);

            OnRemoveHandler(socket);
        }
    }

    void SocketReactor::OnRegisterHandler(const Socket& socket, SocketEventHandler *handler)
    {
        assert(!_isRunning);

        auto foundpair = _handlers.find(socket);
        if (foundpair != _handlers.end())
        {
            if (foundpair->second != handler)
            {
                /* delete previous one */
                delete foundpair->second;
                foundpair->second = handler;
            }
        }
        else
        {
            _handlers.insert(std::make_pair(socket, handler));
        }
    }

    void SocketReactor::OnRemoveHandler(const Socket& socket)
    {
        assert(!_isRunning);

        auto foundpair = _handlers.find(socket);
        if (foundpair != _handlers.end())
        {
            delete foundpair->second;
            _handlers.erase(foundpair);
        }
    }
}
