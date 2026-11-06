#include "categorical.hpp"
#include <algorithm>

Categorical::Categorical() : high_cardinality(false), count(0) {}

void Categorical::update(std::string_view val) {
    count++;
    if (high_cardinality) return;

    std::string str_val(val);
    frequencies[str_val]++;

    // Se superiamo le 1000 parole diverse, è inutile tenerle in RAM
    if (frequencies.size() > MAX_UNIQUE) {
        high_cardinality = true;
        frequencies.clear(); // Svuota la memoria istantaneamente
    }
}

unsigned long long Categorical::get_count() const { return count; }
bool Categorical::is_high_cardinality() const { return high_cardinality; }

std::vector<std::pair<std::string, unsigned long long>> Categorical::get_top_k(int k) const {
    if (high_cardinality || frequencies.empty()) return {};
    
    // Travasa la hash map in un vettore per poterlo ordinare
    std::vector<std::pair<std::string, unsigned long long>> vec(frequencies.begin(), frequencies.end());
    
    // Ordina dal più frequente al meno frequente
    std::sort(vec.begin(), vec.end(), [](const auto& a, const auto& b) {
        return a.second > b.second; 
    });
    
    if (vec.size() > static_cast<size_t>(k)) {
        vec.resize(k);
    }
    return vec;
}