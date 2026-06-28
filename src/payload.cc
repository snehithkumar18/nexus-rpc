#include "payload.h"
#include <algorithm>

namespace NexusRPC {

Payload::Payload() : type(PayloadType::NIL), val_ptr(nullptr) {}

Payload::Payload(int32_t val) : type(PayloadType::INT) {
    val_ptr = new int32_t(val);
}

Payload::Payload(const std::string& val) : type(PayloadType::STRING) {
    val_ptr = new std::string(val);
}

Payload::Payload(bool val) : type(PayloadType::BOOL) {
    val_ptr = new bool(val);
}

Payload::Payload(const std::vector<Payload>& val) : type(PayloadType::ARRAY) {
    val_ptr = new std::vector<Payload>(val);
}

Payload::Payload(const std::unordered_map<std::string, Payload>& val) : type(PayloadType::MAP) {
    val_ptr = new std::unordered_map<std::string, Payload>(val);
}

Payload::~Payload() {
    clear();
}

void Payload::clear() {
    if (val_ptr) {
        if (type == PayloadType::INT) {
            delete static_cast<int32_t*>(val_ptr);
        } else if (type == PayloadType::STRING) {
            delete static_cast<std::string*>(val_ptr);
        } else if (type == PayloadType::BOOL) {
            delete static_cast<bool*>(val_ptr);
        } else if (type == PayloadType::ARRAY) {
            delete static_cast<std::vector<Payload>*>(val_ptr);
        } else if (type == PayloadType::MAP) {
            delete static_cast<std::unordered_map<std::string, Payload>*>(val_ptr);
        }
        val_ptr = nullptr;
    }
    type = PayloadType::NIL;
}

Payload::Payload(const Payload& other) : type(PayloadType::NIL), val_ptr(nullptr) {
    *this = other;
}

Payload& Payload::operator=(const Payload& other) {
    if (this != &other) {
        clear();
        type = other.type;
        if (other.val_ptr) {
            if (type == PayloadType::INT) {
                val_ptr = new int32_t(*static_cast<int32_t*>(other.val_ptr));
            } else if (type == PayloadType::STRING) {
                val_ptr = new std::string(*static_cast<std::string*>(other.val_ptr));
            } else if (type == PayloadType::BOOL) {
                val_ptr = new bool(*static_cast<bool*>(other.val_ptr));
            } else if (type == PayloadType::ARRAY) {
                val_ptr = new std::vector<Payload>(*static_cast<std::vector<Payload>*>(other.val_ptr));
            } else if (type == PayloadType::MAP) {
                val_ptr = new std::unordered_map<std::string, Payload>(*static_cast<std::unordered_map<std::string, Payload>*>(other.val_ptr));
            }
        }
    }
    return *this;
}

Payload::Payload(Payload&& other) noexcept : type(PayloadType::NIL), val_ptr(nullptr) {
    *this = std::move(other);
}

Payload& Payload::operator=(Payload&& other) noexcept {
    if (this != &other) {
        clear();
        type = other.type;
        val_ptr = other.val_ptr;
        other.type = PayloadType::NIL;
        other.val_ptr = nullptr;
    }
    return *this;
}

// INJECTED BUG 2 (Type Confusion): These getters do not validate 'type'.
// They directly static_cast val_ptr, leading to heap-buffer-overflows if types mismatch.
int32_t Payload::get_int() const {
    return *static_cast<int32_t*>(val_ptr);
}

std::string Payload::get_string() const {
    return *static_cast<std::string*>(val_ptr);
}

bool Payload::get_bool() const {
    return *static_cast<bool*>(val_ptr);
}

std::vector<Payload> Payload::get_array() const {
    return *static_cast<std::vector<Payload>*>(val_ptr);
}

std::unordered_map<std::string, Payload> Payload::get_map() const {
    return *static_cast<std::unordered_map<std::string, Payload>*>(val_ptr);
}

bool Payload::operator==(const Payload& other) const {
    if (type != other.type) return false;
    if (!val_ptr && !other.val_ptr) return true;
    if (!val_ptr || !other.val_ptr) return false;
    
    switch (type) {
        case PayloadType::NIL: return true;
        case PayloadType::INT: return get_int() == other.get_int();
        case PayloadType::STRING: return get_string() == other.get_string();
        case PayloadType::BOOL: return get_bool() == other.get_bool();
        case PayloadType::ARRAY: return get_array() == other.get_array();
        case PayloadType::MAP: return get_map() == other.get_map();
    }
    return false;
}

bool Payload::operator!=(const Payload& other) const {
    return !(*this == other);
}

} // namespace NexusRPC
