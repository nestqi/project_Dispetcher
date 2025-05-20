#pragma once

#include <vector>
#include <iostream>

class Resource {
public:
    Resource(const std::vector<int>& totalResources);

    bool canAllocate(int resourceType, int amount) const;
    void allocate(int resourceType, int amount);
    void release(int resourceType, int amount);

    const std::vector<int>& getAvailable() const;
    const std::vector<int>& getTotal() const;

    void printStatus() const;

private:
    std::vector<int> totalResources;
    std::vector<int> availableResources;
};