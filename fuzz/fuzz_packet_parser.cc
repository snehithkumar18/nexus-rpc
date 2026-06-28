#include "../src/packet_parser.h"
#include <cstdint>
#include <cstddef>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size < 10) return 0;

    NexusRPC::Packet packet;
    NexusRPC::PacketParser::deserialize(data, size, packet);

    return 0;
}
