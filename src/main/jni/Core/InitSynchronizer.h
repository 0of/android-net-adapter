#ifndef INITSYNCHRONIZER_H
#define INITSYNCHRONIZER_H

#include <mutex>
#include <atomic>
#include <condition_variable>

#include "globals.h"

namespace Core
{
    class InitSynchronizer
    {
    private:
        std::mutex _lock;
        std::uint32_t _waitCount;
        std::condition_variable _cond;

    public:
        explicit InitSynchronizer(std::uint32_t count);
        ~InitSynchronizer();

    public:
        void Wait();
        void Notify();
    };
}

#endif  /* INITSYNCHRONIZER_H */