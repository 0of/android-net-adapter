#include "NetworkResourceHelper_p.h"

#include "TCPSocketEventHandler_p.h"
#include "NetworkContext.h"
#include "Network/NetworkExceptions.h"

namespace Core
{
    namespace NetImpl
    {
        class AutoConnectionSocket
        {
            ConnectedSocket _connectionSocket;

        public:
            AutoConnectionSocket() = default;
            ~AutoConnectionSocket();

        public:
            void Reset();

        public:
            Net::Socket& Socket() { return _connectionSocket.first; }
            SocketEventHandlerImpl *& Handler() { return _connectionSocket.second; }

            const ConnectedSocket& GetConnectedSocket() const { return _connectionSocket; }

        public:
            /* check is valid */
            operator bool() const { return _connectionSocket.first.IsValid() && _connectionSocket.second != nullptr; }

        private:
            void* operator new(size_t) = delete;
            void operator delete(void*) = delete;
        };

        AutoConnectionSocket::~AutoConnectionSocket()
        {
            if (_connectionSocket.first.IsValid())
                _connectionSocket.first.Shutdown();

            if (_connectionSocket.second)
                delete _connectionSocket.second;
        }

        void AutoConnectionSocket::Reset()
        {
            _connectionSocket.first = Net::Socket{};
            _connectionSocket.second = nullptr;
        }

        NetworkCreationHelper::NetworkCreationHelper(NetworkContext& context, NetworkManageable *mgr)
            : _context(context)
            , _mgr{ mgr }
        {}

        void NetworkCreationHelper::Create(std::int32_t id, const std::string& host, std::uint16_t af, std::uint16_t port, bool isTCP)
        {
            AutoConnectionSocket created;

            if (isTCP)
            {
                auto tcpSocket = Net::TCPSocket::Create(af);
                if (tcpSocket)  /* success */
                {
                    created.Socket() = tcpSocket.AsGeneralSocket();
                    created.Handler() = new NetImpl::TCPSocketEventHandlerImpl{ _context, id, af, port, host };
                }
            }
            else
            {
                /* UDP */
//                auto udpSocket = Net::UDPSocket::Create(message->_domain == ConnectionMessage::INET6 ? AF_INET6 : AF_INET);
//                if (udpSocket)  /* success */
//                {
//                    created.Socket() = udpSocket.AsGeneralSocket();
//                    created.Handler() = new NetImpl::UDPSocketEventHandlerImpl{ ctx, dispatcher, message->_connID, af, message->_port, message->_ip };
//                }
            }

            if (!created)
            {
                // created failed
                throw Net::SocketException{ "socket created failed" };
            }

            /* create OK */
            _context.Add(id, created.GetConnectedSocket());
            created.Reset();
        }
    }
}