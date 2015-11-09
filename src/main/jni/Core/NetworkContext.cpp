#include "NetworkContext.h"

#include "NetworkService.h"
#include "Exception/Exception.h"
#include "Dispatch/Notifier.h"
#include "Core/NetworkMessages.h"

namespace Core
{
    namespace Impl
    {
        class NetworkServiceImpl : public NetworkService
        {
        private:
            Dispatch::SharedContext _networkContext;

        public:
            NetworkServiceImpl(const Dispatch::SharedContext& context)
                : _networkContext{ context }
            {}

        public:
            void request(std::int32_t id, const ConnectionString& connect, OnConnection *&& onConnection) override
            {
                if (onConnection == nullptr)
                {
                    throw Except::InvalidArgumentException{ "onConnection is null" };
                }

                _networkContext->Post(new ConnectionRequestMessage{ id, connect.ip, connect.port, connect.isIpv6, connect.isTcp, std::move(onConnection) });
            }

            void request(std::int32_t id, OnDisconnection *&& onDisconnection) override
            {
                if (onDisconnection == nullptr)
                {
                    throw Except::InvalidArgumentException{ "onConnection is null" };
                }

                _networkContext->Post(new DisconnectionRequestMessage{ id, std::move(onDisconnection) });
            }
        };
    }


    NetworkContext::NetworkContext(Dispatch::EventDispatcher& attachedDispatcher)
        : _dispatcher(attachedDispatcher)
    {}

    NetworkContext::~NetworkContext()
    {
        ShutdownAll();
    }

    void NetworkContext::AcquireNotifier(Dispatch::Notifier& notifier)
    {
        if (std::this_thread::get_id() != _contextTID)
        {
            throw Except::LogicException{ LOGIC_EXCEPT("Can not acquire notifier from other threads") };
        }

        Dispatch::Notifier::Make(notifier, &_dispatcher, this);
    }

    void NetworkContext::Add(std::uint32_t id, const ConnectedSocket& conn)
    {
        auto existed = _connections.find(id);

        if (existed != _connections.end())
        {
            Net::Socket socket = existed->second.first /* socket */;

            try
            {
                _reactor.RemoveHandler(socket);
                /* force to close */
                socket.Shutdown();
            }
            catch (const Except::Exception&)
            {
                /* */
            }

            _connections.erase(existed);
        }

        /* register to */
        _reactor.RegisterHandler(conn.first, conn.second);

        _connections[id] = conn;
    }

    void NetworkContext::Remove(std::uint32_t id)
    {
        auto existed = _connections.find(id);

        if (existed != _connections.end())
        {
            Net::Socket socket = existed->second.first /* socket */;

            try
            {
                _reactor.RemoveHandler(socket);
                /* socket is already closed */
            }
            catch (const Except::Exception&)
            {
                /* */
            }

            _connections.erase(existed);
        }
    }

    bool NetworkContext::Find(std::uint32_t id, ConnectedSocket& s)
    {
        auto found = _connections.find(id);

        if (found != _connections.end())
        {
            s = found->second;
            return true;
        }

        return false;
    }

    void NetworkContext::ShutdownAll()
    {
        for (auto conn : _connections)
        {
            Net::Socket socket = conn.second.first /* socket */;

            try
            {
                _reactor.RemoveHandler(socket);
                socket.Shutdown();
            }
            catch (const Except::Exception&)
            {
                /* */
            }
        }

        _connections.clear();
    }

    void NetworkContext::Run(const std::chrono::milliseconds& timeout)
    {
        _reactor.Run(timeout);
    }

    void *NetworkContext::GetRawCapability(std::uint16_t cap)
    {
        if (IS_CAPABILITY(cap, NetworkService))
        {
            return new Impl::NetworkServiceImpl{ shared_from_this() };
        }

        return nullptr;
    }
}

