
#pragma once
#include <chrono>

namespace Analysis
{
    // Structure to hold performance metrics
    struct PerformanceMetrics
    {
        double computationTimeMs;
        float seamEnergy;
        int memoryUsed; // in bytes (approximate)
    };

    // Measure the time and memory for DP vertical seam
    PerformanceMetrics MeasureDPVerticalSeam(Grid<float> const& energy, std::vector<int>& outSeam);

    // Measure the time and memory for Greedy vertical seam
    PerformanceMetrics MeasureGreedyVerticalSeam(Grid<float> const& energy, std::vector<int>& outSeam);

    // Measure the time and memory for DP horizontal seam
    PerformanceMetrics MeasureDPHorizontalSeam(Grid<float> const& energy, std::vector<int>& outSeam);

    // Measure the time and memory for Greedy horizontal seam
    PerformanceMetrics MeasureGreedyHorizontalSeam(Grid<float> const& energy, std::vector<int>& outSeam);

    // Compare two seams visually by highlighting differences
    void CompareSeams(std::vector<int> const& seam1, std::vector<int> const& seam2,
        std::string const& name1, std::string const& name2);

    // Calculate theoretical number of possible seams
    // For question 2a(i) - demonstrate exponential growth
    unsigned long long CountPossibleSeams(int rows, int cols);

    // Calculate using recurrence (for smaller values to avoid overflow)
    double EstimatePossibleSeamsLog(int rows, int cols);

    // Print comparison table
    void PrintComparisonTable(PerformanceMetrics const& dp, PerformanceMetrics const& greedy,
        std::string const& seamType);
}