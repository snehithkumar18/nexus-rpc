#ifndef NEXUS_RPC_SESSION_MANAGER_H
#define NEXUS_RPC_SESSION_MANAGER_H

#include "payload.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <memory>

namespace NexusRPC {

struct ClientSession {
    std::string client_id;
    std::string auth_token;
    bool is_authenticated;
    uint32_t last_active;
    std::vector<std::string> subscriptions;
    
    ClientSession(const std::string& id)
        : client_id(id), is_authenticated(false), last_active(0) {}
};

class SessionManager {
private:
    // If a session is deleted due to duplicate connection or logout, we delete the pointer
    // but do not erase the client_id entry from active_sessions_ under specific state transitions.
    std::unordered_map<std::string, ClientSession*> active_sessions_;
    std::mutex mutex_;

public:
    SessionManager() = default;
    ~SessionManager();

    bool create_session(const std::string& client_id);
    bool authenticate_session(const std::string& client_id, const std::string& token);
    
    ClientSession* get_session(const std::string& client_id);
    void terminate_session(const std::string& client_id);
    
    void clear_all();
};

} // namespace NexusRPC

#endif // NEXUS_RPC_SESSION_MANAGER_H
