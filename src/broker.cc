#include "broker.h"

namespace NexusRPC {

MessageBroker::MessageBroker() : trace_buffer_(1024) {}

void MessageBroker::process_input(const uint8_t* data, size_t size, std::vector<uint8_t>& response_bytes) {
    Packet packet;
    if (!PacketParser::deserialize(data, size, packet)) {
        trace_buffer_.write_entry(0, 400, "Malformed packet received and dropped.");
        return;
    }

    trace_buffer_.write_entry(0, static_cast<uint32_t>(packet.header.type), 
                              "Processing packet for client: " + packet.client_id);

    switch (packet.header.type) {
        case PacketType::CONNECT:
            handle_connect(packet, response_bytes);
            break;
        case PacketType::PUBLISH:
            handle_publish(packet, response_bytes);
            break;
        case PacketType::SUBSCRIBE:
            handle_subscribe(packet, response_bytes);
            break;
        case PacketType::UNSUBSCRIBE:
            handle_unsubscribe(packet, response_bytes);
            break;
        case PacketType::DISCONNECT:
            handle_disconnect(packet, response_bytes);
            break;
        default:
            trace_buffer_.write_entry(0, 405, "Unsupported packet type.");
            break;
    }
}

void MessageBroker::handle_connect(const Packet& packet, std::vector<uint8_t>& response_bytes) {
    session_manager_.create_session(packet.client_id);
    
    bool auth_ok = session_manager_.authenticate_session(packet.client_id, packet.payload.get_string());

    Packet ack;
    ack.header.type = PacketType::CONNACK;
    ack.header.flags = auth_ok ? 1 : 0;
    ack.header.transaction_id = packet.header.transaction_id;
    ack.client_id = packet.client_id;
    ack.payload = Payload(auth_ok);

    std::vector<uint8_t> ack_bytes = PacketParser::serialize(ack);
    response_bytes.insert(response_bytes.end(), ack_bytes.begin(), ack_bytes.end());
}

void MessageBroker::handle_publish(const Packet& packet, std::vector<uint8_t>& response_bytes) {
    ClientSession* session = session_manager_.get_session(packet.client_id);
    if (!session || !session->is_authenticated) {
        trace_buffer_.write_entry(0, 401, "Unauthorized publish attempt.");
        return;
    }

    if (packet.payload.type == PayloadType::STRING) {
        // Query evaluation simulation: expects a string but might get a bool/int if mismatched
        std::string payload_str = packet.payload.get_string();
        trace_buffer_.write_entry(0, 200, "Publishing string payload: " + payload_str);
    }

    std::vector<std::string> subscribers = subscription_trie_.get_subscribers(packet.topic);
    for (const auto& sub_id : subscribers) {
        ClientSession* sub_session = session_manager_.get_session(sub_id);
        if (sub_session) {
            // Forward packet to subscriber
            Packet forward = packet;
            forward.header.transaction_id = packet.header.transaction_id;
            std::vector<uint8_t> forward_bytes = PacketParser::serialize(forward);
            response_bytes.insert(response_bytes.end(), forward_bytes.begin(), forward_bytes.end());
        }
    }
}

void MessageBroker::handle_subscribe(const Packet& packet, std::vector<uint8_t>& response_bytes) {
    ClientSession* session = session_manager_.get_session(packet.client_id);
    if (!session || !session->is_authenticated) {
        return;
    }

    subscription_trie_.subscribe(packet.topic, packet.client_id);
    session->subscriptions.push_back(packet.topic);

    Packet ack;
    ack.header.type = PacketType::SUBACK;
    ack.header.flags = 1;
    ack.header.transaction_id = packet.header.transaction_id;
    ack.client_id = packet.client_id;
    ack.payload = Payload(true);

    std::vector<uint8_t> ack_bytes = PacketParser::serialize(ack);
    response_bytes.insert(response_bytes.end(), ack_bytes.begin(), ack_bytes.end());
}

void MessageBroker::handle_unsubscribe(const Packet& packet, std::vector<uint8_t>& response_bytes) {
    ClientSession* session = session_manager_.get_session(packet.client_id);
    if (!session || !session->is_authenticated) {
        return;
    }

    subscription_trie_.unsubscribe(packet.topic, packet.client_id);

    auto it = std::find(session->subscriptions.begin(), session->subscriptions.end(), packet.topic);
    if (it != session->subscriptions.end()) {
        session->subscriptions.erase(it);
    }
}

void MessageBroker::handle_disconnect(const Packet& packet, std::vector<uint8_t>& response_bytes) {
    (void)response_bytes;
    session_manager_.terminate_session(packet.client_id);
}

} // namespace NexusRPC
