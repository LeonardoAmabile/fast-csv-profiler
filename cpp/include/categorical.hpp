/**
 * @file categorical.hpp
 * @brief Handles statistical tracking for categorical (textual) data.
 * 
 * Includes a memory-safe fallback mechanism (high cardinality detection)
 * to prevent RAM exhaustion when processing columns with mostly unique strings 
 * (e.g., UUIDs, free text).
 */

#pragma once
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

class Categorical {
private:
    std::unordered_map<std::string, unsigned long long> frequencies;
    bool high_cardinality;
    unsigned long long count;
    
    // Safety limit to prevent RAM exhaustion (OOM errors)
    const size_t MAX_UNIQUE = 1000; 

public:
    Categorical();
    
    /**
     * Processes a single textual value, updating its frequency count.
     * Automatically clears memory and triggers the high cardinality flag 
     * if unique values exceed MAX_UNIQUE.
     */
    void update(std::string_view val);
    
    unsigned long long get_count() const;
    bool is_high_cardinality() const;
    
    /**
     * Retrieves the 'k' most frequent values sorted in descending order.
     * Returns an empty vector if the column was flagged as high cardinality.
     * 
     * @param k The maximum number of top categories to return (default: 5).
     */
    std::vector<std::pair<std::string, unsigned long long>> get_top_k(int k = 5) const;
};