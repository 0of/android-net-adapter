#ifndef NETWORKSTATUS_H
#define NETWORKSTATUS_H

#include <chrono>

namespace Except
{
    class Exception;
}

namespace Core
{
        class SendingWaitable
        {
        public:
            virtual ~SendingWaitable() = default;

        public:
            virtual bool CheckTimeout(const std::chrono::milliseconds& elapsed) = 0;
            virtual void SignalSent() = 0;
            virtual void SignalSentError(Except::Exception& ex, bool isSending /* true means is sending current package */) = 0;
        };

        class ReadingWaitable
        {
        public:
            virtual ~ReadingWaitable() = default;

        public:
            virtual bool CheckTimeout(const std::chrono::milliseconds& elapsed) = 0;
            virtual bool Fulfill(RequestStream *readStream) = 0;
            virtual void SignalReadError(Except::Exception& ex) = 0;
        };

        class ConnectingWaitable
        {
        public:
            virtual ~ConnectingWaitable() = default;

        public:
            virtual bool CheckTimeout(const std::chrono::milliseconds& elapsed) = 0;
            virtual void SignalConnected() = 0;
            virtual void SignalConnectError(Except::Exception& ex) = 0;
        };

        class DisconnectingWaitable
        {
        public:
            virtual ~DisconnectingWaitable() = default;

        public:
            virtual void SignalDisconnected() = 0;
            virtual void SignalDisconnectError(Except::Exception& ex) = 0;
        };


    class NetworkStatus
    {
    public:
        // if not null, any sending or reading operations are forbidden
        ConnectingWaitable *_connectingWaitable;

        // if not null, any connecting sending or reading operations are forbidden
        DisconnectingWaitable *_disconnectingWaitable;

        std::map<RequestStream *, SendingWaitable *> _sendingWaitables;
        std::vector<ReadingWaitable *> _readingWaitables;

    public:
        ~NetworkStatus()
        {
            if (_connectingWaitable) delete _connectingWaitable;
            if (_disconnectingWaitable) delete _disconnectingWaitable;

            for (auto each : _sendingWaitables)
            {
                delete each.first;
                delete each.second;
            }

            for (auto each : _readingWaitables)
            {
                delete each;
            }
        }
    };
}

#endif