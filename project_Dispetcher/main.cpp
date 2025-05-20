#include "Game.h"
#include <iostream>

int main() {
    try {
        Game game;
        game.start();
    }
    catch (const std::exception& e) {
        std::cerr << "Îøèáêà: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}