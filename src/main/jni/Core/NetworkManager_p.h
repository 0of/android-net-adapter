#ifndef NETWORKMANAGER_P_H
#define NETWORKMANAGER_P_H

#include <map>
#include <vector>

#include "NetworkManager.h"

namespace Core
{
    class NetworkContext;
    class NetworkStatus;
    class IncomingMiddleware;
    class NetworkManageable;
    class ConnectionRequestMessage;

    /* implementations in NetworkManager.cpp */
    class NetworkManager::PrivateData
    {
    public:
        NetworkContext& _context;
        std::map<std::int32_t, NetworkStatus *> _status;

        std::vector<IncomingMiddleware *> _incomingMiddlewareChain;

    public:
        PrivateData(NetworkContext& context)
            : _context(context)
        {}

    public:
        void On(ConnectionRequestMessage *message, NetworkManageable *mgr);

    public:
      inline NetworkStatus *GetStatus(std::int32_t id) const
        {
            auto statusIter = _status.find(id);
            if (statusIter != _status.end())
            {
                return statusIter->second;
            }
            return nullptr;
        }

        void Close(std::int32_t id);
//        {
//            auto statusIter = _status.find(id);
//            if (statusIter != _status.end())
//            {
//                delete statusIter->second;
//                _status.erase(statusIter);
//            }
//
//            _context.Remove(id);
//        }
    };
}

#endif  /*  NETWORKMANAGER_P_H  */
