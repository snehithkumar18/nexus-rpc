#ifndef NEXUS_RPC_PAYLOAD_H
#define NEXUS_RPC_PAYLOAD_H

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

namespace NexusRPC {

enum class PayloadType : uint8_t {
    NIL = 0,
    INT = 1,
    STRING = 2,
    BOOL = 3,
    ARRAY = 4,
    MAP = 5
};

class Payload {
public:
    PayloadType type;
    void* val_ptr;

    Payload();
    explicit Payload(int32_t val);
    explicit Payload(const std::string& val);
    explicit Payload(bool val);
    explicit Payload(const std::vector<Payload>& val);
    explicit Payload(const std::unordered_map<std::string, Payload>& val);
    
    ~Payload();
    Payload(const Payload& other);
    Payload& operator=(const Payload& other);
    Payload(Payload&& other) noexcept;
    Payload& operator=(Payload&& other) noexcept;

    bool check_type(PayloadType expected) const;
    int32_t get_int() const;
    std::string get_string() const;
    bool get_bool() const;
    std::vector<Payload> get_array() const;
    std::unordered_map<std::string, Payload> get_map() const;

    void clear();
    bool operator==(const Payload& other) const;
    bool operator!=(const Payload& other) const;
};

} // namespace NexusRPC

#endif // NEXUS_RPC_PAYLOAD_H
