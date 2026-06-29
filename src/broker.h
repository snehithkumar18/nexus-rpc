#ifndef NEXUS_RPC_BROKER_H
#define NEXUS_RPC_BROKER_H

#include "packet_parser.h"
#include "session_manager.h"
#include "subscription_trie.h"
#include "packet_assembler.h"
#include "ring_buffer.h"
#include <string>
#include <vector>
#include <mutex>
#include <unordered_map>

namespace NexusRPC {

class MessageBroker {
private:
    SessionManager session_manager_;
    SubscriptionTrie subscription_trie_;
    PacketAssembler packet_assembler_;
    TraceRingBuffer trace_buffer_;
    std::mutex mutex_;

    ClientSession* last_active_session_ = nullptr;
    std::unordered_map<std::string, std::vector<ClientSession*>> routing_cache_;

    void handle_connect(const Packet& packet, std::vector<uint8_t>& response_bytes);
    void handle_publish(const Packet& packet, std::vector<uint8_t>& response_bytes);
    void handle_subscribe(const Packet& packet, std::vector<uint8_t>& response_bytes);
    void handle_unsubscribe(const Packet& packet, std::vector<uint8_t>& response_bytes);
    void handle_disconnect(const Packet& packet, std::vector<uint8_t>& response_bytes);

public:
    MessageBroker();
    ~MessageBroker() = default;

    // Main entry point to process incoming network bytes.
    // Appends output responses to response_bytes.
    void process_input(const uint8_t* data, size_t size, std::vector<uint8_t>& response_bytes);
    
    TraceRingBuffer& get_trace_buffer() { return trace_buffer_; }
    SessionManager& get_session_manager() { return session_manager_; }
};

} // namespace NexusRPC

#endif // NEXUS_RPC_BROKER_H
