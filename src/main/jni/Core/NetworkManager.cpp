#include "NetworkManager.h"

#include <map>

#include "NetworkMessages.h"
#include "NetworkEvents.h"
#include "NetworkContext.h"
#include "NetworkMiddleware.h"
#include "NetworkManageable.h"
#include "NetworkManager_p.h"
#include "NetworkResourceHelper_p.h"

#include "Exception/Exception.h"
#include "Dispatch/Notifier.h"
#include "Dispatch/EventDispatcher.h"

namespace Core
{
    namespace NetImpl
    {
        class IterableIncomingMiddleware : public IncomingMiddleware
        {
        private:
            std::vector<IncomingMiddleware *>::iterator _begin;
            std::vector<IncomingMiddleware *>::iterator _end;

        public:
            IterableIncomingMiddleware(const std::vector<IncomingMiddleware *>::iterator& begin,
                                       const std::vector<IncomingMiddleware *>::iterator& end)
                : _begin{ begin }
                , _end{ end }
            {}

            ~IterableIncomingMiddleware()
            {}

        public:
            virtual void OnIncoming(std::int32_t id, DirectAccessibleStream *readStream, IncomingMiddleware *next) override
            {
                if (this == next)
                {
                    if (_begin != _end)
                    {
                        auto realNext = *_begin++;
                        realNext->OnIncoming(id, readStream, this);
                    }
                }
                else
                {
                    next->OnIncoming(id, readStream, this);
                }
            }
        };
    }

    void NetworkManager::PrivateData::On(ConnectionRequestMessage *message, NetworkManageable *mgr)
    {
        using Creator = NetImpl::NetworkCreationHelper;

        if (message)
        {
            try
            {
                Creator creator{ _context, mgr };
                creator.Create(message->_connID, message->_ip, message->_domain == ConnectionRequestMessage::INET6 ? AF_INET6 : AF_INET,
                    message->_port, message->_type == ConnectionRequestMessage::TCP);
            }
            catch (const Except::Exception& ex)
            {
                mgr->NotifyException(message->_connID, Stage::BeforeConnect, ex);
            }
        }
    }

    void NetworkManager::PrivateData::Close(std::int32_t id)
    {
        auto statusIter = _status.find(id);
        if (statusIter != _status.end())
        {
            delete statusIter->second;
            _status.erase(statusIter);
        }

        _context.Remove(id);
    }

    class NetworkManager::CoreManager : public NetworkManageable
    {
    private:
        NetworkContext& _context;

    public:
        explicit CoreManager(NetworkContext& context)
            : _context(context)
        {}

    public:
        virtual void NotifyConnected(std::int32_t id) override final
        {
            Dispatch::Notifier notifier;
            _context.AcquireNotifier(notifier);

            ConnectedEvent ev{id};
            notifier.Notify(ev);
        }

        virtual void NotifyDisconnected(std::int32_t id) override final
        {
            Dispatch::Notifier notifier;
            _context.AcquireNotifier(notifier);

            DisconnectedEvent ev{id};
            notifier.Notify(ev);
        }

    public:
        virtual void NotifySent(std::int32_t id, RequestStream *sentStream) override final
        {
        }

        virtual void NotifyRead(std::int32_t id, RequestStream *readStream) override final
        {
        }

        virtual void NotifyException(std::int32_t id, Stage stage, const Except::Exception& ex) override final
        {
        }
    };


    NetworkManager::NetworkManager(NetworkContext& context, Dispatch::EventDispatcher& dispatcher)
        : _p{ new PrivateData{context} }
        , _mgr{ new CoreManager{context} }
        , _dispatcher(dispatcher)
    {

    }

    NetworkManager::~NetworkManager()
    {
        delete _p;
    }

    bool NetworkManager::Process(Dispatch::Message *msg)
    {
        using ConnectionMessage = ConnectionRequestMessage;
        using SocketHandler = SocketEventHandlerImpl;

        if (msg->Category() != Dispatch::Message::MessageCategory_User)
        {
            return false;
        }

        auto type = msg->GetType();
        if (ConnectionRequestMessage::Type == type)
        {
            _p->On(message_cast<ConnectionRequestMessage>(msg), _mgr);
            return true;
        }
        else
        {
        }

        return false;
    }

    void NetworkManager::Select()
    {
        try
        {
            _p->_context.Run(std::chrono::milliseconds{ 50 });
        }
        catch (const Except::TimeoutException& )
        {
            /* */
        }
    }

    void NetworkManager::Register(IncomingMiddleware *&& /* take the ownership */middleware)
    {
        _p->_incomingMiddlewareChain.push_back(std::move(middleware));
    }
}