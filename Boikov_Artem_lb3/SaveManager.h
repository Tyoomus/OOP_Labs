// SaveManager.h
#ifndef SAVE_MANAGER_H
#define SAVE_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

// Классы исключений для сохранения/загрузки
class SaveLoadException : public std::runtime_error {
public:
    explicit SaveLoadException(const std::string& msg) : std::runtime_error(msg) {}
};

class FileOpenException : public SaveLoadException {
public:
    explicit FileOpenException(const std::string& filename) 
        : SaveLoadException("Cannot open file: " + filename) {}
};

class InvalidSaveDataException : public SaveLoadException {
public:
    explicit InvalidSaveDataException(const std::string& msg) 
        : SaveLoadException("Invalid save data: " + msg) {}
};

// Структура для хранения данных сохранения
struct GameSaveData {
    // Данные игрока
    std::string playerName;
    int playerHealth;
    int playerMaxHealth;
    int playerMana;
    int playerMaxMana;
    int playerScore;
    int playerX;
    int playerY;
    int combatMode;
    
    // Данные врагов
    struct EnemyData {
        int x;
        int y;
        int health;
        int damage;
        bool alive;
    };
    std::vector<EnemyData> enemies;
    
    // Данные поля
    struct CellData {
        int x;
        int y;
        int type;
    };
    std::vector<CellData> impassableCells;
    
    // Данные заклинаний
    struct SpellData {
        std::string name;
        std::string description;
        int manaCost;
        int type; // 0 - DirectDamage, 1 - AreaDamage
        int damage;
        int range;
        int radius; // для AreaDamage
    };
    std::vector<SpellData> spells;
    int selectedSpellIndex;
    
    // Данные уровня
    int level;
    int fieldWidth;
    int fieldHeight;
};

class SaveManager {
public:
    SaveManager() = default;
    
    // Сохранение игры в файл
    void saveGame(const GameSaveData& data, const std::string& filename);
    
    // Загрузка игры из файла
    GameSaveData loadGame(const std::string& filename);
    
private:
    std::string serialize(const GameSaveData& data);
    GameSaveData deserialize(const std::string& json);
};

#endif