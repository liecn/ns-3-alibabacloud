#include "flow-routing.h"
#include <iostream>

// Global variable definitions
std::unordered_map<FlowKey, uint32_t, FlowKeyHash> global_flow_to_path_map;
bool global_use_custom_routing = false;

// Statistics counters
uint64_t custom_routing_hits = 0;
uint64_t custom_routing_misses = 0;
uint64_t total_routing_lookups = 0;

// Function to extract flow key from packet
FlowKey ExtractFlowKeyFromPacket(uint16_t cur_node,
                                 uint32_t src_ip, uint32_t dst_ip, uint8_t protocol,
                                 uint16_t src_port, uint16_t dst_port) {
    FlowKey key;
    key.cur_node = cur_node;
    key.src_ip = src_ip;
    key.dst_ip = dst_ip;
    key.protocol = 0x11; // force UDP
    key.src_port = 10006; // fixed port for routing decision
    key.dst_port = 100;   // fixed dport
    return key;
}

// Function to lookup flow path
int LookupFlowPath(const FlowKey& flow_key) {
    total_routing_lookups++;
    
    if (!global_use_custom_routing) {
        return -1;  // Custom routing disabled
    }
    
    // Check if we have a pre-calculated path
    auto it = global_flow_to_path_map.find(flow_key);
    if (it != global_flow_to_path_map.end()) {
        custom_routing_hits++;
        return it->second;  // Return the next-hop interface
    }
    
    custom_routing_misses++;
    return -1;  // No custom path found, fall back to ECMP
}

// Function to print routing statistics
void PrintRoutingStatistics() {
    if (total_routing_lookups == 0) {
        std::cout << "[ROUTING STATS] No routing lookups performed" << std::endl;
        return;
    }
    
    double hit_rate = (double)custom_routing_hits / total_routing_lookups * 100.0;
    
    std::cout << "[ROUTING STATS] Total lookups: " << total_routing_lookups << std::endl;
    std::cout << "[ROUTING STATS] Custom routing hits: " << custom_routing_hits << std::endl;
    std::cout << "[ROUTING STATS] Custom routing misses: " << custom_routing_misses << std::endl;
    std::cout << "[ROUTING STATS] Custom routing hit rate: " << hit_rate << "%" << std::endl;
}

// Function to reset statistics
void ResetRoutingStatistics() {
    custom_routing_hits = 0;
    custom_routing_misses = 0;
    total_routing_lookups = 0;
}

void SetGlobalCustomRouting(bool enable) {
    global_use_custom_routing = enable;
}

void ClearGlobalFlowMap() {
    global_flow_to_path_map.clear();
}

// Direct access functions for frontend-backend communication
extern "C" {
    void SetGlobalFlowMapDirect(const FlowKey* keys, const uint32_t* values, size_t size) {
        global_flow_to_path_map.clear();
        for (size_t i = 0; i < size; i++) {
            global_flow_to_path_map[keys[i]] = values[i];
        }
    }
    
    void SetGlobalCustomRoutingDirect(bool enable) {
        global_use_custom_routing = enable;
    }
    
    void ClearGlobalFlowMapDirect() {
        global_flow_to_path_map.clear();
    }
    
    void PrintRoutingStatsDirect() {
        PrintRoutingStatistics();
    }
    
    void ResetRoutingStatsDirect() {
        ResetRoutingStatistics();
    }
} 