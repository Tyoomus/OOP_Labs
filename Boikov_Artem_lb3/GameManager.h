#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "Player.h"
#include "GameField.h"
#include "Enemy.h"
#include "Spell.h"
#include <vector>
#include <memory>

class DirectDamageSpell;
class AreaDamageSpell;
class SpellBook;
class GameEngine;  // Добавляем forward declaration

// Только forward declaration, без определения
struct GameSaveData;

class GameManager {
private:
    Player player;
    GameField field;
    std::vector<Enemy> enemies;
    bool gameRunning;
    
    // Для режима выбора цели заклинаний
    int spellTargetX;
    int spellTargetY;
    bool selectingSpellTarget;
    int selectedSpellIndex;
    
    GameEngine* gameEngine;  // Добавляем указатель на GameEngine

public:
    GameManager();
    GameManager(GameEngine* engine);  // Новый конструктор
    
    void startGame();
    void processTurn();
    void displayGameState() const;
    bool isGameRunning() const;
    
    // Методы для работы с заклинаниями (через dynamic_cast)
    void castDirectDamageSpell(const DirectDamageSpell& spell, int targetX, int targetY);
    void castAreaDamageSpell(const AreaDamageSpell& spell, int targetX, int targetY);
    
    GameSaveData getSaveData() const;
    bool loadFromData(const GameSaveData& data);
    
    // Для работы с уровнями
    void nextLevel();
    bool areAllEnemiesDefeated() const;
    const Player& getPlayer() const;
    
    void setGameEngine(GameEngine* engine);  // Сеттер для GameEngine
    void setGameRunning(bool running) { gameRunning = running; }
    
private:
    void initializeGame();
    void processPlayerTurn();
    void processEnemiesTurn();
    void checkGameOver();
    void displayField() const;
    void performAttack();
    
    // Методы для работы с заклинаниями
    void processSpellSelection();
    void processSpellTargeting();
    void displaySpellInfo() const;
    void addRandomSpellToPlayer();
};

#endif