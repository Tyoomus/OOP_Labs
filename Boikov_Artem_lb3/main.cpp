#include <iostream>
#include "GameEngine.h"

int main() {
    try {
        GameEngine game;
        game.run();
        
        std::cout << "Thanks for playing!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}