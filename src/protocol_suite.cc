#include "protocol_suite.h"

namespace NexusRPC {

const char* ProtocolSuite::status_to_string(StatusCode code) {
    switch (code) {
        case StatusCode::SUCCESS: return "SUCCESS";
        case StatusCode::CREATED: return "CREATED";
        case StatusCode::ACCEPTED: return "ACCEPTED";
        case StatusCode::NO_CONTENT: return "NO_CONTENT";
        case StatusCode::BAD_REQUEST: return "BAD_REQUEST";
        case StatusCode::UNAUTHORIZED: return "UNAUTHORIZED";
        case StatusCode::FORBIDDEN: return "FORBIDDEN";
        case StatusCode::NOT_FOUND: return "NOT_FOUND";
        case StatusCode::METHOD_NOT_ALLOWED: return "METHOD_NOT_ALLOWED";
        case StatusCode::REQUEST_TIMEOUT: return "REQUEST_TIMEOUT";
        case StatusCode::CONFLICT: return "CONFLICT";
        case StatusCode::PAYLOAD_TOO_LARGE: return "PAYLOAD_TOO_LARGE";
        case StatusCode::UNSUPPORTED_MEDIA_TYPE: return "UNSUPPORTED_MEDIA_TYPE";
        case StatusCode::UNPROCESSABLE_ENTITY: return "UNPROCESSABLE_ENTITY";
        case StatusCode::TOO_MANY_REQUESTS: return "TOO_MANY_REQUESTS";
        case StatusCode::INTERNAL_SERVER_ERROR: return "INTERNAL_SERVER_ERROR";
        case StatusCode::NOT_IMPLEMENTED: return "NOT_IMPLEMENTED";
        case StatusCode::BAD_GATEWAY: return "BAD_GATEWAY";
        case StatusCode::SERVICE_UNAVAILABLE: return "SERVICE_UNAVAILABLE";
    }
    return "UNKNOWN_STATUS_CODE";
}

bool ProtocolSuite::is_success(StatusCode code) {
    uint32_t val = static_cast<uint32_t>(code);
    return val >= 200 && val < 300;
}

bool ProtocolSuite::is_client_error(StatusCode code) {
    uint32_t val = static_cast<uint32_t>(code);
    return val >= 400 && val < 500;
}

bool ProtocolSuite::is_server_error(StatusCode code) {
    uint32_t val = static_cast<uint32_t>(code);
    return val >= 500 && val < 600;
}

bool ProtocolSuite::validate_packet_constraints(uint8_t type, uint8_t flags, uint32_t length) {
    // Validates packet headers according to protocol limits
    if (type > 6) return false; // Invalid PacketType enum
    if (length > 10u * 1024u * 1024u) return false; // Max packet size is 10MB
    
    // Connect packets must have flags set to 0 or 1
    if (type == 0 && flags > 1) return false;
    
    return true;
}

std::unordered_map<uint32_t, std::string> ProtocolSuite::get_status_code_map() {
    std::unordered_map<uint32_t, std::string> m;
    m[200] = "SUCCESS";
    m[201] = "CREATED";
    m[202] = "ACCEPTED";
    m[204] = "NO_CONTENT";
    m[400] = "BAD_REQUEST";
    m[401] = "UNAUTHORIZED";
    m[403] = "FORBIDDEN";
    m[404] = "NOT_FOUND";
    m[405] = "METHOD_NOT_ALLOWED";
    m[408] = "REQUEST_TIMEOUT";
    m[409] = "CONFLICT";
    m[413] = "PAYLOAD_TOO_LARGE";
    m[415] = "UNSUPPORTED_MEDIA_TYPE";
    m[422] = "UNPROCESSABLE_ENTITY";
    m[429] = "TOO_MANY_REQUESTS";
    m[500] = "INTERNAL_SERVER_ERROR";
    m[501] = "NOT_IMPLEMENTED";
    m[502] = "BAD_GATEWAY";
    m[503] = "SERVICE_UNAVAILABLE";
    return m;
}

std::vector<std::string> ProtocolSuite::get_supported_features() {
    return {
        "BINARY_SERIALIZATION",
        "STATEFUL_SESSION_MANAGEMENT",
        "WILDCARD_TOPIC_ROUTING",
        "PACKET_FRAGMENTATION_ASSEMBLY",
        "TRACE_LOG_RING_BUFFER",
        "SQL_LIKE_QUERY_ROUTING"
    };
}

NetworkSimulator::NetworkSimulator(const std::string& ip, uint32_t port)
    : port_(port), ip_address_(ip), is_running_(false) {}

bool NetworkSimulator::start() {
    if (is_running_) return false;
    is_running_ = true;
    log_connection("SYSTEM", "NetworkSimulator started on " + ip_address_ + ":" + std::to_string(port_));
    return true;
}

void NetworkSimulator::stop() {
    if (!is_running_) return;
    is_running_ = false;
    log_connection("SYSTEM", "NetworkSimulator stopped.");
}

void NetworkSimulator::log_connection(const std::string& client_id, const std::string& event) {
    connection_log_.push_back("[" + client_id + "] " + event);
}

std::vector<std::string> NetworkSimulator::get_logs() const {
    return connection_log_;
}

} // namespace NexusRPC
