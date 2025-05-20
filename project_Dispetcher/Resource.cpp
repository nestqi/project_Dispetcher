#include "Resource.h"

Resource::Resource(const std::vector<int>& totalResources)
    : totalResources(totalResources), availableResources(totalResources) {}

bool Resource::canAllocate(int resourceType, int amount) const {
    return resourceType >= 0 &&
        resourceType < static_cast<int>(availableResources.size()) &&
        availableResources[resourceType] >= amount;
}

void Resource::allocate(int resourceType, int amount) {
    if (canAllocate(resourceType, amount)) {
        availableResources[resourceType] -= amount;
    }
}

void Resource::release(int resourceType, int amount) {
    if (resourceType >= 0 && resourceType < static_cast<int>(availableResources.size())) {
        availableResources[resourceType] += amount;
        // Не может быть больше общего количества
        availableResources[resourceType] = std::min(availableResources[resourceType], totalResources[resourceType]);
    }
}

const std::vector<int>& Resource::getAvailable() const {
    return availableResources;
}

const std::vector<int>& Resource::getTotal() const {
    return totalResources;
}

void Resource::printStatus() const {
    std::cout << "Доступные ресурсы: ";
    for (size_t i = 0; i < availableResources.size(); ++i) {
        std::cout << "Ресурс " << i + 1 << ": " << availableResources[i] << "/" << totalResources[i] << " ";
    }
    std::cout << std::endl;
}