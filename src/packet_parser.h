#ifndef NEXUS_RPC_PACKET_PARSER_H
#define NEXUS_RPC_PACKET_PARSER_H

#include "payload.h"
#include <vector>
#include <string>
#include <cstdint>

namespace NexusRPC {

enum class PacketType : uint8_t {
    CONNECT = 0,
    CONNACK = 1,
    PUBLISH = 2,
    SUBSCRIBE = 3,
    SUBACK = 4,
    UNSUBSCRIBE = 5,
    DISCONNECT = 6
};

struct PacketHeader {
    PacketType type;
    uint8_t flags;
    uint32_t length;
    uint32_t transaction_id;
};

struct Packet {
    PacketHeader header;
    std::string topic;
    std::string client_id;
    Payload payload;
};

class PacketParser {
public:
    static std::vector<uint8_t> serialize(const Packet& packet);
    
    // Parses raw bytes into a Packet.
    // Returns true on success, false on truncation/malformation.
    static bool deserialize(const uint8_t* data, size_t size, Packet& out_packet);
};

} // namespace NexusRPC

#endif // NEXUS_RPC_PACKET_PARSER_H
