#include "Socket.h"

#include <fcntl.h>

#include <limits>

#include "NetworkExceptions.h"

#ifndef NETERRNO
#  if defined(OS_WINDOWS)
#    define NETERRNO(errno) WSA##errno
#  else
#    define NETERRNO(errno) errno
#  endif  /* NETERRNO */
#endif

/* Socket */
namespace Net
{
    Socket Socket::Create(std::int32_t domain, std::int32_t type, std::int32_t protocol)
    {
        Socket socket;
        if (!socket.Init(domain, type, protocol))
            socket.OnSocketError(); /* throw the exception*/

        return socket;
    }

    Socket::Socket()
        : _socket(NET_INVALID_SOCKET)
    {

    }

    Socket::~Socket()
    {

    }

    void Socket::SetSendBufferSize(std::int32_t size)
    {
        SetOption(SOL_SOCKET, SO_SNDBUF, reinterpret_cast<const std::int8_t *>(&size), sizeof(size));
    }

    std::int32_t Socket::GetSendBufferSize() const
    {
        std::int32_t bufferSize = 0;
        net_socketlen_t inputLength = sizeof (bufferSize);

        GetOption(SOL_SOCKET, SO_SNDBUF, reinterpret_cast<std::int8_t *>(&bufferSize), &inputLength);

        return bufferSize;
    }

    void Socket::SetReceiveBufferSize(std::int32_t size)
    {
        SetOption(SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const std::int8_t *>(&size), sizeof(size));
    }

    std::int32_t Socket::GetReceiveBufferSize() const
    {
        std::int32_t bufferSize = 0;
        net_socketlen_t inputLength = sizeof (bufferSize);

        GetOption(SOL_SOCKET, SO_RCVBUF, reinterpret_cast<std::int8_t *>(&bufferSize), &inputLength);

        return bufferSize;
    }

    void Socket::SetSendTimeout(const std::chrono::milliseconds& timeout)
    {
#pragma push_macro("max")
#undef max
        std::chrono::milliseconds::rep maxTimespan = std::numeric_limits<std::int32_t>::max();
#pragma pop_macro("max")

        if (timeout.count() > maxTimespan)
        {
            /* throw invalid argument */
            throw Except::InvalidArgumentException{ LOGIC_EXCEPT("Invalid timeout value") };
        }

#if defined(OS_WINDOWS)
        DWORD timespan = static_cast<decltype(timespan)>(timeout.count());
#else
        timeval timespan;
        timespan.tv_sec = timeout.count() / 1000;
        timespan.tv_usec = (timeout.count() % 1000) * 1000;
#endif

        SetOption(SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const std::int8_t *>(&timespan), sizeof(timespan));
    }

    std::chrono::milliseconds Socket::GetSendTimeout() const
    {
#if defined(OS_WINDOWS)
        DWORD timespan = 0;
#else
        timeval timespan;
        timespan.tv_sec = 0;
        timespan.tv_usec = 0;
#endif

        net_socketlen_t sizeofTimespan = sizeof(timespan);
        GetOption(SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<std::int8_t *>(&timespan), &sizeofTimespan);

#if defined(OS_WINDOWS)
        return std::chrono::milliseconds(timespan);
#else
        return std::chrono::milliseconds(timespan.tv_sec * 1000 + timespan.tv_usec / 1000);
#endif
    }

    void Socket::SetOption(std::int32_t level, std::int32_t option, const std::int8_t * optval, std::int32_t optlen)
    {
        if (_socket == NET_INVALID_SOCKET)
            /* throw invalid socket */
            throw InvalidSocketException{ LOGIC_EXCEPT("Invalid socket") };

        auto rc = ::setsockopt(_socket, level, option, reinterpret_cast<const char *>(optval), optlen);

        if (rc == NET_SOCKET_ERROR)
            OnSocketError();
    }

    void Socket::GetOption(std::int32_t level, std::int32_t option, std::int8_t * optval, std::int32_t *optlen) const
    {
        if (_socket == NET_INVALID_SOCKET)
            /* throw invalid socket */
            throw InvalidSocketException{ LOGIC_EXCEPT("Invalid socket") };

        auto rc = ::getsockopt(_socket, level, option, reinterpret_cast<char*>(optval), optlen);

        if (rc == NET_SOCKET_ERROR)
            OnSocketError();
    }

    void Socket::SetLinger(bool on, const std::chrono::seconds& seconds)
    {
        struct linger l;

#pragma push_macro("max")
#undef max
        std::chrono::milliseconds::rep maxTimespan = std::numeric_limits<decltype(l.l_linger)>::max();
#pragma pop_macro("max")

        if (seconds.count() > maxTimespan)
        {
            /* throw invalid argument */
        }

        l.l_onoff = on ? 1 : 0;
        l.l_linger = static_cast<decltype(l.l_linger)>(seconds.count());

        SetOption(SOL_SOCKET, SO_LINGER, reinterpret_cast<const std::int8_t *>(&l), sizeof(l));
    }

    bool Socket::GetLinger(std::chrono::seconds& seconds) const
    {
        struct linger l;
        net_socketlen_t len = sizeof(l);

        GetOption(SOL_SOCKET, SO_LINGER, reinterpret_cast<std::int8_t *>(&l), &len);

        seconds = std::chrono::seconds(l.l_linger);
        return 1 == l.l_onoff;
    }

    void Socket::SetNoDelay(bool enabled)
    {
#if defined(OS_WINDOWS)
        BOOL isEnabled = enabled ? TRUE : FALSE;
#else
        std::int32_t isEnabled = enabled ? 1 : 0;
#endif

        SetOption(IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const std::int8_t *>(&isEnabled), sizeof(isEnabled));
    }

    bool Socket::GetNoDelay() const
    {
#if defined(OS_WINDOWS)
        BOOL isEnabled = FALSE;
#else
        std::int32_t isEnabled = 0;
#endif
        net_socketlen_t len = sizeof(isEnabled);

        GetOption(IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<std::int8_t *>(&isEnabled), &len);

#if defined(OS_WINDOWS)
        return TRUE == isEnabled;
#else
        return 0 != isEnabled;
#endif
    }

    void Socket::SetKeepAlive(bool enabled)
    {
#if defined(OS_WINDOWS)
        DWORD isEnabled = enabled ? TRUE : FALSE;
#else
        std::int32_t isEnabled = enabled ? 1 : 0;
#endif

        SetOption(SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<const std::int8_t *>(&isEnabled), sizeof(isEnabled));
    }

    bool Socket::GetKeepAlive() const
    {
#if defined(OS_WINDOWS)
        DWORD isEnabled = FALSE;
#else
        std::int32_t isEnabled = 0;
#endif
        net_socketlen_t len = sizeof(isEnabled);

        GetOption(SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<std::int8_t *>(&isEnabled), &len);

#if defined(OS_WINDOWS)
        return TRUE == isEnabled;
#else
        return 0 != isEnabled;
#endif
    }

    void Socket::SetReuseAddress(bool enabled)
    {
#if defined(OS_WINDOWS)
        DWORD isEnabled = enabled ? TRUE : FALSE;
#else
        std::int32_t isEnabled = enabled ? 1 : 0;
#endif

        SetOption(SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const std::int8_t *>(&isEnabled), sizeof(isEnabled));
    }

    bool Socket::GetReuseAddress() const
    {
#if defined(OS_WINDOWS)
        DWORD isEnabled = FALSE;
#else
        std::int32_t isEnabled = 0;
#endif
        net_socketlen_t len = sizeof(isEnabled);

        GetOption(SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<std::int8_t *>(&isEnabled), &len);

#if defined(OS_WINDOWS)
        return TRUE == isEnabled;
#else
        return 0 != isEnabled;
#endif
    }

    void Socket::SetOOBInline(bool enabled)
    {
#if defined(OS_WINDOWS)
        DWORD isEnabled = enabled ? TRUE : FALSE;
#else
        std::int32_t isEnabled = enabled ? 1 : 0;
#endif

        SetOption(SOL_SOCKET, SO_OOBINLINE, reinterpret_cast<const std::int8_t *>(&isEnabled), sizeof(isEnabled));
    }

    bool Socket::GetOOBInline() const
    {
#if defined(OS_WINDOWS)
        DWORD isEnabled = FALSE;
#else
        std::int32_t isEnabled = 0;
#endif
        net_socketlen_t len = sizeof(isEnabled);

        GetOption(SOL_SOCKET, SO_OOBINLINE, reinterpret_cast<std::int8_t *>(&isEnabled), &len);

#if defined(OS_WINDOWS)
        return TRUE == isEnabled;
#else
        return 0 != isEnabled;
#endif
    }

    void Socket::SetNonBlocking(bool enabled)
    {
        if (_socket == NET_INVALID_SOCKET)
            /* throw invalid socket */
            throw InvalidSocketException{ LOGIC_EXCEPT("Invalid socket") };

#if defined(OS_WINDOWS)
        u_long isEnabled = enabled ? 1 : 0;

        if (NET_SOCKET_ERROR == ioctlsocket(_socket, FIONBIO, &isEnabled))
            OnSocketError();
#else
        auto rc = ::fcntl(_socket, F_GETFL);
        if (rc == -1) OnSocketError();

        if (enabled)
        {
            rc |= O_NONBLOCK;
        }
        else
        {
            rc &= ~O_NONBLOCK;
        }

        (void)fcntl(F_SETFL, rc);
#endif
    }

    SocketAddress Socket::Address() const
    {
        if (_socket == NET_INVALID_SOCKET)
            /* throw invalid socket */
            throw InvalidSocketException{ LOGIC_EXCEPT("Invalid socket") };

        SocketAddress::RawAddresss rawAddr;
        net_socketlen_t len = sizeof(rawAddr);

        auto rc = ::getsockname(_socket, reinterpret_cast<struct sockaddr *>(&rawAddr), &len);

        if (rc == NET_SOCKET_ERROR)
            OnSocketError();

        return rawAddr;
    }

    std::uint32_t Socket::Available() const
    {
        if (_socket == NET_INVALID_SOCKET)
            /* throw invalid socket */
            throw InvalidSocketException{ LOGIC_EXCEPT("Invalid socket") };

#if defined(OS_WINDOWS)
        u_long avail = 0;
        auto rc = ioctlsocket(_socket, FIONREAD, reinterpret_cast<u_long*>(&avail));
#else
        std::int32_t avail = 0;
        auto rc = ::ioctl(_socket, FIONREAD, &avail);
#endif

        if (rc == NET_SOCKET_ERROR)
            OnSocketError();

        return static_cast<std::uint32_t>(avail);
    }

    void Socket::Shutdown()
    {
        if (_socket == NET_INVALID_SOCKET)
            return;

        auto rc = ::shutdown(_socket, 2);

        if (rc == NET_SOCKET_ERROR)
            OnSocketError();
    }

    bool Socket::Init(std::int32_t af, std::int32_t type, std::int32_t protocol)
    {
        if (_socket != NET_INVALID_SOCKET)
            return false;

        _socket = ::socket(af, type, protocol);
        return _socket != NET_INVALID_SOCKET;
    }

    std::int32_t Socket::LastError()
    {
#if defined(OS_WINDOWS)
        return WSAGetLastError();
#else
        return errno;
#endif
    }

    void Socket::OnError(const char *func)
    {
        auto err = LastError();

        switch (err)
        {
#ifdef OS_WINDOWS
        case NETERRNO(SYSNOTREADY):
            throw NetException{ "Network subsystem is unavailable" };
        case  NETERRNO(NOTINITIALISED):
            throw NetException{ "Network subsystem not initialized" };
#endif
        case NETERRNO(EINTR):
            throw SocketException{"Socket is closed or a process is terminated"};
        case NETERRNO(EACCES):
            throw Except::PermissionDeniedException{ "An attempt was made to access a socket in a way forbidden by its access permissions" };
        case NETERRNO(EFAULT):
            throw SocketException{"Bad address"};
        case NETERRNO(EINVAL):
            throw Except::InvalidArgumentException{ func };
        case NETERRNO(EMFILE):
            throw SocketException{ "Too many open files. Too many open sockets" };
        case NETERRNO(EWOULDBLOCK):
            throw SocketException{ "Operations on nonblocking sockets that cannot be completed immediately" };
        case NETERRNO(EINPROGRESS):
            throw SocketException{ "Operation now in progress" };
        case NETERRNO(EALREADY):
            throw SocketException{ "Operation already in progress" };
        case NETERRNO(ENOTSOCK):
            throw SocketException{"Socket operation on nonsocket"};
        case NETERRNO(ENOBUFS):
            throw Except::IOException{"No buffer space available"};

        case NETERRNO(ETIMEDOUT):
            throw Except::TimeoutException{};

        case NETERRNO(ECONNABORTED):
            throw ConnectionAbortedException{};
        case NETERRNO(ECONNRESET):
            throw ConnectionResetException{};
        case NETERRNO(ECONNREFUSED):
            throw ConnectionRefusedException{};

        case NETERRNO(EDESTADDRREQ):
            throw NetException{"Destination address required"};
        case NETERRNO(EMSGSIZE):
            throw NetException{ "Message too long" };
        case NETERRNO(EPROTOTYPE):
            throw NetException{ "Wrong protocol type" };
        case NETERRNO(ENOPROTOOPT):
            throw NetException{ "Protocol not available" };
        case NETERRNO(EPROTONOSUPPORT):
            throw NetException{ "Protocol not supported" };
        case NETERRNO(ESOCKTNOSUPPORT):
            throw NetException{ "Socket type not supported" };
        case NETERRNO(EOPNOTSUPP):
            throw NetException{ "Operation not supported" };
        case NETERRNO(EPFNOSUPPORT):
            throw NetException{ "Protocol family not supported" };
        case NETERRNO(EAFNOSUPPORT):
            throw NetException{ "Address family not supported" };
        case NETERRNO(EADDRINUSE):  /* connect */
            throw NetException{ "Address already in use" };
        case NETERRNO(EADDRNOTAVAIL):    /* connect, sendto */
            throw NetException{ "Cannot assign requested address" };
        case NETERRNO(ENETDOWN):
            throw NetException{ "Network is down" };
        case NETERRNO(ENETUNREACH):
            throw NetException{ "Network is unreachable" };
        case NETERRNO(ENETRESET):
            throw NetException{ "Network dropped connection on reset" };
        case NETERRNO(EISCONN):
            throw NetException{ "Socket is already connected" };
        case NETERRNO(ENOTCONN):
            throw NetException{ "Socket is not connected" };
        case NETERRNO(ESHUTDOWN):
            throw NetException{ "Cannot send after socket shutdown" };
        case NETERRNO(EHOSTDOWN):
            throw NetException{ "Host is down" };
        case NETERRNO(EHOSTUNREACH):
            throw NetException{ "No route to host" };
        }
    }
}

/* TCP socket */
namespace Net
{
    TCPSocket TCPSocket::Create(std::int32_t domain)
    {
        TCPSocket socket;
        socket.Init(domain);

        return socket;
    }

    TCPSocket::TCPSocket()
        : _socket()
    {

    }

    TCPSocket::TCPSocket(const Socket& socket)
        : _socket(socket)
    {

    }

    TCPSocket::~TCPSocket()
    {

    }

    void TCPSocket::Connect(const SocketAddress& address)
    {
        if (!_socket.IsValid())
            /* throw invalid socket */
            throw InvalidSocketException{ LOGIC_EXCEPT("Invalid socket") };

        auto rc = 0;

        do
        {
            rc = ::connect(_socket, address.GetRawAddress(), address.RawLength());
        }
        while (rc == NET_SOCKET_ERROR && Socket::LastError() == NETERRNO(EINTR));

        if (rc == NET_SOCKET_ERROR)
            OnSocketError();
    }

    std::uint32_t TCPSocket::Send(const std::uint8_t*buffer, std::uint32_t length, std::uint32_t flags)
    {
        if (!_socket.IsValid())
            throw InvalidSocketException{ LOGIC_EXCEPT("Invalid socket") };

        auto rc = 0;

        do
        {
            rc = ::send(_socket, reinterpret_cast<const char *>(buffer), length, flags);
        }
        while (rc < 0 && Socket::LastError() == NETERRNO(EINTR));

        if (rc < 0)
        {
            OnSocketError();
            rc = 0;
        }

        return static_cast<std::uint32_t>(rc);
    }

    std::uint32_t TCPSocket::Receive(std::uint8_t* buffer, std::uint32_t length, std::uint32_t flags)
    {
        if (!_socket.IsValid())
            throw InvalidSocketException{ LOGIC_EXCEPT("Invalid socket") };

        auto rc = 0;

        do
        {
            rc = ::recv(_socket, reinterpret_cast<char *>(buffer), length, flags);
        } while (rc < 0 && Socket::LastError() == NETERRNO(EINTR));

        if (rc < 0)
        {
            OnSocketError();
            rc = 0;
        }

        return static_cast<std::uint32_t>(rc);
    }

    /* call socket's shutdown */
    void TCPSocket::Close()
    {
        _socket.Shutdown();
    }

    bool TCPSocket::Init(std::int32_t domain)
    {
        if (!_socket.IsValid())
        {
            _socket = Socket::Create(domain, SOCK_STREAM);
        }

        return false;
    }

    void TCPSocket::OnError(const char *func)
    {
        Socket::OnError(func);
    }
}

namespace Net
{
    UDPSocket UDPSocket::Create(std::int32_t domain)
    {
        UDPSocket socket;
        socket.Init(domain);

        return socket;
    }

    UDPSocket::UDPSocket()
        : _socket()
    {

    }

    UDPSocket::UDPSocket(const Socket& socket)
        : _socket(socket)
    {

    }

    UDPSocket::~UDPSocket()
    {

    }

    std::uint32_t UDPSocket::SendTo(const SocketAddress& address, const std::uint8_t*buffer, std::uint32_t length, std::uint32_t flags)
    {
        if (!_socket.IsValid())
            throw InvalidSocketException{ LOGIC_EXCEPT("Invalid socket") };

        auto rc = 0;

        do
        {
            rc = ::sendto(_socket, reinterpret_cast<const char*>(buffer), length, flags, address.GetRawAddress(), address.RawLength());
        } 
        while (rc < 0 && Socket::LastError() == NETERRNO(EINTR));

        if (rc < 0)
        {
            OnSocketError();
            rc = 0;
        }

        return static_cast<std::uint32_t>(rc);
    }

    std::uint32_t UDPSocket::ReceiveFrom(SocketAddress& address, std::uint8_t* buffer, std::uint32_t length, std::uint32_t flags)
    {
        if (!_socket.IsValid())
            throw InvalidSocketException{ LOGIC_EXCEPT("Invalid socket") };

        auto rc = 0;

        /* raw address */
        SocketAddress::RawAddresss rawAddr;
        net_socketlen_t len = sizeof(rawAddr);

        do
        {
            rc = ::recvfrom(_socket, reinterpret_cast<char*>(buffer), length, flags, reinterpret_cast<struct sockaddr *>(&rawAddr), &len);
        } 
        while (rc < 0 && Socket::LastError() == NETERRNO(EINTR));

        if (rc < 0)
        {
            OnSocketError();
            rc = 0;
        }
        else
        {
            address = rawAddr;
        }

        return static_cast<std::uint32_t>(rc);
    }

    void UDPSocket::Close()
    {
        _socket.Shutdown();
    }

    bool UDPSocket::Init(std::int32_t domain)
    {
        if (!_socket.IsValid())
        {
            _socket = Socket::Create(domain, SOCK_DGRAM);
        }

        return false;
    }

    void UDPSocket::OnError(const char *func)
    {
        Socket::OnError(func);
    }
}