#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <array>
#include <cstdint>

namespace Dispatch
{
    namespace Serialization
    {
        template<typename BufferType, typename SerializationService>
        class Serializer
        {
        private:
            BufferType *const _serializeTo;

        public:
            Serializer(BufferType *to) : _serializeTo(to) {}

        public:
            template<typename AnyType>
            Serializer<SerializationService>& operator << (AnyType&& any)
            {
                SerializationService::DoSerialize(_serializeTo, any /* no need forward */);
                return *this;
            }
        };

        template<typename BufferType, typename DeserializationService>
        class Deserializer
        {
        private:
            const BufferType * const _deserializeFrom;

        public:
            Deserializer(const BufferType *const from) : _deserializeFrom(from) {}

        public:
            template<typename AnyType>
            Deserializer<DeserializationService>& operator >> (AnyType&& any)
            {
                DeserializationService::DoDeserialize(_deserializeFrom, any /* no need forward */);
                return *this;
            }
        };
    }
}

#endif /* SERIALIZATION_H */
