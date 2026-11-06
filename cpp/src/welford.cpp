#include "welford.hpp"
#include <cmath>
#include <limits>

// Constructor: initialize everything to zero or extreme limits
Welford::Welford() : count(0), null_count(0), mean(0.0), m2(0.0), 
                     min_val(std::numeric_limits<double>::max()), 
                     max_val(std::numeric_limits<double>::lowest()) {}

void Welford::update(double value) {
    count++;
    
    // Update min and max
    if (value < min_val) min_val = value;
    if (value > max_val) max_val = value;

    // Welford's algorithm for streaming mean and variance
    double delta = value - mean;
    mean += delta / count;
    double delta2 = value - mean;
    m2 += delta * delta2;
}

void Welford::add_null() {
    null_count++;
}

double Welford::get_variance() const {
    if (count < 2) return 0.0;
    return m2 / (count - 1); // Sample variance
}

double Welford::get_std_dev() const {
    return std::sqrt(get_variance());
}

double Welford::get_mean() const {
    return mean;
}    

double Welford::get_min() const {
    return min_val;
}    

double Welford::get_max() const {
    return max_val;
}    

unsigned long long Welford::get_null_count() const { 
    return null_count; 
}

unsigned long long Welford::get_count() const {
     return count; 
}