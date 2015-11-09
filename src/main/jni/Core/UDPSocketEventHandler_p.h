#ifndef UDPSOCKETEVENTHANDLER_P_H
#define UDPSOCKETEVENTHANDLER_P_H

#include "NetworkMessagesHandler.h"

namespace Core{

    class NetworkContext;

    namespace NetImpl
    {
        class UDPSocketEventHandlerImpl : public Core::SocketEventHandlerImpl
        {
            using Base = Core::SocketEventHandlerImpl;

        private:
            Net::SocketAddress _peerAddress;

            Core::NetworkContext &_context;
            bool _isConnected;

        public:
            UDPSocketEventHandlerImpl(NetworkContext& context, std::int32_t connID, std::uint16_t domain, std::uint16_t port, const std::string& addr);

        public:
            void Handle(const Net::Socket &socket, Net::Modes modes) NOEXCEPT override;

        private:
            void OnRead(Net::UDPSocket& tcpSocket);
        };
    }
}

#endif  /*  UDPSOCKETEVENTHANDLER_P_H  */