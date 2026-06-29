#include "../src/broker.h"
#include <cstdint>
#include <cstddef>
#include <vector>
#include <cstring>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size < 10) return 0;

    NexusRPC::MessageBroker broker;

    // Setup initial authenticated sessions
    broker.get_session_manager().create_session("client1");
    broker.get_session_manager().authenticate_session("client1", "secret_token_123");

    broker.get_session_manager().create_session("client2");
    broker.get_session_manager().authenticate_session("client2", "secret_token_123");

    // Process multiple packets from the fuzzer input
    size_t offset = 0;
    std::vector<uint8_t> response;
    while (offset + 10 <= size) {
        // Read the packet length from the header (bytes 2-5)
        uint32_t length;
        std::memcpy(&length, data + offset + 2, 4);
        
        size_t packet_size = 10 + length;
        if (offset + packet_size > size || packet_size < 10) {
            break;
        }
        
        broker.process_input(data + offset, packet_size, response);
        offset += packet_size;
        response.clear();
    }

    return 0;
}
