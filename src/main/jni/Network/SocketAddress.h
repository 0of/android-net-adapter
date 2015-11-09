#ifndef SOCKETADDRESS_H
#define SOCKETADDRESS_H

#include <cstdint>
#include <string>

#include "globals.h"

#if defined(OS_WINDOWS)
#  include <winsock2.h>
#  include <ws2tcpip.h>
#  define NET_INVALID_SOCKET  INVALID_SOCKET
#  define NET_SOCKET_ERROR SOCKET_ERROR
#  define net_socketlen_t socklen_t

#elif defined(OS_LINUX)
/* linux */
#  include <unistd.h>
#  include <errno.h>
#  include <fcntl.h>
#  include <netdb.h>
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <sys/select.h>
#  include <sys/ioctl.h>
#  include <arpa/inet.h>
#  include <netinet/in.h>
#  include <netinet/tcp.h>
#  define SOCKET int
#  define NET_INVALID_SOCKET  -1
#  define NET_SOCKET_ERROR  -1
#  define net_socketlen_t socklen_t

#else
#  error "Not support"
#endif

namespace Net
{
    class SocketAddress
    {
    public:
        typedef union {
            struct sockaddr_in _ipv4;
            struct sockaddr_in6 _ipv6;
        } RawAddresss;

    private:
        RawAddresss _addr;

    public:
        SocketAddress();

        SocketAddress(const RawAddresss& rawAddress);
        /* default ipv4 */
        explicit SocketAddress(std::uint16_t port);
        SocketAddress(const std::string& hostAddress, std::uint16_t portNumber);
        SocketAddress(const SocketAddress& addr);

        ~SocketAddress();
        SocketAddress& operator = (const SocketAddress& socketAddress);

        std::string Host() const;
        std::uint16_t Port() const;

        bool operator == (const SocketAddress& socketAddress) const;
        bool operator != (const SocketAddress& socketAddress) const;

        /* port */
        std::uint32_t RawLength() const;
        const struct sockaddr* GetRawAddress() const;
    };
}

#endif  /* SOCKETADDRESS_H */