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
    const size_t MAX_UNIQUE = 1000; // Limite salvavita per la RAM

public:
    Categorical();
    void update(std::string_view val);
    unsigned long long get_count() const;
    bool is_high_cardinality() const;
    
    // Ritorna i 'k' valori più frequenti per il report
    std::vector<std::pair<std::string, unsigned long long>> get_top_k(int k = 5) const;
};