#include "Dispatcher.h"
#include <algorithm>
#include <stdexcept>
#include <iomanip>

Dispatcher::Dispatcher(const std::vector<std::vector<int>>& processesNeeds,
    const std::vector<int>& totalResources) {
    resource = std::make_shared<Resource>(totalResources);

    for (size_t i = 0; i < processesNeeds.size(); ++i) {
        processes.push_back(std::make_shared<Process>(i + 1, processesNeeds[i]));
    }
}

bool Dispatcher::isSafeState() const {
    std::vector<int> available = resource->getAvailable();
    std::vector<std::vector<int>> allocated;
    std::vector<std::vector<int>> need;

    for (const auto& process : processes) {
        allocated.push_back(process->getAllocated());
        need.push_back(process->getRemainingNeed());
    }

    return checkSafety(available, allocated, need);
}

bool Dispatcher::checkSafety(const std::vector<int>& available,
    const std::vector<std::vector<int>>& allocated,
    const std::vector<std::vector<int>>& need) const {
    std::vector<int> work = available;
    std::vector<bool> finish(processes.size(), false);
    bool found;

    do {
        found = false;
        for (size_t i = 0; i < processes.size(); ++i) {
            if (!finish[i]) {
                bool canFinish = true;
                for (size_t j = 0; j < work.size(); ++j) {
                    if (need[i][j] > work[j]) {
                        canFinish = false;
                        break;
                    }
                }

                if (canFinish) {
                    for (size_t j = 0; j < work.size(); ++j) {
                        work[j] += allocated[i][j];
                    }
                    finish[i] = true;
                    found = true;
                }
            }
        }
    } while (found);

    return std::all_of(finish.begin(), finish.end(), [](bool f) { return f; });
}

bool Dispatcher::requestResources(int processId, int resourceType, int amount) {
    // Проверка корректности ввода
    if (processId < 1 || processId > static_cast<int>(processes.size())) {
        throw std::out_of_range("Неверный ID процесса");
    }

    if (resourceType < 0 || resourceType >= static_cast<int>(resource->getTotal().size())) {
        throw std::out_of_range("Неверный тип ресурса");
    }

    if (amount <= 0) {
        throw std::invalid_argument("Количество ресурсов должно быть положительным");
    }

    auto& process = *processes[processId - 1];

    // Проверка на завершенность процесса и доступность ресурсов
    if (process.isCompleted()) {
        return false;
    }

    if (!resource->canAllocate(resourceType, amount)) {
        return false;
    }

    // Проверка, что процесс не запрашивает больше, чем ему нужно
    if (process.getRemainingNeed()[resourceType] < amount) {
        return false;
    }

    // Временное выделение ресурсов для проверки безопасности
    resource->allocate(resourceType, amount);
    std::vector<int> tempAlloc(resource->getTotal().size(), 0);
    tempAlloc[resourceType] = amount;
    process.allocate(tempAlloc);

    bool isSafe = isSafeState();

    // Откат изменений, если состояние небезопасно
    if (!isSafe) {
        resource->release(resourceType, amount);
        tempAlloc[resourceType] = -amount;
        process.allocate(tempAlloc);
    }

    return isSafe;
}

bool Dispatcher::isAllProcessesCompleted() const {
    return std::all_of(processes.begin(), processes.end(),
        [](const auto& p) { return p->isCompleted(); });
}

void Dispatcher::reset() {
    resource = std::make_shared<Resource>(resource->getTotal());
    for (auto& process : processes) {
        process->releaseAll();
    }
}

void Dispatcher::printState() const {
    // Вывод информации о ресурсах
    std::cout << "\n=== Состояние системы ===" << std::endl;
    resource->printStatus();

    // Вывод информации о процессах
    std::cout << "\nПроцессы:\n";
    std::cout << std::setw(10) << "ID"
        << std::setw(20) << "Выделено"
        << std::setw(20) << "Осталось"
        << std::setw(15) << "Статус" << std::endl;

    for (const auto& process : processes) {
        std::cout << std::setw(10) << process->getId();

        // Выделенные ресурсы
        std::cout << std::setw(10);
        for (int a : process->getAllocated()) {
            std::cout << a << " ";
        }

        // Оставшиеся потребности
        std::cout << std::setw(15);
        for (int r : process->getRemainingNeed()) {
            std::cout << r << " ";
        }

        // Статус
        std::cout << std::setw(15) << (process->isCompleted() ? "Завершен" : "Активен");
        std::cout << std::endl;
    }
}

Resource& Dispatcher::getResource() {
    return *resource;
}

const std::vector<std::shared_ptr<Process>>& Dispatcher::getProcesses() const {
    return processes;
}

Process& Dispatcher::getProcess(int id) {
    if (id < 1 || id > static_cast<int>(processes.size())) {
        throw std::out_of_range("Неверный ID процесса");
    }
    return *processes[id - 1];
}