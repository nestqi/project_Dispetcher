#pragma once

#include <vector>

class Process {
public:
    Process(int id, const std::vector<int>& maxNeed);

    bool canComplete(const std::vector<int>& available) const;
    void allocate(const std::vector<int>& resources);
    void releaseAll();

    int getId() const;
    const std::vector<int>& getAllocated() const;
    const std::vector<int>& getMaxNeed() const;
    const std::vector<int>& getRemainingNeed() const;

    bool isCompleted() const;

private:
    int id;
    std::vector<int> maxNeed;
    std::vector<int> allocated;
    std::vector<int> remainingNeed;
};
