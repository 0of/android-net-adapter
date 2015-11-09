#include "ClientRunnable.h"

#include <cassert>
#include <array>

#include "NetworkContext.h"
#include "InitSynchronizer.h"
#include "NetworkMessagesHandler.h"

#include "Exception/Exception.h"
#include "Dispatch/ExceptionEvent.h"
#include "Dispatch/MessageQueueManager.h"
#include "Dispatch/RequestPacketQueue.h"
#include "Dispatch/EventDispatcher.h"
#include "Dispatch/EventHandler.h"

#include "Network/SocketReactor.h"

#define CLASS_NAME(class) #class

namespace Core
{
    InitNotifier::InitNotifier(InitSynchronizer *s)
        : _sync{ s }
    {}

    void InitNotifier::Initialized(ClientRunnable& runnable)
    {
        assert(_sync != nullptr);

        OnInitialized(runnable);
        _sync->Notify();
    }

    void InitNotifier::OnInitialized(ClientRunnable& )
    {

    }

    ClientRunnable::ClientRunnable()
    {
    }

    ClientRunnable::~ClientRunnable()
    {
    }

    Dispatch::SharedContext ClientRunnable::Context() const
    {
        return _context;
    }

    void ClientRunnable::Run(InitNotifier &initNotifier)
    {
        static Dispatch::QueueIdentifier ClientQueueIdentifier = std::this_thread::get_id();

        auto tid = std::this_thread::get_id();

        /* init dispach */
        Dispatch::EventDispatcher dispatcher(tid);
        _dispatcher = &dispatcher;

        _queue = std::make_shared<SharedRequestPacketQueue::element_type>(tid);
        _context = std::make_shared<NetworkContext>(dispatcher);

        auto& queueMgr = Dispatch::MessageQueueManager::GetInstance();

        if (_queue && _context)
        {
            /* register */
            queueMgr.RegisterMessageQueue(ClientQueueIdentifier, _queue);

            /* notify initialization is done */
            initNotifier.Initialized(*this);

            /* start to run */
            OnRun();
        }

        queueMgr.UnregisterMessageQueue(ClientQueueIdentifier);
    }

    void ClientRunnable::OnRun()
    {
        using MessagesBuffer = std::array<Dispatch::UnreadMessage, 8>;
        using LocalMessage = std::unique_ptr<Dispatch::Message>;

        bool processed = false;

        MessagesBuffer messages;

        auto first = messages.begin();
        auto last = first;

        FOREVER
        {
            while (first != last)
            {
                const auto& unreadMsg = *first++;

                /* message context */
                auto context = unreadMsg.first;
                /* message content */
                LocalMessage msg{ unreadMsg.second };

                processed = false;

                /* normal message */
                if (msg->Category() == Dispatch::Message::MessageCategory_User)
                {
                    if (processed)
                    {
                        try
                        {
                            processed = NetworkMessagesHandler::Handle(*_dispatcher, /* give our network context */*_context, msg.get());
                        }
                        catch (const Except::Exception& )
                        {
                            /* */
                        }
                    }
                }
                else if (msg->Category() == Dispatch::Message::MessageCategory_Event &&
                    msg->GetType() == Dispatch::EventMessage::EventContainer)
                {
                    /* pass to dispatch */
                    _dispatcher->Dispatch(*context, *static_cast<Dispatch::EventMessage *>(msg.get()));
                }
            }

            first = messages.begin();
            last = _queue->TakeSome(first, messages.end());

            try
            {
                _context->Run(std::chrono::milliseconds{ 50 });
            }
            catch (const Except::TimeoutException& )
            {
                /* */
            }
        }

        ///*! exception safe !*/
        ///* clean up */
        while (first != last)
        {
            auto msg = (*first++).second;
            /* delete messages */
            delete msg;
        }

        _context->ShutdownAll();
    }

    Dispatch::QueueIdentifier ClientRunnable::RegisteredQueueIdentifier() const
    {
        return _queue->AttachedThreadID();
    }

}