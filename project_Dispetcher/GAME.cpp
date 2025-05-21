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
        int choice = Utils::getIntInput("�������� ��������: ", 1, 4);

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
        std::cout << "\n�����������! �� ������ ��� ������!\n";
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

        std::cout << "\n������: ������� " << processId
            << " ������� " << amount
            << " ������ ������� " << resourceType + 1 << "\n";

        int choice = Utils::getIntInput("������ �������? (1 - ��, 0 - ���): ", 0, 1);

        if (choice == 1) {
            if (dispatcher->requestResources(processId, resourceType, amount)) {
                std::cout << "������� ������. ��������� ���������.\n";
                score += 10;
            }
            else {
                std::cout << "������! ������ �������� � ������������� ���������.\n";
                dispatcher->reset();
                std::cout << "������� ��������. ������� #" << ++attempts << "\n";
                score = std::max(0, score - 5);
            }
        }
        else {
            std::cout << "������ ��������.\n";
        }

        Utils::pause();
    }

    if (dispatcher->isAllProcessesCompleted()) {
        Utils::clearScreen();
        std::cout << "\n�����������! ��� �������� ���������!\n";
        std::cout << "��� ����: " << score << "\n";
        std::cout << "���������� �������: " << attempts << "\n";
        Utils::pause();
    }
}

void Game::showMenu() const {
    std::cout << "\nменю:\n";
    std::cout << "1. ������ ����� �������\n";
    std::cout << "2. ��������� ����\n";
    std::cout << "3. �������� ����������\n";
    std::cout << "4. �����\n";
}

void Game::showStats() const {
    Utils::clearScreen();
    std::cout << "\n����������:\n";
    std::cout << "������� �������: " << currentLevel << " �� " << LEVELS.size() << "\n";
    std::cout << "����: " << score << "\n";
    std::cout << "����� �������: " << attempts << "\n";
    Utils::pause();
}

void Game::showLevelInfo() const {
    const auto& level = LEVELS[currentLevel - 1];
    std::cout << "\n" << level.description << "\n";
    std::cout << "���������: " << level.processesCount << "\n";
    std::cout << "����� ��������: " << level.resourcesTypes << "\n";
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
    std::cout << "���� ��������� � ���� savegame.json\n";
}

void Game::loadGame() {
    std::ifstream file("savegame.json");
    if (!file.is_open()) {
        std::cout << "���� ���������� �� ������!\n";
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

        std::cout << "���� ������� ���������!\n";
    }
    catch (const std::exception& e) {
        std::cout << "������ ��������: " << e.what() << "\n";
    }
}