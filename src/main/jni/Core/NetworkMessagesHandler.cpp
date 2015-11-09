#include "NetworkMessagesHandler.h"

#include <algorithm>
#include <cassert>

#include "NetworkContext.h"
#include "NetworkEvents.h"
#include "NetworkMessagesHandler.h"
#include "Exception/Exception.h"
#include "Dispatch/EventDispatcher.h"

namespace Core
{
    MemoryResponseStream::MemoryResponseStream(std::uint32_t totalLen)
        : _buffer{ nullptr }
        , _readableLen{ 0 }
        , _offset{ 0 }
    {
        /* too large */
        assert(totalLen < 1024 * 1024);
        _buffer = new std::uint8_t[totalLen];
    }

    MemoryResponseStream::MemoryResponseStream()
    {
        if (_buffer)
            delete[]_buffer;
    }

    std::uint32_t MemoryResponseStream::Read(std::uint8_t *buffer, std::uint32_t len)
    {
        std::uint32_t readCount = std::min(_readableLen - _offset, len);
        if (readCount)
        {
            ::memcpy(buffer, _buffer + _offset, readCount);
            _offset += readCount;
        }

        return readCount;
    }

    SocketEventHandlerImpl::SocketEventHandlerImpl(std::int32_t connID, std::uint16_t domain, std::uint16_t port, const std::string& addr)
        : _token{ connID, domain, port, addr }
        , _queue{}
        , _buffer{ nullptr }
        , _len{4096}
    {
        _buffer = new std::uint8_t[_len];
    }

    SocketEventHandlerImpl::~SocketEventHandlerImpl()
    {
        for (auto each : _queue)
            delete each.second;

        if (_buffer)
            delete[]_buffer;
    }

    bool SocketEventHandlerImpl::Accept(Net::Mode mode) const
    {
        return mode == Net::Write && _queue.empty();
    }

    void SocketEventHandlerImpl::PushTask(const Task& t)
    {
        assert(t.first == Send && t.second != nullptr);
        _queue.push_back(t);
    }
}

