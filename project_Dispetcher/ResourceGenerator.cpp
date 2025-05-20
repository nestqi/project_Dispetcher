#include "ResourceGenerator.h"
#include <algorithm>

ResourceGenerator::ResourceGenerator(int seed) : gen(seed) {}

std::vector<int> ResourceGenerator::generateTotalResources(int resourceTypes, int min, int max) {
    std::uniform_int_distribution<> dist(min, max);
    std::vector<int> resources(resourceTypes);
    for (int& res : resources) {
        res = dist(gen);
    }
    return resources;
}

std::vector<std::vector<int>> ResourceGenerator::generateProcessesNeeds(int processesCount,
    int resourceTypes,
    const std::vector<int>& totalResources) {
    std::vector<std::vector<int>> needs;
    std::uniform_real_distribution<> dist(0.1, 0.7);

    for (int i = 0; i < processesCount; ++i) {
        std::vector<int> processNeeds;
        for (int j = 0; j < resourceTypes; ++j) {
            double ratio = dist(gen);
            int max_possible = static_cast<int>(totalResources[j] * ratio);
            std::uniform_int_distribution<> need_dist(1, std::max(1, max_possible));
            processNeeds.push_back(need_dist(gen));
        }
        needs.push_back(processNeeds);
    }

    return needs;
}