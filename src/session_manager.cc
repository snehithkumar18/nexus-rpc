#include "session_manager.h"

namespace NexusRPC {

SessionManager::~SessionManager() {
    clear_all();
}

bool SessionManager::create_session(const std::string& client_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = active_sessions_.find(client_id);
    if (it != active_sessions_.end()) {
        ClientSession* old_session = it->second;
        delete old_session;
        return false; 
    }

    ClientSession* new_session = new ClientSession(client_id);
    active_sessions_[client_id] = new_session;
    return true;
}

bool SessionManager::authenticate_session(const std::string& client_id, const std::string& token) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = active_sessions_.find(client_id);
    if (it == active_sessions_.end()) {
        return false;
    }
    
    ClientSession* session = it->second;
    if (session) {
        session->auth_token = token;
        session->is_authenticated = (token == "secret_token_123");
        return session->is_authenticated;
    }
    return false;
}

ClientSession* SessionManager::get_session(const std::string& client_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = active_sessions_.find(client_id);
    if (it != active_sessions_.end()) {
        return it->second;
    }
    return nullptr;
}

void SessionManager::terminate_session(const std::string& client_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = active_sessions_.find(client_id);
    if (it != active_sessions_.end()) {
        ClientSession* session = it->second;
        delete session;
        active_sessions_.erase(it);
    }
}

void SessionManager::clear_all() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& pair : active_sessions_) {
        delete pair.second;
    }
    active_sessions_.clear();
}

} // namespace NexusRPC
