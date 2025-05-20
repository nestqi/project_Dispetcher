#pragma once

#include <vector>
#include <string>

struct LevelConfig {
    int processesCount;
    int resourcesTypes;
    int minResourcesPerType;
    int maxResourcesPerType;
    std::string description;
};

const std::vector<LevelConfig> LEVELS = {
    {3, 2, 5, 10, "Легкий уровень: 3 процесса, 2 типа ресурсов"},
    {5, 3, 3, 15, "Средний уровень: 5 процессов, 3 типа ресурсов"},
    {7, 4, 10, 30, "Сложный уровень: 7 процессов, 4 типа ресурсов"}
};