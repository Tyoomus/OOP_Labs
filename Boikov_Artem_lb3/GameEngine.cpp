#include "GameEngine.h"
#include <iostream>
#include <limits>
#include <thread>
#include <chrono>

GameEngine::GameEngine() 
    : currentGame(nullptr), state(GameState::MENU), currentLevel(1), 
      shouldExit(false), saveRequested(false), loadRequested(false) {}

void GameEngine::run() {
    while (!shouldExit) {
        switch (state) {
            case GameState::MENU:
                showMainMenu();
                break;
            case GameState::RUNNING:
                processGameLoop();
                break;
            case GameState::GAME_OVER:
                handleGameOver();
                break;
            case GameState::VICTORY:
                handleVictory();
                break;
            case GameState::LOADING:
                break;
        }
    }
}

void GameEngine::showMainMenu() {
    std::cout << "\n================================" << std::endl;
    std::cout << "         GAME MENU" << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << "1. New Game" << std::endl;
    std::cout << "2. Load Game" << std::endl;
    std::cout << "3. Exit" << std::endl;
    std::cout << "Choose option: ";
    
    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    switch (choice) {
        case 1:
            startNewGame();
            break;
        case 2:
            loadGame();
            break;
        case 3:
            exitGame();
            break;
        default:
            std::cout << "Invalid option. Please try again." << std::endl;
            break;
    }
}

void GameEngine::startNewGame() {
    std::cout << "\nStarting new game..." << std::endl;
    currentGame = std::make_unique<GameManager>(this);
    currentGame->startGame();
    state = GameState::RUNNING;
    currentLevel = 1;
}

void GameEngine::loadGame(const std::string& filename) {
    try {
        auto loadedData = saveManager.loadGame(filename);
        
        currentGame = std::make_unique<GameManager>(this);
        currentGame->setGameEngine(this);
        
        if (currentGame->loadFromData(loadedData)) {
            std::cout << "Game loaded successfully from " << filename << std::endl;
            currentLevel = loadedData.level;
            if (currentLevel < 1) currentLevel = 1;
            state = GameState::RUNNING;
            currentGame->setGameRunning(true);
        } else {
            std::cout << "Failed to load game. Starting new game instead." << std::endl;
            startNewGame();
        }
    } catch (const SaveLoadException& e) {
        std::cerr << "Load error: " << e.what() << std::endl;
        std::cout << "Starting new game instead." << std::endl;
        startNewGame();
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        startNewGame();
    }
}

void GameEngine::saveGame(const std::string& filename) {
    if (!currentGame) {
        std::cout << "No active game to save!" << std::endl;
        return;
    }
    
    try {
        GameSaveData saveData = currentGame->getSaveData();
        saveData.level = currentLevel;
        saveManager.saveGame(saveData, filename);
        std::cout << "\n*** GAME SAVED SUCCESSFULLY to " << filename << " ***" << std::endl;
    } catch (const SaveLoadException& e) {
        std::cerr << "Save error: " << e.what() << std::endl;
        std::cout << "Failed to save game!" << std::endl;
    }
}

void GameEngine::exitGame() {
    if (currentGame && state == GameState::RUNNING) {
        std::cout << "Save before exit? (y/n): ";
        char choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (choice == 'y' || choice == 'Y') {
            saveGame("quit_save.json");
        }
    }
    std::cout << "Exiting game. Goodbye!" << std::endl;
    shouldExit = true;
}

void GameEngine::requestSave() {
    saveRequested = true;
}

void GameEngine::requestLoad(const std::string& filename) {
    loadRequested = true;
    loadFilename = filename;
}

bool GameEngine::isRunning() const {
    return !shouldExit;
}

GameState GameEngine::getState() const {
    return state;
}

void GameEngine::processGameLoop() {
    if (!currentGame) {
        state = GameState::MENU;
        return;
    }
    
    if (state == GameState::RUNNING && currentGame->isGameRunning()) {
        static bool levelDisplayed = false;
        if (!levelDisplayed) {
            std::cout << "\n--- Level " << currentLevel << " ---" << std::endl;
            std::cout << "Commands: M - Main Menu, K - Save Game, L - Load Game, Q - Quit" << std::endl;
            levelDisplayed = true;
        }
        
        checkAndProcessRequests();
        currentGame->processTurn();
        
        if (!currentGame->isGameRunning()) {
            if (!currentGame->getPlayer().isAlive()) {
                state = GameState::GAME_OVER;
                levelDisplayed = false;
            } else if (currentGame->areAllEnemiesDefeated()) {
                currentLevel++;
                if (currentLevel <= 3) {
                    std::cout << "\n=== LEVEL COMPLETE! ===\n" << std::endl;
                    std::cout << "Proceeding to level " << currentLevel << "..." << std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                    currentGame->nextLevel();
                    levelDisplayed = false;
                } else {
                    state = GameState::VICTORY;
                    levelDisplayed = false;
                }
            }
        }
    } else if (state == GameState::RUNNING && !currentGame->isGameRunning()) {
        state = GameState::MENU;
    }
}

void GameEngine::checkAndProcessRequests() {
    if (saveRequested) {
        saveGame();
        saveRequested = false;
    }
    
    if (loadRequested) {
        loadGame(loadFilename);
        loadRequested = false;
    }
}

void GameEngine::handleVictory() {
    std::cout << "\n================================" << std::endl;
    std::cout << "          VICTORY!" << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << "Congratulations! You completed the game!" << std::endl;
    if (currentGame) {
        std::cout << "Final score: " << currentGame->getPlayer().getScore() << std::endl;
    }
    
    std::cout << "\n1. Main Menu" << std::endl;
    std::cout << "2. Exit" << std::endl;
    std::cout << "Choose option: ";
    
    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    if (choice == 1) {
        cleanup();
        state = GameState::MENU;
    } else {
        exitGame();
    }
}

void GameEngine::handleGameOver() {
    std::cout << "\n================================" << std::endl;
    std::cout << "          GAME OVER" << std::endl;
    std::cout << "================================" << std::endl;
    if (currentGame) {
        std::cout << "Final score: " << currentGame->getPlayer().getScore() << std::endl;
    }
    
    std::cout << "\n1. Main Menu" << std::endl;
    std::cout << "2. Exit" << std::endl;
    std::cout << "Choose option: ";
    
    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    if (choice == 1) {
        cleanup();
        state = GameState::MENU;
    } else {
        exitGame();
    }
}

void GameEngine::cleanup() {
    currentGame.reset();
    currentLevel = 1;
    saveRequested = false;
    loadRequested = false;
}