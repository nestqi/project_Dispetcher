#include "Game.h"
#include "Data.h"
#include "ResourceGenerator.h"
#include "Utils.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Game::Game() : currentLevel(0), score(0), attempts(0) {
    std::srand(std::time(nullptr));
}

void Game::start() {
    while (true) {
        Utils::clearScreen();
        showMenu();
        int choice = Utils::getIntInput("Выберите действие: ", 1, 4);

        switch (choice) {
        case 1:
            nextLevel();
            playLevel();
            break;
        case 2:
            loadGame();
            if (dispatcher) playLevel();
            break;
        case 3:
            showStats();
            break;
        case 4:
            return;
        }
    }
}

void Game::nextLevel() {
    if (currentLevel < static_cast<int>(LEVELS.size())) {
        currentLevel++;
        const auto& level = LEVELS[currentLevel - 1];

        ResourceGenerator rg(std::time(nullptr));
        auto totalResources = rg.generateTotalResources(level.resourcesTypes,
            level.minResourcesPerType,
            level.maxResourcesPerType);
        auto processesNeeds = rg.generateProcessesNeeds(level.processesCount,
            level.resourcesTypes,
            totalResources);

        dispatcher = std::make_unique<Dispatcher>(processesNeeds, totalResources);
        attempts = 0;
        score = 0;
    }
    else {
        std::cout << "\nПоздравляем! Вы прошли все уровни!\n";
        currentLevel = 0;
    }
}

void Game::playLevel() {
    const auto& level = LEVELS[currentLevel - 1];

    while (!dispatcher->isAllProcessesCompleted()) {
        Utils::clearScreen();
        showLevelInfo();
        dispatcher->printState();

        std::vector<int> unfinished;
        for (int i = 1; i <= level.processesCount; ++i) {
            if (!dispatcher->getProcess(i).isCompleted()) {
                unfinished.push_back(i);
            }
        }

        if (unfinished.empty()) break;

        int processId = unfinished[std::rand() % unfinished.size()];
        auto& process = dispatcher->getProcess(processId);

        std::vector<int> neededResources;
        const auto& remaining = process.getRemainingNeed();
        for (size_t i = 0; i < remaining.size(); ++i) {
            if (remaining[i] > 0) {
                neededResources.push_back(i);
            }
        }

        if (neededResources.empty()) break;

        int resourceType = neededResources[std::rand() % neededResources.size()];
        int maxAmount = process.getRemainingNeed()[resourceType];
        int amount = 1 + (maxAmount > 1 ? std::rand() % maxAmount : 0);

        std::cout << "\nЗапрос: Процесс " << processId
            << " требует " << amount
            << " единиц Ресурса " << resourceType + 1 << "\n";

        int choice = Utils::getIntInput("Выдать ресурсы? (1 - Да, 0 - Нет): ", 0, 1);

        if (choice == 1) {
            if (dispatcher->requestResources(processId, resourceType, amount)) {
                std::cout << "Ресурсы выданы. Состояние безопасно.\n";
                score += 10;
            }
            else {
                std::cout << "Ошибка! Выдача приведет к небезопасному состоянию.\n";
                dispatcher->reset();
                std::cout << "Система сброшена. Попытка #" << ++attempts << "\n";
                score = std::max(0, score - 5);
            }
        }
        else {
            std::cout << "Запрос отклонен.\n";
        }

        Utils::pause();
    }

    if (dispatcher->isAllProcessesCompleted()) {
        Utils::clearScreen();
        std::cout << "\nПоздравляем! Все процессы завершены!\n";
        std::cout << "Ваш счет: " << score << "\n";
        std::cout << "Количество попыток: " << attempts << "\n";
        Utils::pause();
    }
}

void Game::showMenu() const {
    std::cout << "\nМеню:\n";
    std::cout << "1. Начать новый уровень\n";
    std::cout << "2. Загрузить игру\n";
    std::cout << "3. Показать статистику\n";
    std::cout << "4. Выход\n";
}

void Game::showStats() const {
    Utils::clearScreen();
    std::cout << "\nСтатистика:\n";
    std::cout << "Текущий уровень: " << currentLevel << " из " << LEVELS.size() << "\n";
    std::cout << "Счет: " << score << "\n";
    std::cout << "Всего попыток: " << attempts << "\n";
    Utils::pause();
}

void Game::showLevelInfo() const {
    const auto& level = LEVELS[currentLevel - 1];
    std::cout << "\n" << level.description << "\n";
    std::cout << "Процессов: " << level.processesCount << "\n";
    std::cout << "Типов ресурсов: " << level.resourcesTypes << "\n";
}

void Game::saveGame() const {
    json gameState;
    gameState["currentLevel"] = currentLevel;
    gameState["score"] = score;
    gameState["attempts"] = attempts;

    if (dispatcher) {
        gameState["totalResources"] = dispatcher->getResource().getTotal();
        gameState["availableResources"] = dispatcher->getResource().getAvailable();

        json processesJson;
        for (const auto& process : dispatcher->getProcesses()) {
            json processJson;
            processJson["id"] = process->getId();
            processJson["maxNeed"] = process->getMaxNeed();
            processJson["allocated"] = process->getAllocated();
            processesJson.push_back(processJson);
        }
        gameState["processes"] = processesJson;
    }

    std::ofstream file("savegame.json");
    file << gameState.dump(4);
    std::cout << "Игра сохранена в файл savegame.json\n";
}

void Game::loadGame() {
    std::ifstream file("savegame.json");
    if (!file.is_open()) {
        std::cout << "Файл сохранения не найден!\n";
        return;
    }

    try {
        json gameState;
        file >> gameState;

        currentLevel = gameState["currentLevel"];
        score = gameState["score"];
        attempts = gameState["attempts"];

        if (gameState.contains("totalResources")) {
            std::vector<int> totalResources = gameState["totalResources"];
            std::vector<int> availableResources = gameState["availableResources"];

            std::vector<std::vector<int>> processesNeeds;
            std::vector<std::vector<int>> allocatedResources;

            for (const auto& processJson : gameState["processes"]) {
                processesNeeds.push_back(processJson["maxNeed"]);
                allocatedResources.push_back(processJson["allocated"]);
            }

            dispatcher = std::make_unique<Dispatcher>(processesNeeds, totalResources);

            for (const auto& processJson : gameState["processes"]) {
                int id = processJson["id"];
                std::vector<int> allocated = processJson["allocated"];
                dispatcher->getProcess(id).allocate(allocated);
            }

            auto& resource = dispatcher->getResource();
            for (size_t i = 0; i < availableResources.size(); ++i) {
                resource.allocate(i, resource.getTotal()[i] - availableResources[i]);
            }
        }

        std::cout << "Игра успешно загружена!\n";
    }
    catch (const std::exception& e) {
        std::cout << "Ошибка загрузки: " << e.what() << "\n";
    }
}