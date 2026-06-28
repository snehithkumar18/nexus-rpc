#include "subscription_trie.h"
#include <sstream>
#include <algorithm>

namespace NexusRPC {

TrieNode::~TrieNode() {
    for (auto& pair : children) {
        delete pair.second;
    }
}

SubscriptionTrie::SubscriptionTrie() {
    root_ = new TrieNode("");
}

SubscriptionTrie::~SubscriptionTrie() {
    delete root_;
}

static std::vector<std::string> split_topic(const std::string& topic) {
    std::vector<std::string> tokens;
    std::stringstream ss(topic);
    std::string token;
    while (std::getline(ss, token, '/')) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

void SubscriptionTrie::subscribe(const std::string& topic_path, const std::string& subscriber_id) {
    std::vector<std::string> tokens = split_topic(topic_path);
    TrieNode* current = root_;
    
    for (const auto& token : tokens) {
        auto it = current->children.find(token);
        if (it == current->children.end()) {
            TrieNode* new_node = new TrieNode(token);
            current->children[token] = new_node;
            current = new_node;
        } else {
            current = it->second;
        }
    }
    
    current->is_subscription_endpoint = true;
    if (std::find(current->subscriber_ids.begin(), current->subscriber_ids.end(), subscriber_id) == current->subscriber_ids.end()) {
        current->subscriber_ids.push_back(subscriber_id);
    }
}

// INJECTED BUG 3 (Double Free):
// When cleaning up nodes, if a node becomes empty, we delete it.
// However, the recursive caller doesn't properly handle the return status and can
// attempt to delete the same child node pointer again in the parent context.
bool SubscriptionTrie::remove_recursive(TrieNode* node, const std::vector<std::string>& tokens, 
                                       size_t index, const std::string& subscriber_id) {
    if (!node) return false;

    if (index == tokens.size()) {
        auto it = std::find(node->subscriber_ids.begin(), node->subscriber_ids.end(), subscriber_id);
        if (it != node->subscriber_ids.end()) {
            node->subscriber_ids.erase(it);
        }
        if (node->subscriber_ids.empty()) {
            node->is_subscription_endpoint = false;
        }
        return node->subscriber_ids.empty() && node->children.empty();
    }

    const std::string& token = tokens[index];
    auto it = node->children.find(token);
    if (it != node->children.end()) {
        bool should_delete_child = remove_recursive(it->second, tokens, index + 1, subscriber_id);
        if (should_delete_child) {
            TrieNode* child = it->second;
            
            // DELIBERATE BUG: Double-free risk.
            // We delete the child node here, but we do not erase the key from the parent's children map.
            // If another operation traverses this path or if we do a wildcard cleanup, 
            // the parent will see a non-null pointer to already deleted memory, and try to delete it again.
            delete child;
            
            return true; // Tells the parent to delete "node" as well
        }
    }
    return false;
}

void SubscriptionTrie::unsubscribe(const std::string& topic_path, const std::string& subscriber_id) {
    std::vector<std::string> tokens = split_topic(topic_path);
    remove_recursive(root_, tokens, 0, subscriber_id);
}

void SubscriptionTrie::match_recursive(TrieNode* node, const std::vector<std::string>& tokens, 
                                     size_t index, std::vector<std::string>& matches) const {
    if (!node) return;

    // Wildcard matching: '#' matches everything remaining
    auto hash_it = node->children.find("#");
    if (hash_it != node->children.end()) {
        for (const auto& sub : hash_it->second->subscriber_ids) {
            matches.push_back(sub);
        }
    }

    if (index == tokens.size()) {
        if (node->is_subscription_endpoint) {
            for (const auto& sub : node->subscriber_ids) {
                matches.push_back(sub);
            }
        }
        return;
    }

    const std::string& token = tokens[index];
    
    // Exact match
    auto exact_it = node->children.find(token);
    if (exact_it != node->children.end()) {
        match_recursive(exact_it->second, tokens, index + 1, matches);
    }

    // Single level wildcard '+'
    auto plus_it = node->children.find("+");
    if (plus_it != node->children.end()) {
        match_recursive(plus_it->second, tokens, index + 1, matches);
    }
}

std::vector<std::string> SubscriptionTrie::get_subscribers(const std::string& topic_path) const {
    std::vector<std::string> tokens = split_topic(topic_path);
    std::vector<std::string> matches;
    match_recursive(root_, tokens, 0, matches);
    return matches;
}

} // namespace NexusRPC
