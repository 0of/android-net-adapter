#include "TCPSocketEventHandler_p.h"

#include <cassert>

#include "NetworkMessagesHandler.h"
#include "NetworkEvents.h"
#include "NetworkContext.h"
#include "NetworkTransportObject.h"
#include "Exception/Exception.h"
#include "Dispatch/Notifier.h"

namespace Core{

    namespace NetImpl
    {
            TCPSocketEventHandlerImpl::TCPSocketEventHandlerImpl(Core::NetworkContext & context, std::int32_t connID, std::uint16_t domain, std::uint16_t port, const std::string& addr)
                : Base(connID, domain, port, addr)
    #if defined(COMPILER_GCC) && (__GNUC__ * 10 + __GNUC_MINOR__ <= 48)
                /* GCC 4.8 bug */
                , _context(context)
    #else
                , _context{ context }
    #endif
                ,_isConnected{ false }
            {}

             void TCPSocketEventHandlerImpl::Handle(const Net::Socket &socket, Net::Modes modes) NOEXCEPT
            {
                if (modes & Net::Read)
                {
                    if (!_buffer)
                        throw Except::OutOfMemoryException{};

                    Net::TCPSocket tcpSocket{ socket };
                    OnRead(tcpSocket);
                }
                else if (modes & Net::Write)
                {
                    if (!_buffer)
                        throw Except::OutOfMemoryException{};

                    // check if connected
                    if (!_isConnected)
                    {
                        // do connect
                        Net::TCPSocket tcpSocket{ socket };
                        Net::SocketAddress connectAddress{ _token.address, _token.port };

                        try
                        {
                            tcpSocket.Connect(connectAddress);
                            // connected
                            _isConnected = true;

                            Dispatch::Notifier notifier;
                            _context.AcquireNotifier(notifier);

                            //Core::SentRequestEvent ev{ _token.id, accumulatedCount };
                            //notifier.Notify(ev);
                        }
                        catch(const Except::IOException&)
                        {

                        }
                    }

                    if (!_queue.empty())
                        return;

                    auto task = _queue.front();
                    /* remove */
                    _queue.pop_front();

                    std::unique_ptr<Stream> stream{ task.second };

                    Net::TCPSocket tcpSocket{ socket };

                    std::uint32_t accumulatedCount = 0;
                    std::uint32_t readCount = 0;
                    try
                    {
                        while ((readCount = stream->Read(_buffer, _len)) != 0)
                        {
                            tcpSocket.Send(_buffer, readCount);
                            accumulatedCount += readCount;
                        }

                        /* sent event */
    //                    Dispatch::Notifier notifier;
    //                    _context.AcquireNotifier(notifier);
    //
    //                    Core::SentRequestEvent ev{ _token.id, accumulatedCount };
    //                    notifier.Notify(ev);
                    }
                    catch (const Except::IOException&)
                    {
                        try
                        {
                            tcpSocket.Close();
                        }
                        catch (const Except::Exception&)
                        {
                        }
                        _isConnected = false;
                        _context.Remove(_token.id);

                        /* close event */
    //                    Dispatch::Notifier notifier;
    //                    _context.AcquireNotifier(notifier);
    //
    //                    Core::DisconnectedEvent ev{ _token.id };
    //                    notifier.Notify(ev);
                    }
                }
            }


        void TCPSocketEventHandlerImpl::OnRead(Net::TCPSocket& tcpSocket)
        {
            bool needClose = false;

            try
            {
                auto received = tcpSocket.Receive(_buffer, _len);

                if (received > 0)
                {
                    auto stream = std::make_shared<MemoryResponseStream>(received);
                    if (stream && stream->_buffer)  /* allocate successfully */
                    {
                        ::memcpy(stream->_buffer, _buffer, received);
                        stream->_readableLen = received;
                        stream->_offset = 0;

//                       Dispatch::Notifier notifier;
//                        _context.AcquireNotifier(notifier);
//                        Core::ReceiveResponseEvent ev{ _token.id, stream };
//                        notifier.Notify(ev);
                    }
                    else
                    {
                        throw Except::OutOfMemoryException{};
                    }
                }
                else
                {
                    /* being shutdown */
                    needClose = true;
                }
            }
            catch (const Except::IOException&)
            {
                needClose = true;
            }

            if (needClose)
            {
                try
                {
                    tcpSocket.Close();
                }
                catch (const Except::Exception&)
                {
                }
                _isConnected = false;
                _context.Remove(_token.id);

//                Dispatch::Notifier notifier;
//                _context.AcquireNotifier(notifier);
//                /* close event */
//                Core::DisconnectedEvent ev{ _token.id };
//                notifier.Notify(ev);
            }
        }
}
}

