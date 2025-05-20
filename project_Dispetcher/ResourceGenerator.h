#pragma once
#include <vector>
#include <random>
#include <algorithm>


class ResourceGenerator {
public:
    ResourceGenerator(int seed = std::random_device{}());

    // Объявления без реализации
    std::vector<int> generateTotalResources(int resourceTypes, int min, int max);
    std::vector<std::vector<int>> generateProcessesNeeds(int processesCount,
        int resourceTypes,
        const std::vector<int>& totalResources);
private:
    std::mt19937 gen;
};