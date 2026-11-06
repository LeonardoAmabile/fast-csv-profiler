#include "categorical.hpp"
#include <algorithm>

Categorical::Categorical() : high_cardinality(false), count(0) {}

void Categorical::update(std::string_view val) {
    count++;
    
    // If already flagged as high cardinality, skip tracking to save CPU/RAM
    if (high_cardinality) return;

    std::string str_val(val);
    frequencies[str_val]++;

    // If unique values exceed the safety threshold, tracking them is no longer viable for RAM
    if (frequencies.size() > MAX_UNIQUE) {
        high_cardinality = true;
        frequencies.clear(); // Instantly free up memory to prevent Out-Of-Memory (OOM) errors
    }
}

unsigned long long Categorical::get_count() const { return count; }
bool Categorical::is_high_cardinality() const { return high_cardinality; }

std::vector<std::pair<std::string, unsigned long long>> Categorical::get_top_k(int k) const {
    // Return empty if there is no data or if memory was cleared due to high cardinality
    if (high_cardinality || frequencies.empty()) return {};
    
    // Transfer the hash map contents into a vector since maps cannot be sorted directly by value
    std::vector<std::pair<std::string, unsigned long long>> vec(frequencies.begin(), frequencies.end());
    
    // Sort descending by frequency (highest count first)
    std::sort(vec.begin(), vec.end(), [](const auto& a, const auto& b) {
        return a.second > b.second; 
    });
    
    // Truncate the vector to keep only the top 'k' elements
    if (vec.size() > static_cast<size_t>(k)) {
        vec.resize(k);
    }
    
    return vec;
}