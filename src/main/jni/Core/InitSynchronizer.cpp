#include "InitSynchronizer.h"

namespace Core
{
    InitSynchronizer::InitSynchronizer(std::uint32_t count)
        :_waitCount{ count }
    {

    }

    InitSynchronizer::~InitSynchronizer()
    {

    }

    void InitSynchronizer::Wait()
    {
        std::unique_lock<decltype(_lock)> ul(_lock);

        _cond.wait(ul, [&](){return 0 == _waitCount; });
    }

    void InitSynchronizer::Notify()
    {
        std::unique_lock<decltype(_lock)> ul(_lock);
        --_waitCount;

        _cond.notify_one();
    }
}

