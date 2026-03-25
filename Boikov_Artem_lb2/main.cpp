#include <iostream>
#include "GameManager.h"

int main() {
    try {
        GameManager game;
        game.startGame();
        
        while (game.isGameRunning()) {
            game.processTurn();
        }
        
        std::cout << "Thanks for playing!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}