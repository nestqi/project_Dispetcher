#pragma once
#include <vector>
#include <memory>
#include "Dispatcher.h"

class Game {
public:
    Game();
    void start();
    void nextLevel();
    void saveGame() const;
    void loadGame();

private:
    int currentLevel;
    int score;
    int attempts;
    std::unique_ptr<Dispatcher> dispatcher;

    void playLevel();
    void showMenu() const;
    void showStats() const;
    void showLevelInfo() const;
};