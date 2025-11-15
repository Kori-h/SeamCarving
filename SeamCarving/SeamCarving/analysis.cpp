#include "../pch.h"
#include "../SeamCarving/analysis.hpp"
#include "../SeamCarving/seamcarvingdp.hpp"
#include "../SeamCarving/seamcarvinggreedy.hpp"

namespace Analysis
{
    PerformanceMetrics MeasureDPVerticalSeam(Grid<float> const& energy, std::vector<int>& outSeam)
    {
        PerformanceMetrics metrics;

        auto start = std::chrono::high_resolution_clock::now();

        outSeam = DP::FindVerticalSeam(energy);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;

        metrics.computationTimeMs = elapsed.count();
        metrics.seamEnergy = DP::CalculateVerticalSeamEnergy(energy, outSeam);

        // Memory estimation: cumulative grid (width * height * sizeof(float))
        metrics.memoryUsed = energy.width * energy.height * sizeof(float);

        return metrics;
    }

    PerformanceMetrics MeasureGreedyVerticalSeam(Grid<float> const& energy, std::vector<int>& outSeam)
    {
        PerformanceMetrics metrics;

        auto start = std::chrono::high_resolution_clock::now();

        outSeam = Greedy::FindVerticalSeamGreedy(energy);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;

        metrics.computationTimeMs = elapsed.count();
        metrics.seamEnergy = Greedy::CalculateVerticalSeamEnergy(energy, outSeam);

        // Memory estimation: only stores the seam path (height * sizeof(int))
        metrics.memoryUsed = energy.height * sizeof(int);

        return metrics;
    }

    PerformanceMetrics MeasureDPHorizontalSeam(Grid<float> const& energy, std::vector<int>& outSeam)
    {
        PerformanceMetrics metrics;

        auto start = std::chrono::high_resolution_clock::now();

        outSeam = DP::FindHorizontalSeam(energy);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;

        metrics.computationTimeMs = elapsed.count();
        metrics.seamEnergy = DP::CalculateHorizontalSeamEnergy(energy, outSeam);

        // Memory estimation: cumulative grid
        metrics.memoryUsed = energy.width * energy.height * sizeof(float);

        return metrics;
    }

    PerformanceMetrics MeasureGreedyHorizontalSeam(Grid<float> const& energy, std::vector<int>& outSeam)
    {
        PerformanceMetrics metrics;

        auto start = std::chrono::high_resolution_clock::now();

        outSeam = Greedy::FindHorizontalSeamGreedy(energy);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;

        metrics.computationTimeMs = elapsed.count();
        metrics.seamEnergy = Greedy::CalculateHorizontalSeamEnergy(energy, outSeam);

        // Memory estimation: only stores the seam path
        metrics.memoryUsed = energy.width * sizeof(int);

        return metrics;
    }

    void CompareSeams(std::vector<int> const& seam1, std::vector<int> const& seam2,
        std::string const& name1, std::string const& name2)
    {
        if (seam1.size() != seam2.size())
        {
            std::cout << "Seams have different sizes, cannot compare!" << std::endl;
            return;
        }

        int differences = 0;
        int maxDiff = 0;

        for (size_t i = 0; i < seam1.size(); ++i)
        {
            int diff = std::abs(seam1[i] - seam2[i]);
            if (diff > 0)
            {
                ++differences;
                maxDiff = std::max(maxDiff, diff);
            }
        }

        std::cout << "\n=== Seam Comparison: " << name1 << " vs " << name2 << " ===" << std::endl;
        std::cout << "Total positions: " << seam1.size() << std::endl;
        std::cout << "Different positions: " << differences << " ("
            << (100.0f * differences / seam1.size()) << "%)" << std::endl;
        std::cout << "Maximum pixel difference: " << maxDiff << std::endl;

        if (differences == 0)
        {
            std::cout << "The seams are IDENTICAL!" << std::endl;
        }
        else
        {
            std::cout << "The seams are DIFFERENT." << std::endl;
        }
    }

    unsigned long long CountPossibleSeams(int rows, int cols)
    {
        // For question 2a(i): Count possible seams
        // This demonstrates exponential growth but will overflow quickly

        if (rows <= 0 || cols <= 0) return 0;
        if (rows == 1) return cols; // First row: can start at any column

        // For larger values, this will overflow - use EstimatePossibleSeamsLog instead
        if (rows > 20)
        {
            std::cout << "Warning: rows > 20 will cause overflow. Use EstimatePossibleSeamsLog instead." << std::endl;
            return 0;
        }

        // DP approach to count seams
        // dp[i][j] = number of seams ending at position (i, j)
        std::vector<std::vector<unsigned long long>> dp(rows, std::vector<unsigned long long>(cols, 0));

        // Base case: first row
        for (int j = 0; j < cols; ++j)
        {
            dp[0][j] = 1;
        }

        // Fill the table
        for (int i = 1; i < rows; ++i)
        {
            for (int j = 0; j < cols; ++j)
            {
                // Can come from (i-1, j-1), (i-1, j), or (i-1, j+1)
                if (j > 0)
                    dp[i][j] += dp[i - 1][j - 1];

                dp[i][j] += dp[i - 1][j];

                if (j < cols - 1)
                    dp[i][j] += dp[i - 1][j + 1];
            }
        }

        // Sum all seams ending in the last row
        unsigned long long total = 0;
        for (int j = 0; j < cols; ++j)
        {
            total += dp[rows - 1][j];
        }

        return total;
    }

    double EstimatePossibleSeamsLog(int rows, int cols)
    {
        // Returns log base 10 of the number of possible seams
        // This demonstrates exponential growth: number of seams ≈ 10^result

        if (rows <= 0 || cols <= 0) return 0;
        if (rows == 1) return std::log10(cols);

        // Use logarithms to avoid overflow
        std::vector<std::vector<double>> logDp(rows, std::vector<double>(cols, -std::numeric_limits<double>::infinity()));

        // Base case
        for (int j = 0; j < cols; ++j)
        {
            logDp[0][j] = 0; // log10(1) = 0
        }

        // Fill using log-sum-exp trick
        for (int i = 1; i < rows; ++i)
        {
            for (int j = 0; j < cols; ++j)
            {
                std::vector<double> terms;

                if (j > 0 && logDp[i - 1][j - 1] > -std::numeric_limits<double>::infinity())
                    terms.push_back(logDp[i - 1][j - 1]);

                if (logDp[i - 1][j] > -std::numeric_limits<double>::infinity())
                    terms.push_back(logDp[i - 1][j]);

                if (j < cols - 1 && logDp[i - 1][j + 1] > -std::numeric_limits<double>::infinity())
                    terms.push_back(logDp[i - 1][j + 1]);

                if (!terms.empty())
                {
                    // log(a + b + c) = log(a) + log(1 + b/a + c/a)
                    // For simplicity, approximate for large numbers
                    double maxTerm = *std::max_element(terms.begin(), terms.end());
                    double sum = 0;
                    for (double t : terms)
                    {
                        sum += std::pow(10, t - maxTerm);
                    }
                    logDp[i][j] = maxTerm + std::log10(sum);
                }
            }
        }

        // Sum in log space
        std::vector<double> lastRowLogs;
        for (int j = 0; j < cols; ++j)
        {
            if (logDp[rows - 1][j] > -std::numeric_limits<double>::infinity())
                lastRowLogs.push_back(logDp[rows - 1][j]);
        }

        if (lastRowLogs.empty()) return 0;

        double maxLog = *std::max_element(lastRowLogs.begin(), lastRowLogs.end());
        double sum = 0;
        for (double l : lastRowLogs)
        {
            sum += std::pow(10, l - maxLog);
        }

        return maxLog + std::log10(sum);
    }

    void PrintComparisonTable(PerformanceMetrics const& dp, PerformanceMetrics const& greedy,
        std::string const& seamType)
    {
        std::cout << "\n=== Performance Comparison: " << seamType << " ===" << std::endl;
        std::cout << std::fixed << std::setprecision(4);

        std::cout << "\n+------------------+------------------+------------------+" << std::endl;
        std::cout << "| Metric           | Dynamic Prog.    | Greedy Algorithm |" << std::endl;
        std::cout << "+------------------+------------------+------------------+" << std::endl;

        std::cout << "| Time (ms)        | "
            << std::setw(16) << dp.computationTimeMs << " | "
            << std::setw(16) << greedy.computationTimeMs << " |" << std::endl;

        std::cout << "| Seam Energy      | "
            << std::setw(16) << dp.seamEnergy << " | "
            << std::setw(16) << greedy.seamEnergy << " |" << std::endl;

        std::cout << "| Memory (bytes)   | "
            << std::setw(16) << dp.memoryUsed << " | "
            << std::setw(16) << greedy.memoryUsed << " |" << std::endl;

        std::cout << "+------------------+------------------+------------------+" << std::endl;

        float energyDiff = ((greedy.seamEnergy - dp.seamEnergy) / dp.seamEnergy) * 100.0f;
        double speedup = dp.computationTimeMs / greedy.computationTimeMs;
        float memoryReduction = ((float)(dp.memoryUsed - greedy.memoryUsed) / dp.memoryUsed) * 100.0f;

        std::cout << "\nAnalysis:" << std::endl;
        std::cout << "- DP finds optimal seam (minimum energy)" << std::endl;
        std::cout << "- Greedy seam has " << std::abs(energyDiff) << "% "
            << (energyDiff > 0 ? "MORE" : "LESS") << " energy than DP" << std::endl;
        std::cout << "- Greedy is " << speedup << "x "
            << (speedup > 1 ? "faster" : "slower") << " than DP" << std::endl;
        std::cout << "- Greedy uses " << memoryReduction << "% less memory" << std::endl;
    }
}