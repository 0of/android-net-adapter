#ifndef TCPSOCKETEVENTHANDLER_P_H
#define TCPSOCKETEVENTHANDLER_P_H

#include "NetworkMessagesHandler.h"

namespace Core{

    class NetworkContext;

    namespace NetImpl
    {
        class TCPSocketEventHandlerImpl : public Core::SocketEventHandlerImpl
        {
            using Base = Core::SocketEventHandlerImpl;

        private:
            Core::NetworkContext &_context;
            bool _isConnected;

        public:
            TCPSocketEventHandlerImpl(NetworkContext& context, std::int32_t connID, std::uint16_t domain, std::uint16_t port, const std::string& addr);

        public:
            void Handle(const Net::Socket &socket, Net::Modes modes) NOEXCEPT override;

        private:
            void OnRead(Net::TCPSocket& tcpSocket);
        };
    }
}

#endif  /* TCPSOCKETEVENTHANDLER_P_H  */