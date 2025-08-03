#ifndef FLOW_ROUTING_H
#define FLOW_ROUTING_H

#include <unordered_map>
#include <cstdint>
#include <vector>
#include <ostream>

// Flow key structure for packet-based routing
struct FlowKey {
    uint16_t cur_node;   // ID of the node performing the lookup (current hop)
    uint32_t src_ip;
    uint32_t dst_ip;
    uint8_t protocol;
    uint16_t src_port;
    uint16_t dst_port;
    
    bool operator==(const FlowKey& other) const {
        return cur_node == other.cur_node &&
               src_ip == other.src_ip && dst_ip == other.dst_ip && 
               protocol == other.protocol && src_port == other.src_port && 
               dst_port == other.dst_port;
    }
};

// Hash function for FlowKey
struct FlowKeyHash {
    std::size_t operator()(const FlowKey& key) const {
        std::size_t h0 = std::hash<uint16_t>{}(key.cur_node);
        std::size_t h1 = std::hash<uint32_t>{}(key.src_ip);
        std::size_t h2 = std::hash<uint32_t>{}(key.dst_ip);
        std::size_t h3 = std::hash<uint8_t>{}(key.protocol);
        std::size_t h4 = std::hash<uint16_t>{}(key.src_port);
        std::size_t h5 = std::hash<uint16_t>{}(key.dst_port);
        return h0 ^ (h1 << 1) ^ (h2 << 2) ^ (h3 << 3) ^ (h4 << 4) ^ (h5 << 5);
    }
};

inline std::ostream &operator<<(std::ostream &os, const FlowKey &k) {
    os << "{cur=" << k.cur_node << " sip=" << k.src_ip << " dip=" << k.dst_ip
       << " sport=" << k.src_port << " dport=" << k.dst_port << "}";
    return os;
}

// Global variables for flow routing
extern std::unordered_map<FlowKey, uint32_t, FlowKeyHash> global_flow_to_path_map;
extern bool global_use_custom_routing;

// Function to extract flow key from packet
FlowKey ExtractFlowKeyFromPacket(uint16_t cur_node,
                                 uint32_t src_ip, uint32_t dst_ip, uint8_t protocol, 
                                 uint16_t src_port, uint16_t dst_port);

// Function to lookup flow path
int LookupFlowPath(const FlowKey& flow_key);

// Function to print routing statistics
void PrintRoutingStatistics();

// Function to reset statistics
void ResetRoutingStatistics();

// Interface functions for frontend to backend communication
void SetGlobalCustomRouting(bool enable);
void ClearGlobalFlowMap();

// Direct access to global variables (for frontend use)
extern "C" {
    void SetGlobalFlowMapDirect(const FlowKey* keys, const uint32_t* values, size_t size);
    void SetGlobalCustomRoutingDirect(bool enable);
    void ClearGlobalFlowMapDirect();
    void PrintRoutingStatsDirect();
    void ResetRoutingStatsDirect();
}

// Frontend function declarations (implemented in common.h)
void EnableCustomRouting(bool enable);
void PrecalculateFlowPaths();
void ApplyCustomFlowOverrides();
void SyncFlowPathsWithBackend();
void AddFlowPath(uint32_t src_ip, uint32_t dst_ip, uint8_t protocol, 
                 uint16_t src_port, uint16_t dst_port, uint32_t next_hop_interface);
void ClearFlowPaths();
void OverrideFlowPath(uint32_t src_ip, uint32_t dst_ip, uint8_t protocol, 
                      uint16_t src_port, uint16_t dst_port, uint32_t forced_interface);

#endif // FLOW_ROUTING_H 