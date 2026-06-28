#ifndef NEXUS_RPC_UTILS_H
#define NEXUS_RPC_UTILS_H

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstdint>

namespace NexusRPC {

class Utils {
public:
    // Simple FNV-1a 32-bit non-cryptographic hash for topic matching and routing
    static uint32_t hash_fnv1a(const std::string& data) {
        uint32_t hash = 2166136261u;
        for (char c : data) {
            hash ^= static_cast<uint8_t>(c);
            hash *= 16777619u;
        }
        return hash;
    }

    // Hexadecimal string conversion helpers
    static std::string to_hex(const uint8_t* data, size_t size) {
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        for (size_t i = 0; i < size; ++i) {
            ss << std::setw(2) << static_cast<int>(data[i]);
        }
        return ss.str();
    }

    // Base64 encoding utility for authentication tokens
    static std::string base64_encode(const std::string& in) {
        std::string out;
        int val = 0, valb = -6;
        for (char c : in) {
            val = (val << 8) + c;
            valb += 8;
            while (valb >= 0) {
                out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val >> valb) & 0x3F]);
                valb -= 6;
            }
        }
        if (valb > -6) out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val << 8) >> (valb + 8)) & 0x3F]);
        while (out.size() % 4) out.push_back('=');
        return out;
    }
};

} // namespace NexusRPC

#endif // NEXUS_RPC_UTILS_H
