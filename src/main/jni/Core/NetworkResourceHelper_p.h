#ifndef NETWORKRESOURCEHELPER_H
#define NETWORKRESOURCEHELPER_H

#include <cstdint>
#include <string>

namespace Core
{
    class NetworkContext;
    class NetworkManageable;

    namespace NetImpl
    {
        class NetworkCreationHelper
        {
        private:
            NetworkContext& _context;
            NetworkManageable *_mgr;

        public:
            NetworkCreationHelper(NetworkContext& context, NetworkManageable *mgr);
            ~NetworkCreationHelper() = default;

        public:
            void Create(std::int32_t id, const std::string& host, std::uint16_t af, std::uint16_t port, bool isTCP);
        };
    }
}

#endif  /*  NETWORKRESOURCEHELPER_H  */