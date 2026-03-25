#include "SaveManager.h"
#include <fstream>
#include <sstream>

std::string SaveManager::serialize(const GameSaveData& data) {
    std::ostringstream oss;
    
    oss << "{\n";
    oss << "  \"version\": 1,\n";
    oss << "  \"level\": " << data.level << ",\n";
    oss << "  \"field\": {\"width\": " << data.fieldWidth << ", \"height\": " << data.fieldHeight << "},\n";
    
    oss << "  \"player\": {\n";
    oss << "    \"name\": \"" << data.playerName << "\",\n";
    oss << "    \"health\": " << data.playerHealth << ",\n";
    oss << "    \"maxHealth\": " << data.playerMaxHealth << ",\n";
    oss << "    \"mana\": " << data.playerMana << ",\n";
    oss << "    \"maxMana\": " << data.playerMaxMana << ",\n";
    oss << "    \"score\": " << data.playerScore << ",\n";
    oss << "    \"x\": " << data.playerX << ",\n";
    oss << "    \"y\": " << data.playerY << ",\n";
    oss << "    \"combatMode\": " << data.combatMode << "\n";
    oss << "  },\n";
    
    oss << "  \"enemies\": [\n";
    for (size_t i = 0; i < data.enemies.size(); ++i) {
        const auto& e = data.enemies[i];
        oss << "    {";
        oss << "\"x\": " << e.x << ", ";
        oss << "\"y\": " << e.y << ", ";
        oss << "\"health\": " << e.health << ", ";
        oss << "\"damage\": " << e.damage << ", ";
        oss << "\"alive\": " << (e.alive ? "true" : "false");
        oss << "}";
        if (i < data.enemies.size() - 1) oss << ",";
        oss << "\n";
    }
    oss << "  ],\n";
    
    oss << "  \"impassableCells\": [\n";
    for (size_t i = 0; i < data.impassableCells.size(); ++i) {
        const auto& c = data.impassableCells[i];
        oss << "    {\"x\": " << c.x << ", \"y\": " << c.y << "}";
        if (i < data.impassableCells.size() - 1) oss << ",";
        oss << "\n";
    }
    oss << "  ],\n";
    
    oss << "  \"spells\": [\n";
    for (size_t i = 0; i < data.spells.size(); ++i) {
        const auto& s = data.spells[i];
        oss << "    {";
        oss << "\"name\": \"" << s.name << "\", ";
        oss << "\"description\": \"" << s.description << "\", ";
        oss << "\"manaCost\": " << s.manaCost << ", ";
        oss << "\"type\": " << s.type << ", ";
        oss << "\"damage\": " << s.damage << ", ";
        oss << "\"range\": " << s.range;
        if (s.type == 1) {
            oss << ", \"radius\": " << s.radius;
        }
        oss << "}";
        if (i < data.spells.size() - 1) oss << ",";
        oss << "\n";
    }
    oss << "  ],\n";
    
    oss << "  \"selectedSpellIndex\": " << data.selectedSpellIndex << "\n";
    oss << "}\n";
    
    return oss.str();
}

GameSaveData SaveManager::deserialize(const std::string& json) {
    GameSaveData data;
    
    data.level = 1;
    data.playerHealth = 100;
    data.playerMaxHealth = 100;
    data.playerMana = 50;
    data.playerMaxMana = 100;
    data.playerScore = 0;
    data.playerX = 5;
    data.playerY = 5;
    data.combatMode = 0;
    data.fieldWidth = 15;
    data.fieldHeight = 15;
    data.selectedSpellIndex = -1;
    
    try {
        auto findNumber = [&json](const std::string& key) -> int {
            size_t pos = json.find("\"" + key + "\":");
            if (pos == std::string::npos) return 0;
            pos = json.find(":", pos);
            if (pos == std::string::npos) return 0;
            pos++;
            
            while (pos < json.size() && std::isspace(json[pos])) pos++;
            
            std::string num;
            while (pos < json.size() && (std::isdigit(json[pos]) || json[pos] == '-')) {
                num += json[pos];
                pos++;
            }
            
            if (num.empty()) return 0;
            return std::stoi(num);
        };
        
        auto findString = [&json](const std::string& key) -> std::string {
            size_t pos = json.find("\"" + key + "\":");
            if (pos == std::string::npos) return "";
            pos = json.find(":", pos);
            if (pos == std::string::npos) return "";
            pos++;
            
            while (pos < json.size() && std::isspace(json[pos])) pos++;
            
            if (json[pos] != '"') return "";
            pos++;
            
            size_t endPos = json.find("\"", pos);
            if (endPos == std::string::npos) return "";
            
            return json.substr(pos, endPos - pos);
        };
        
        data.level = findNumber("level");
        data.playerName = findString("name");
        data.playerHealth = findNumber("health");
        data.playerMaxHealth = findNumber("maxHealth");
        data.playerMana = findNumber("mana");
        data.playerMaxMana = findNumber("maxMana");
        data.playerScore = findNumber("score");
        data.playerX = findNumber("x");
        data.playerY = findNumber("y");
        data.combatMode = findNumber("combatMode");
        data.fieldWidth = findNumber("width");
        data.fieldHeight = findNumber("height");
        data.selectedSpellIndex = findNumber("selectedSpellIndex");
        
        size_t wallsStart = json.find("\"impassableCells\":");
        if (wallsStart != std::string::npos) {
            size_t arrayStart = json.find("[", wallsStart);
            if (arrayStart != std::string::npos) {
                size_t arrayEnd = json.find("]", arrayStart);
                if (arrayEnd != std::string::npos) {
                    std::string wallsStr = json.substr(arrayStart + 1, arrayEnd - arrayStart - 1);
                    
                    size_t pos = 0;
                    while (pos < wallsStr.size()) {
                        size_t objStart = wallsStr.find("{", pos);
                        if (objStart == std::string::npos) break;
                        size_t objEnd = wallsStr.find("}", objStart);
                        if (objEnd == std::string::npos) break;
                        
                        std::string wallStr = wallsStr.substr(objStart, objEnd - objStart + 1);
                        
                        GameSaveData::CellData cell;
                        
                        size_t xPos = wallStr.find("\"x\":");
                        if (xPos != std::string::npos) {
                            xPos = wallStr.find(":", xPos) + 1;
                            while (xPos < wallStr.size() && std::isspace(wallStr[xPos])) xPos++;
                            std::string xNum;
                            while (xPos < wallStr.size() && (std::isdigit(wallStr[xPos]) || wallStr[xPos] == '-')) {
                                xNum += wallStr[xPos];
                                xPos++;
                            }
                            if (!xNum.empty()) cell.x = std::stoi(xNum);
                        }
                        
                        size_t yPos = wallStr.find("\"y\":");
                        if (yPos != std::string::npos) {
                            yPos = wallStr.find(":", yPos) + 1;
                            while (yPos < wallStr.size() && std::isspace(wallStr[yPos])) yPos++;
                            std::string yNum;
                            while (yPos < wallStr.size() && (std::isdigit(wallStr[yPos]) || wallStr[yPos] == '-')) {
                                yNum += wallStr[yPos];
                                yPos++;
                            }
                            if (!yNum.empty()) cell.y = std::stoi(yNum);
                        }
                        
                        cell.type = 1;
                        data.impassableCells.push_back(cell);
                        
                        pos = objEnd + 1;
                    }
                }
            }
        }
        
        size_t enemiesStart = json.find("\"enemies\":");
        if (enemiesStart != std::string::npos) {
            size_t arrayStart = json.find("[", enemiesStart);
            if (arrayStart != std::string::npos) {
                size_t arrayEnd = json.find("]", arrayStart);
                if (arrayEnd != std::string::npos) {
                    std::string enemiesStr = json.substr(arrayStart + 1, arrayEnd - arrayStart - 1);
                    
                    size_t pos = 0;
                    while (pos < enemiesStr.size()) {
                        size_t objStart = enemiesStr.find("{", pos);
                        if (objStart == std::string::npos) break;
                        size_t objEnd = enemiesStr.find("}", objStart);
                        if (objEnd == std::string::npos) break;
                        
                        std::string enemyStr = enemiesStr.substr(objStart, objEnd - objStart + 1);
                        
                        GameSaveData::EnemyData enemy;
                        
                        auto getEnemyInt = [&enemyStr](const std::string& key) -> int {
                            size_t p = enemyStr.find("\"" + key + "\":");
                            if (p == std::string::npos) return 0;
                            p = enemyStr.find(":", p) + 1;
                            while (p < enemyStr.size() && std::isspace(enemyStr[p])) p++;
                            std::string num;
                            while (p < enemyStr.size() && (std::isdigit(enemyStr[p]) || enemyStr[p] == '-')) {
                                num += enemyStr[p];
                                p++;
                            }
                            return num.empty() ? 0 : std::stoi(num);
                        };
                        
                        auto getEnemyBool = [&enemyStr](const std::string& key) -> bool {
                            size_t p = enemyStr.find("\"" + key + "\":");
                            if (p == std::string::npos) return false;
                            p = enemyStr.find(":", p) + 1;
                            while (p < enemyStr.size() && std::isspace(enemyStr[p])) p++;
                            return enemyStr.substr(p, 4) == "true";
                        };
                        
                        enemy.x = getEnemyInt("x");
                        enemy.y = getEnemyInt("y");
                        enemy.health = getEnemyInt("health");
                        enemy.damage = getEnemyInt("damage");
                        enemy.alive = getEnemyBool("alive");
                        
                        data.enemies.push_back(enemy);
                        pos = objEnd + 1;
                    }
                }
            }
        }
        
    } catch (const std::exception& e) {
        throw InvalidSaveDataException("Failed to parse save file: " + std::string(e.what()));
    }
    
    return data;
}

void SaveManager::saveGame(const GameSaveData& data, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw FileOpenException(filename);
    }
    
    std::string jsonData = serialize(data);
    file << jsonData;
    file.close();
    
    if (file.fail()) {
        throw SaveLoadException("Failed to write to file: " + filename);
    }
}

GameSaveData SaveManager::loadGame(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw FileOpenException(filename);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    if (buffer.str().empty()) {
        throw InvalidSaveDataException("File is empty");
    }
    
    return deserialize(buffer.str());
}