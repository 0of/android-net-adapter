#include "SocketAddress.h"

#include <memory>
#include <cstring>

namespace Net
{
    SocketAddress::SocketAddress()
    {
        ::memset(&_addr, 0, sizeof (_addr));
    }

    SocketAddress::SocketAddress(const RawAddresss& rawAddress)
    {
        ::memcpy(&_addr, &rawAddress, sizeof (_addr));
    }

    SocketAddress::SocketAddress(std::uint16_t port)
    {
        ::memset(&_addr, 0, sizeof (_addr));

        _addr._ipv4.sin_family = AF_INET;
        _addr._ipv4.sin_port = port;
    }

    SocketAddress::SocketAddress(const std::string& hostAddress, std::uint16_t portNumber)
    {
        ::memset(&_addr, 0, sizeof (_addr));

#if defined(OS_WINDOWS)
        /* parse ipv4 */
        auto addr = inet_addr(hostAddress.c_str());
        if (addr != INADDR_NONE && addr != INADDR_ANY)
        {
            _addr._ipv4.sin_addr.S_un.S_addr = addr;
            _addr._ipv4.sin_port = portNumber;
            _addr._ipv4.sin_family = AF_INET;
        }
        else
        {
            /* parse ipv6*/
            struct addrinfo* addrInfo = NULL;
            struct addrinfo hints;
            std::memset(&hints, 0, sizeof(hints));
            hints.ai_flags = AI_NUMERICHOST;

            if (getaddrinfo(hostAddress.c_str(), NULL, &hints, &addrInfo) == 0)
            {
                std::memcpy(&_addr._ipv6.sin6_addr, addrInfo->ai_addr, addrInfo->ai_addrlen);
                /* scope ID */
                _addr._ipv6.sin6_scope_id = reinterpret_cast<struct sockaddr_in6*>(addrInfo->ai_addr)->sin6_scope_id;

                _addr._ipv6.sin6_port = portNumber;
                _addr._ipv6.sin6_family = AF_INET6;

                freeaddrinfo(addrInfo);
            }
        }
#else
        struct in_addr ia4;
        if (inet_pton(AF_INET, hostAddress.c_str(), &ia4) == 1 /* no error */)
        {
            std::memcpy(&_addr._ipv4.sin_addr, &ia4, sizeof(ia4));
            _addr._ipv4.sin_port = portNumber;
            _addr._ipv4.sin_family = AF_INET;
        }
        else
        {
            /* parse ipv6*/
            /* not support scope ID */
            struct in6_addr ia6;
            if (inet_pton(AF_INET6, hostAddress.c_str(), &ia6) == 1 /* no error */)
            {
                std::memcpy(&_addr._ipv6.sin6_addr, &ia6, sizeof(ia6));
                _addr._ipv6.sin6_port = portNumber;
                _addr._ipv6.sin6_family = AF_INET6;
            }
        }
#endif
    }

    SocketAddress::SocketAddress(const SocketAddress& addr)
    {
        ::memcpy(&_addr, &addr._addr, sizeof (addr._addr));
    }

    SocketAddress::~SocketAddress()
    {

    }

    SocketAddress& SocketAddress::operator = (const SocketAddress& socketAddress)
    {
        ::memcpy(&_addr, &socketAddress._addr, sizeof (socketAddress._addr));
        return *this;
    }

    std::string SocketAddress::Host() const
    {
        /* ip*/
        std::string hostString;

        auto addr = reinterpret_cast<const struct sockaddr*>(&_addr);

        std::int32_t addressLength = 0;
        if (addr->sa_family == AF_INET)
            addressLength = sizeof (_addr._ipv4);
        else if (addr->sa_family == AF_INET6)
            addressLength = sizeof (_addr._ipv6);

        if (!addressLength)
        {
#if defined(OS_WINDOWS)
            DWORD dwBufferLen = 0;

            /* XP sp1 later */
            if (WSAAddressToStringA(const_cast<LPSOCKADDR>(addr), addressLength, NULL, NULL, &dwBufferLen) == WSAEFAULT)
            {
                auto stringBuffer = std::make_unique<char[]>(dwBufferLen);
                if (stringBuffer)
                {
                    if (WSAAddressToStringA(const_cast<LPSOCKADDR>(addr), addressLength, NULL, stringBuffer.get(), &dwBufferLen) == 0 /* success */)
                    {
                        hostString = stringBuffer.get();
                    }
                }
            }
#elif defined(OS_LINUX)
            char stringBuffer[INET6_ADDRSTRLEN];
            if (0 == ::getnameinfo(addr, addressLength, stringBuffer, sizeof(stringBuffer), 0, 0, NI_NUMERICHOST))
            {
                hostString = stringBuffer;
            }
#else
#  error "TODO"
#endif
        }

        return hostString;
    }

    std::uint16_t SocketAddress::Port() const
    {
        /* same memory layout */
        return _addr._ipv6.sin6_port;
    }

    bool SocketAddress::operator == (const SocketAddress& socketAddress) const
    {
        return ::memcmp(&_addr, &socketAddress._addr, sizeof (_addr)) == 0;
    }

    bool SocketAddress::operator != (const SocketAddress& socketAddress) const
    {
        return ::memcmp(&_addr, &socketAddress._addr, sizeof (_addr)) != 0;
    }

    ///* port */
    std::uint32_t SocketAddress::RawLength() const
    {
        auto addr = reinterpret_cast<const struct sockaddr*>(&_addr);

        std::uint32_t len = 0;
        if (addr->sa_family == AF_INET)
        {
            len = sizeof (_addr._ipv4);
        }
        else if (addr->sa_family == AF_INET6)
        {
            len = sizeof (_addr._ipv6);
        }

        return len;
    }

    const struct sockaddr* SocketAddress::GetRawAddress() const
    {
        return reinterpret_cast<const struct sockaddr*>(&_addr);
    }
}