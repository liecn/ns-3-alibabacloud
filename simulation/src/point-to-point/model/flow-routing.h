#ifndef FLOW_ROUTING_H
#define FLOW_ROUTING_H

#include <unordered_map>
#include <cstdint>
#include <vector>

// Flow key structure for packet-based routing
struct FlowKey {
    uint32_t src_ip;
    uint32_t dst_ip;
    uint8_t protocol;
    uint16_t src_port;
    uint16_t dst_port;
    
    bool operator==(const FlowKey& other) const {
        return src_ip == other.src_ip && dst_ip == other.dst_ip && 
               protocol == other.protocol && src_port == other.src_port && 
               dst_port == other.dst_port;
    }
};

// Hash function for FlowKey
struct FlowKeyHash {
    std::size_t operator()(const FlowKey& key) const {
        std::size_t h1 = std::hash<uint32_t>{}(key.src_ip);
        std::size_t h2 = std::hash<uint32_t>{}(key.dst_ip);
        std::size_t h3 = std::hash<uint8_t>{}(key.protocol);
        std::size_t h4 = std::hash<uint16_t>{}(key.src_port);
        std::size_t h5 = std::hash<uint16_t>{}(key.dst_port);
        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4);
    }
};

// Global variables for flow routing
extern std::unordered_map<FlowKey, uint32_t, FlowKeyHash> global_flow_to_path_map;
extern bool global_use_custom_routing;

// Function to extract flow key from packet
FlowKey ExtractFlowKeyFromPacket(uint32_t src_ip, uint32_t dst_ip, uint8_t protocol, 
                                 uint16_t src_port, uint16_t dst_port);

// Function to lookup flow path
int LookupFlowPath(const FlowKey& flow_key);

// Function to print routing statistics
void PrintRoutingStatistics();

// Function to reset statistics
void ResetRoutingStatistics();

// Interface functions for frontend to backend communication
void SetGlobalFlowMap(const std::unordered_map<FlowKey, uint32_t, FlowKeyHash>& flow_map);
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