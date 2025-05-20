#pragma once

#include <vector>
#include <memory>
#include <stdexcept>
#include "Process.h"
#include "Resource.h"

class Dispatcher {
public:
    // Конструктор
    Dispatcher(const std::vector<std::vector<int>>& processesNeeds,
        const std::vector<int>& totalResources);

    // Основные методы управления ресурсами
    bool isSafeState() const;
    bool requestResources(int processId, int resourceType, int amount);
    bool isAllProcessesCompleted() const;
    void reset();

    // Методы для отображения состояния
    void printState() const;

    // Методы доступа к внутренним данным
    Resource& getResource();
    const std::vector<std::shared_ptr<Process>>& getProcesses() const;
    Process& getProcess(int id);

    // Удаление копирования и присваивания
    Dispatcher(const Dispatcher&) = delete;
    Dispatcher& operator=(const Dispatcher&) = delete;

private:
    std::vector<std::shared_ptr<Process>> processes;
    std::shared_ptr<Resource> resource;

    // Внутренний метод проверки безопасности
    bool checkSafety(const std::vector<int>& available,
        const std::vector<std::vector<int>>& allocated,
        const std::vector<std::vector<int>>& need) const;
};