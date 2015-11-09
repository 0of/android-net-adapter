#include "UDPSocketEventHandler_p.h"

#include <cassert>

#include "NetworkMessagesHandler.h"
#include "NetworkEvents.h"
#include "NetworkContext.h"
#include "NetworkTransportObject.h"
#include "Exception/Exception.h"
#include "Dispatch/Notifier.h"

namespace Core
{
    namespace NetImpl
    {
         UDPSocketEventHandlerImpl::UDPSocketEventHandlerImpl(Core::NetworkContext & context, std::int32_t connID,
                                                              std::uint16_t domain, std::uint16_t port, const std::string& addr)
            : Base(connID, domain, port, addr)
#if defined(COMPILER_GCC) && (__GNUC__ * 10 + __GNUC_MINOR__ <= 48)
            /* GCC 4.8 bug */
            , _context(context)
#else
            , _context{ context }
#endif
            , _peerAddress{ addr, port }
        {}

        void UDPSocketEventHandlerImpl::Handle(const Net::Socket &socket, Net::Modes modes) NOEXCEPT
        {
            if (modes & Net::Read)
            {
                if (!_buffer)
                    throw Except::OutOfMemoryException{};

                Net::UDPSocket udpSocket{ socket };

                OnRead(udpSocket);
            }
            else if (modes & Net::Write && !_queue.empty())
            {
                if (!_buffer)
                    throw Except::OutOfMemoryException{};

                auto task = _queue.front();
                /* remove */
                _queue.pop_front();

                std::unique_ptr<Stream> stream{ task.second };

                Net::UDPSocket udpSocket{ socket };
                Net::SocketAddress sendAddress{ _token.address, _token.port };

                std::uint32_t readCount = 0;
                std::uint32_t accumulatedCount = 0;

                try
                {
                    while ((readCount = stream->Read(_buffer, _len)) != 0)
                    {
                        udpSocket.SendTo(sendAddress, _buffer, readCount);
                        accumulatedCount += readCount;
                    }
//
//                    Core::SentRequestEvent ev{ _token.id, accumulatedCount };
//                    _dispatcher.Dispatch(_context, ev);
                }
                catch (const Except::IOException&)
                {
                    /* end connection */
                    try
                    {
                        udpSocket.Close();
                    }
                    catch (const Except::Exception&)
                    {
                    }
                    _context.Remove(_token.id);

//                    Core::DisconnectedEvent ev{ _token.id };
//                    _dispatcher.Dispatch(_context, ev);
                }
            }
        }

        void UDPSocketEventHandlerImpl::OnRead(Net::UDPSocket& udpSocket)
        {
            Net::SocketAddress recvPeerAddress;

            bool needClose = false;

            try
            {
                auto received = udpSocket.ReceiveFrom(recvPeerAddress, _buffer, _len);

                /* normally not larger than MTU */
                if (received > 0)
                {
                    if (recvPeerAddress == _peerAddress)
                    {
                        auto stream = std::make_shared<MemoryResponseStream>(received);
                        if (stream && stream->_buffer)  /* allocate successfully */
                        {
                            ::memcpy(stream->_buffer, _buffer, received);
                            stream->_readableLen = received;
                            stream->_offset = 0;
//
//                            Core::ReceiveResponseEvent ev{ _token.id, stream };
//                            _dispatcher.Dispatch(_context, ev);
                        }
                        else
                        {
                            throw Except::OutOfMemoryException{};
                        }
                    }
                    else
                    {
                        /* not from target, just discard it */
                    }
                }
                else /* no bytes to read */
                {
                    needClose = true;
                }
            }
            catch (const Except::IOException&)
            {
                needClose = true;
            }

            if (needClose)
            {
                /* just end connection */
                try
                {
                    udpSocket.Close();
                }
                catch (const Except::Exception&)
                {
                }
                _context.Remove(_token.id);

//                Core::DisconnectedEvent ev{ _token.id };
//                _dispatcher.Dispatch(_context, ev);
            }
        }
    }
}