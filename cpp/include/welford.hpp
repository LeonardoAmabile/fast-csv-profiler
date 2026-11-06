/**
 * @file welford.hpp
 * @brief Implementation of Welford's online algorithm.
 * 
 * Allows computing numerical statistics (mean, variance, standard deviation) 
 * in a single stream pass (O(N) time, O(1) space) without loading the dataset 
 * into RAM, completely avoiding numeric overflow and catastrophic cancellation 
 * common in naive sum-of-squares approaches.
 */

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
    Welford();

    /**
     * @brief Updates the running mathematical statistics with a new value.
     * 
     * @param value The newly parsed numeric value from the data stream.
     */
    void update(double value);
    
    /**
     * @brief Increments the null counter for missing or empty numeric cells.
     */
    void add_null();

    // ==========================================
    // Data Retrieval Methods (JSON Serialization)
    // ==========================================
    
    double get_mean() const;
    double get_variance() const;
    double get_std_dev() const;
    double get_min() const;
    double get_max() const;
    unsigned long long get_null_count() const;
    unsigned long long get_count() const;
};