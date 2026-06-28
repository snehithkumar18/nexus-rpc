#ifndef NEXUS_RPC_SUBSCRIPTION_TRIE_H
#define NEXUS_RPC_SUBSCRIPTION_TRIE_H

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace NexusRPC {

struct TrieNode {
    std::string token;
    bool is_subscription_endpoint = false;
    std::vector<std::string> subscriber_ids;
    std::unordered_map<std::string, TrieNode*> children;

    TrieNode(const std::string& tok) : token(tok) {}
    ~TrieNode();
};

class SubscriptionTrie {
private:
    TrieNode* root_;

    void match_recursive(TrieNode* node, const std::vector<std::string>& tokens, 
                         size_t index, std::vector<std::string>& matches) const;

    // Helper to recursively cleanup empty nodes.
    // that double-frees node pointers under specific wildcard unsubscription sequences.
    bool remove_recursive(TrieNode* node, const std::vector<std::string>& tokens, 
                          size_t index, const std::string& subscriber_id);

public:
    SubscriptionTrie();
    ~SubscriptionTrie();

    void subscribe(const std::string& topic_path, const std::string& subscriber_id);
    void unsubscribe(const std::string& topic_path, const std::string& subscriber_id);
    std::vector<std::string> get_subscribers(const std::string& topic_path) const;
};

} // namespace NexusRPC

#endif // NEXUS_RPC_SUBSCRIPTION_TRIE_H
