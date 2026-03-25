#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "GameManager.h"
#include "SaveManager.h"
#include <string>
#include <memory>
#include <functional>

enum class GameState {
    MENU,
    RUNNING,
    GAME_OVER,
    VICTORY,
    LOADING
};

class GameEngine {
private:
    std::unique_ptr<GameManager> currentGame;
    SaveManager saveManager;
    GameState state;
    int currentLevel;
    bool shouldExit;
    bool saveRequested;  // Флаг для запроса сохранения
    bool loadRequested;  // Флаг для запроса загрузки
    std::string loadFilename; // Имя файла для загрузки
    
public:
    GameEngine();
    
    void run();
    void showMainMenu();
    void startNewGame();
    void loadGame(const std::string& filename = "savegame.json");
    void saveGame(const std::string& filename = "savegame.json");
    void exitGame();
    void requestSave();  // Запрос на сохранение из GameManager
    void requestLoad(const std::string& filename = "savegame.json"); // Запрос на загрузку
    
    bool isRunning() const;
    GameState getState() const;
    
private:
    void processGameLoop();
    void handleGameOver();
    void handleVictory();
    void cleanup();
    void checkAndProcessRequests(); // Проверка и обработка запросов
};

#endif