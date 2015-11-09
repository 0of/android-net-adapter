#ifndef SOCKET_H
#define SOCKET_H

#include <cstdint>
#include <chrono>

#include "SocketAddress.h"

namespace Net {

    class Socket
    {
    private:
        SOCKET _socket;

    public:
        /* init */
        static Socket Create(std::int32_t domain, std::int32_t type, std::int32_t protocol = 0);
        static std::int32_t LastError();
        static void OnError(const char *func);
        /* for debug use */
        #define OnSocketError() OnError(__FUNCTION__)

    public:
        Socket();
        /* will not shutdown the socket */
        ~Socket();

        bool operator == (const Socket& socket) const { return _socket == socket._socket; }
        bool operator != (const Socket& socket) const { return _socket != socket._socket; }

        /*
            Configurations
            */
        void SetSendBufferSize(std::int32_t size);
        std::int32_t GetSendBufferSize() const;

        void SetReceiveBufferSize(std::int32_t size);
        std::int32_t GetReceiveBufferSize() const;

        void SetSendTimeout(const std::chrono::milliseconds& timeout);
        std::chrono::milliseconds GetSendTimeout() const;

        void SetReceiveTimeout(const std::chrono::milliseconds& timeout);
        std::chrono::milliseconds GetReceiveTimeout() const;

        void SetOption(std::int32_t level, std::int32_t option, const std::int8_t * optval, std::int32_t optlen);
        void GetOption(std::int32_t level, std::int32_t option, std::int8_t * optval, std::int32_t *optlen) const;

        /* SO_LINGER */
        void SetLinger(bool on, const std::chrono::seconds& seconds);
        bool GetLinger(std::chrono::seconds& seconds) const;

        /* TCP_NODELAY */
        void SetNoDelay(bool enabled);
        bool GetNoDelay() const;

        /* SO_KEEPALIVE */
        void SetKeepAlive(bool enabled);
        bool GetKeepAlive() const;

        /* SO_REUSEADDR */
        void SetReuseAddress(bool enabled);
        bool GetReuseAddress() const;

        /* SO_OOBINLINE */
        void SetOOBInline(bool enabled);
        bool GetOOBInline() const;

        void SetNonBlocking(bool enabled);

        SocketAddress Address() const;

        std::uint32_t Available() const;

        void Shutdown();

        bool IsValid() const { return _socket != NET_SOCKET_ERROR; }

    public:
        operator SOCKET() const { return _socket; }

    protected:
        bool Init(std::int32_t af, std::int32_t type, std::int32_t protocol);
    };

    class TCPSocket
    {
    protected:
        Socket _socket;

    public:
        /* create default */
        static TCPSocket Create(std::int32_t domain);

    public:
        TCPSocket();
        explicit TCPSocket(const Socket& socket);
        virtual ~TCPSocket();

    public:
        operator bool() const { return _socket.IsValid(); }

        /*
            Connect/Send/Receive
        */
    public:
        virtual void Connect(const SocketAddress& address);

    public:
        virtual std::uint32_t Send(const std::uint8_t*buffer, std::uint32_t length, std::uint32_t flags = 0);
        virtual std::uint32_t Receive(std::uint8_t* buffer, std::uint32_t length, std::uint32_t flags = 0);

    public:
        Socket AsGeneralSocket() { return _socket; }

        /* call socket's shutdown */
        virtual void Close();

    protected:
        virtual bool Init(std::int32_t domain);

        virtual void OnError(const char *func);
    };

    class UDPSocket
    {
    protected:
        Socket _socket;

    public:
        /* create default */
        static UDPSocket Create(std::int32_t domain);

    public:
        UDPSocket();
        explicit UDPSocket(const Socket& socket);
        virtual ~UDPSocket();

    public:
        operator bool() const { return _socket.IsValid(); }

    public:
        virtual std::uint32_t SendTo(const SocketAddress& address, const std::uint8_t*buffer, std::uint32_t length, std::uint32_t flags = 0);
        virtual std::uint32_t ReceiveFrom(SocketAddress& address, std::uint8_t* buffer, std::uint32_t length, std::uint32_t flags = 0);
    
    public:
        Socket AsGeneralSocket() { return _socket; }

        /* call socket's shutdown */
        virtual void Close();

    protected:
        virtual bool Init(std::int32_t domain);

        virtual void OnError(const char *func);
    };
}

#endif // SOCKET_H
