#include "../src/broker.h"
#include <cstdint>
#include <cstddef>
#include <vector>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size < 10) return 0;

    NexusRPC::MessageBroker broker;

    // Setup initial authenticated sessions
    broker.get_session_manager().create_session("client1");
    broker.get_session_manager().authenticate_session("client1", "secret_token_123");

    broker.get_session_manager().create_session("client2");
    broker.get_session_manager().authenticate_session("client2", "secret_token_123");

    // Process fuzzer input through the broker
    std::vector<uint8_t> response;
    broker.process_input(data, size, response);

    return 0;
}
