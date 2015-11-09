#include "NetworkManager_p.h"

#include "NetworkStatus.h"
#include "NetworkEvents.h"
#include "NetworkContext.h"
#include "Exception/Exception.h"
#include "Network/NetworkExceptions.h"
#include "Dispatch/Notifier.h"
#include "Dispatch/EventDispatcher.h"

namespace Core{
// Event handlers
    namespace NetImpl
    {
        #define EVENT_HANDLER_DEF(Class) \
            class Class : public Dispatch::EventHandler \
            { \
            private: \
                NetworkManager::PrivateData *_data; \
            public: \
                explicit Class(NetworkManager::PrivateData *data) \
                    : _data{ data }\
                {} \
            private: \
                virtual void Handle(Dispatch::Context& context, Dispatch::Event& ev) \
                { \
                    if (&context == &_data->_context) \
                    { \
                        auto event = event_cast<Class##Event>(&ev); \
                        if (!event) return; \
                        auto status = _data->GetStatus(event->_connID); \
                        if (!status) return; \
                        HandleEvent(event, status); \
                    } \
                } \
                \
                void HandleEvent(Class##Event *ev, NetworkStatus *status); \
            };

        EVENT_HANDLER_DEF(Connected)
        EVENT_HANDLER_DEF(Disconnected)

        void Connected::HandleEvent(ConnectedEvent *ev, NetworkStatus *status)
        {
            if (status->_connectingWaitable)
            {
                status->_connectingWaitable->SignalConnected();
                delete status->_connectingWaitable;
                status->_connectingWaitable = nullptr;
            }
        }

        void Disconnected::HandleEvent(DisconnectedEvent *ev, NetworkStatus *status)
        {
            // still connecting
            if (status->_connectingWaitable)
            {
                Net::SocketException connectedFailed;
                status->_connectingWaitable->SignalConnectError(connectedFailed);
            }
            else
            {
                // notify all read & write being closed
                Net::ConnectionAbortedException abortedException;
                // read
                for (auto eachRead : status->_readingWaitables)
                    eachRead->SignalReadError(abortedException);

                // write
                for (auto eachWrite : status->_sendingWaitables)
                    eachWrite.second->SignalSentError(abortedException, false);
            }

            // then notify disconnection
            if (status->_disconnectingWaitable)
            {
                status->_disconnectingWaitable->SignalDisconnected();

                // free resources
                _data->Close(ev->_connID);
            }
        }

        class NetworkExceptionEventHandler : public Dispatch::EventHandler
        {
        private:
            NetworkManager::PrivateData *_data;

        public:
            explicit NetworkExceptionEventHandler(NetworkManager::PrivateData *data)
                : _data{ data }
            {}

        private:
            virtual void Handle(Dispatch::Context& context, Dispatch::Event& ev)
            {
                if (&context == &_data->_context)
                {
                    auto event = event_cast<NetworkExceptionEvent>(&ev);
                    if (!event) return;

                    switch (event->_stage)
                    {
                    }
                }
            }

        private:

        };

    }
}