#ifndef NEXUS_RPC_PACKET_ASSEMBLER_H
#define NEXUS_RPC_PACKET_ASSEMBLER_H

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <mutex>

namespace NexusRPC {

struct Fragment {
    uint16_t sequence_num;
    uint16_t total_fragments;
    uint32_t total_size;
    std::vector<uint8_t> payload;
};

class PacketAssembler {
private:
    // Maps transaction ID to list of received fragments
    std::unordered_map<uint32_t, std::vector<Fragment>> message_buffer_;
    std::mutex mutex_;

public:
    PacketAssembler() = default;
    ~PacketAssembler() = default;

    // Returns true if all fragments have arrived and assembly is complete.
    bool add_fragment(uint32_t transaction_id, uint16_t sequence_num, 
                      uint16_t total_fragments, uint32_t total_size, 
                      const uint8_t* payload_data, uint16_t payload_len);

    // INJECTED BUG 4 (Stack Buffer Overflow): Copies assembled fragments 
    // into a stack-allocated buffer without verifying the total_size fits.
    std::vector<uint8_t> assemble_packet(uint32_t transaction_id);
    
    void clear_transaction(uint32_t transaction_id);
};

} // namespace NexusRPC

#endif // NEXUS_RPC_PACKET_ASSEMBLER_H
