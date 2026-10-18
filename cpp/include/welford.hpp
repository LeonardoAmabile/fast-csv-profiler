#pragma once

class Welford {
private:
    unsigned long long count;
    unsigned long long null_count;
    double mean;
    double m2;      // Sum of squares of differences from the current mean
    double min_val;
    double max_val;

public:
    // Constructor
    Welford();

    // Core methods to update the state row by row
    void update(double value);
    void add_null();

    // Getters for the final JSON report
    double get_mean() const;
    double get_variance() const;
    double get_min() const;
    double get_max() const;
    unsigned long long get_null_count() const;
    unsigned long long get_count() const;
};