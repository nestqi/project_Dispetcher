#include "Process.h"
#include <algorithm>

Process::Process(int id, const std::vector<int>& maxNeed)
    : id(id), maxNeed(maxNeed), allocated(maxNeed.size(), 0), remainingNeed(maxNeed) {}

bool Process::canComplete(const std::vector<int>& available) const {
    for (size_t i = 0; i < remainingNeed.size(); ++i) {
        if (remainingNeed[i] > available[i]) {
            return false;
        }
    }
    return true;
}

void Process::allocate(const std::vector<int>& resources) {
    for (size_t i = 0; i < resources.size(); ++i) {
        allocated[i] += resources[i];
        remainingNeed[i] -= resources[i];
        if (remainingNeed[i] < 0) remainingNeed[i] = 0;
    }
}

void Process::releaseAll() {
    std::fill(allocated.begin(), allocated.end(), 0);
    remainingNeed = maxNeed;
}

int Process::getId() const {
    return id;
}

const std::vector<int>& Process::getAllocated() const {
    return allocated;
}

const std::vector<int>& Process::getMaxNeed() const {
    return maxNeed;
}

const std::vector<int>& Process::getRemainingNeed() const {
    return remainingNeed;
}

bool Process::isCompleted() const {
    return std::all_of(remainingNeed.begin(), remainingNeed.end(), [](int need) { return need == 0; });
}