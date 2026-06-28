#include "packet_parser.h"
#include <cstring>

namespace NexusRPC {

static uint32_t read_u32(const uint8_t* ptr) {
    uint32_t val;
    std::memcpy(&val, ptr, 4);
    return val;
}

static void write_u32(uint8_t* ptr, uint32_t val) {
    std::memcpy(ptr, &val, 4);
}

std::vector<uint8_t> PacketParser::serialize(const Packet& packet) {
    std::vector<uint8_t> bytes;
    
    // Header size: 10 bytes (type(1), flags(1), length(4), transaction_id(4))
    bytes.resize(10);
    bytes[0] = static_cast<uint8_t>(packet.header.type);
    bytes[1] = packet.header.flags;
    write_u32(bytes.data() + 6, packet.header.transaction_id);

    // Topic serialization
    uint32_t topic_len = static_cast<uint32_t>(packet.topic.size());
    uint8_t len_bytes[4];
    write_u32(len_bytes, topic_len);
    bytes.insert(bytes.end(), len_bytes, len_bytes + 4);
    bytes.insert(bytes.end(), packet.topic.begin(), packet.topic.end());

    // Client ID serialization
    uint32_t client_len = static_cast<uint32_t>(packet.client_id.size());
    write_u32(len_bytes, client_len);
    bytes.insert(bytes.end(), len_bytes, len_bytes + 4);
    bytes.insert(bytes.end(), packet.client_id.begin(), packet.client_id.end());

    // Payload serialization (supports INT and STRING for simplicity)
    bytes.push_back(static_cast<uint8_t>(packet.payload.type));
    if (packet.payload.type == PayloadType::INT) {
        int32_t val = packet.payload.get_int();
        uint8_t val_bytes[4];
        std::memcpy(val_bytes, &val, 4);
        bytes.insert(bytes.end(), val_bytes, val_bytes + 4);
    } else if (packet.payload.type == PayloadType::STRING) {
        std::string val = packet.payload.get_string();
        uint32_t val_len = static_cast<uint32_t>(val.size());
        write_u32(len_bytes, val_len);
        bytes.insert(bytes.end(), len_bytes, len_bytes + 4);
        bytes.insert(bytes.end(), val.begin(), val.end());
    } else if (packet.payload.type == PayloadType::BOOL) {
        bytes.push_back(packet.payload.get_bool() ? 1 : 0);
    }

    // Update total length in header
    uint32_t total_len = static_cast<uint32_t>(bytes.size() - 10);
    write_u32(bytes.data() + 2, total_len);

    return bytes;
}

bool PacketParser::deserialize(const uint8_t* data, size_t size, Packet& out_packet) {
    if (size < 10) return false;

    out_packet.header.type = static_cast<PacketType>(data[0]);
    out_packet.header.flags = data[1];
    out_packet.header.length = read_u32(data + 2);
    out_packet.header.transaction_id = read_u32(data + 6);

    if (size < 10 + out_packet.header.length) return false;

    size_t offset = 10;

    // Topic
    if (offset + 4 > size) return false;
    uint32_t topic_len = read_u32(data + offset);
    offset += 4;
    if (offset + topic_len > size) return false;
    out_packet.topic.assign(reinterpret_cast<const char*>(data + offset), topic_len);
    offset += topic_len;

    // Client ID
    if (offset + 4 > size) return false;
    uint32_t client_len = read_u32(data + offset);
    offset += 4;
    if (offset + client_len > size) return false;
    out_packet.client_id.assign(reinterpret_cast<const char*>(data + offset), client_len);
    offset += client_len;

    // Payload
    if (offset >= size) return false;
    PayloadType p_type = static_cast<PayloadType>(data[offset++]);
    
    if (p_type == PayloadType::INT) {
        if (offset + 4 > size) return false;
        int32_t val;
        std::memcpy(&val, data + offset, 4);
        out_packet.payload = Payload(val);
        offset += 4;
    } else if (p_type == PayloadType::STRING) {
        if (offset + 4 > size) return false;
        uint32_t val_len = read_u32(data + offset);
        offset += 4;
        if (offset + val_len > size) return false;
        std::string val(reinterpret_cast<const char*>(data + offset), val_len);
        out_packet.payload = Payload(val);
        offset += val_len;
    } else if (p_type == PayloadType::BOOL) {
        if (offset >= size) return false;
        out_packet.payload = Payload(data[offset] != 0);
        offset += 1;
    } else {
        out_packet.payload = Payload(); // NIL
    }

    return true;
}

} // namespace NexusRPC
