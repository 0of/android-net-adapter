#ifndef CAPABILITYCATEGORY_H
#define CAPABILITYCATEGORY_H

#include <cstdint>
#include <string>
#include <map>
#include <type_traits>

namespace Dispatch
{
    namespace Defs
    {
        template<typename T>
        struct CapabilityMetaClass;

        template<std::uint16_t id>
        struct CapabilityId;
    }

    /* link error occurred when duplicated id detected */
#ifndef CAPABILITY_DECL
#define CAPABILITY_DECL(class, id, description) \
    namespace Dispatch \
    { \
    namespace Defs \
    { \
    template<> \
    struct CapabilityMetaClass<class>\
    { \
    static const std::uint16_t ID = id; \
    }; \
    \
    template<> \
    struct CapabilityId<id>{}; \
    } \
    }
#endif

#ifndef IS_CAPABILITY
#define IS_CAPABILITY(id, class) \
    (Dispatch::Defs::CapabilityMetaClass<class>::ID == id)
#endif  /* IS_CAPABILITY */

#ifndef CAPABILITY
#define CAPABILITY(instance) \
    std::make_pair(Dispatch::Defs::CapabilityMetaClass<std::remove_pointer<std::decay<decltype(instance)>::type>::type >>::ID, instance)
#endif
}

#endif  /* CAPABILITYCATEGORY_H */