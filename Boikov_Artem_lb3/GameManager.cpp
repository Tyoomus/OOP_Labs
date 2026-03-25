#include "GameManager.h"
#include "GameEngine.h"
#include "DirectDamageSpell.h"
#include "AreaDamageSpell.h"
#include "SpellBook.h"
#include "SaveManager.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>
#include <limits>

GameManager::GameManager() 
    : player("Hero", 5, 5), field(15, 15), gameRunning(false),
      spellTargetX(5), spellTargetY(5), selectingSpellTarget(false), 
      selectedSpellIndex(0), gameEngine(nullptr) {
    field.generateImpassableCells(15);
}

GameManager::GameManager(GameEngine* engine) 
    : player("Hero", 5, 5), field(15, 15), gameRunning(false),
      spellTargetX(5), spellTargetY(5), selectingSpellTarget(false), 
      selectedSpellIndex(0), gameEngine(engine) {
    field.generateImpassableCells(15);
}

void GameManager::setGameEngine(GameEngine* engine) {
    gameEngine = engine;
}

void GameManager::startGame() {
    initializeGame();
    gameRunning = true;
    std::cout << "Game started! Use WASD to move, 'c' to switch combat mode, 'f' to attack." << std::endl;
    std::cout << "Special commands: 's' - Save game, 'l' - Load game, 'm' - Main menu, 'q' - Quit" << std::endl;
    std::cout << "In spell mode: 1-5 select spell, WASD move cursor, T target mode, SPACE cast, Q quit" << std::endl;
}

void GameManager::initializeGame() {
    field.placePlayer(5, 5);
    
    enemies.emplace_back(3, 3, 30, 5);
    enemies.emplace_back(12, 7, 30, 5);
    enemies.emplace_back(8, 12, 30, 5);
    
    for (const auto& enemy : enemies) {
        field.placeEnemy(enemy.getPositionX(), enemy.getPositionY());
    }
}

void GameManager::displayGameState() const {
    std::cout << "\n=== Game State ===" << std::endl;
    std::cout << "Player: " << player.getName() << std::endl;
    std::cout << "Health: " << player.getHealth() << "/" << player.getMaxHealth() << std::endl;
    std::cout << "Mana: " << player.getMana() << "/" << player.getMaxMana() << std::endl;
    std::cout << "Position: (" << player.getPositionX() << ", " << player.getPositionY() << ")" << std::endl;
    std::cout << "Combat Mode: ";
    
    switch (player.getCombatMode()) {
        case CombatMode::MELEE: std::cout << "MELEE"; break;
        case CombatMode::RANGED: std::cout << "RANGED"; break;
        case CombatMode::CASTING: std::cout << "SPELL CASTING"; break;
    }
    
    std::cout << std::endl;
    
    if (player.getCombatMode() == CombatMode::MELEE || player.getCombatMode() == CombatMode::RANGED) {
        std::cout << "Damage: " << player.getDamage() << std::endl;
        std::cout << "Attack Range: " << player.getAttackRange() << std::endl;
    }
    
    std::cout << "Score: " << player.getScore() << std::endl;
    
    if (player.getCombatMode() == CombatMode::CASTING) {
        displaySpellInfo();
    }
    
    int aliveCount = 0;
    for (const auto& enemy : enemies) {
        if (enemy.isAlive()) {
            aliveCount++;
        }
    }
    std::cout << "Enemies alive: " << aliveCount << "/" << enemies.size() << std::endl;
    
    for (size_t i = 0; i < enemies.size(); ++i) {
        if (enemies[i].isAlive()) {
            std::cout << "Enemy " << (i+1) << " HP: " << enemies[i].getHealth() << " at (" 
                      << enemies[i].getPositionX() << ", " << enemies[i].getPositionY() << ")" << std::endl;
        }
    }
    
    std::cout << "\n=== GAME FIELD ===" << std::endl;
    displayField();
}

void GameManager::displaySpellInfo() const {
    const SpellHand& hand = player.getSpellHand();
    std::cout << "\n=== Spells ===" << std::endl;
    std::cout << "Selected spell: ";
    
    if (hand.getSelectedSpell()) {
        const Spell* spell = hand.getSelectedSpell();
        std::cout << spell->getName() 
                  << " (Mana: " << spell->getManaCost() 
                  << ", Range: " << spell->getCastRange() << ")";
    } else {
        std::cout << "None";
    }
    
    std::cout << std::endl;
    
    for (int i = 0; i < hand.getSpellCount(); ++i) {
        const Spell* spell = hand.getSpell(i);
        std::cout << (i == hand.getSelectedSpellIndex() ? "> " : "  ") 
                  << (i+1) << ". " << spell->getName() 
                  << " - " << spell->getDescription() 
                  << " [" << spell->getManaCost() << " mana]" << std::endl;
    }
}

void GameManager::displayField() const {
    int width = field.getWidth();
    int height = field.getHeight();
    
    std::cout << "\n=== GAME FIELD ===" << std::endl;
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const Cell& cell = field.getCell(x, y);
            
            if (selectingSpellTarget && x == spellTargetX && y == spellTargetY) {
                std::cout << "X ";
            }
            else if (cell.getHasPlayer()) {
                std::cout << "P ";
            }
            else if (cell.getHasEnemy()) {
                bool enemyAliveHere = false;
                for (const auto& enemy : enemies) {
                    if (enemy.isAlive() && enemy.getPositionX() == x && enemy.getPositionY() == y) {
                        enemyAliveHere = true;
                        break;
                    }
                }
                if (enemyAliveHere) {
                    std::cout << "E ";
                } else {
                    std::cout << ". ";
                }
            }
            else if (cell.getType() == CellType::IMPASSABLE) {
                std::cout << "# ";
            }
            else {
                std::cout << ". ";
            }
        }
        std::cout << std::endl;
    }
    
    std::cout << "\nLEGEND: P=You E=Enemy #=Wall .=Empty";
    if (selectingSpellTarget) {
        std::cout << " X=Spell Target";
    }
    std::cout << std::endl;
    
    std::cout << "Your position: (" << player.getPositionX() << ", " << player.getPositionY() << ")" << std::endl;
    
    if (selectingSpellTarget) {
        std::cout << "Target position: (" << spellTargetX << ", " << spellTargetY << ")" << std::endl;
    }
    
    bool hasAliveEnemies = false;
    for (size_t i = 0; i < enemies.size(); ++i) {
        if (enemies[i].isAlive()) {
            if (!hasAliveEnemies) {
                std::cout << "Enemy positions: ";
                hasAliveEnemies = true;
            }
            std::cout << "(" << enemies[i].getPositionX() << "," << enemies[i].getPositionY() << ") ";
        }
    }
    if (hasAliveEnemies) {
        std::cout << std::endl;
    }
}

void GameManager::processTurn() {
    if (!gameRunning) return;
    
    displayGameState();
    
    if (selectingSpellTarget) {
        processSpellTargeting();
    } else {
        processPlayerTurn();
    }
    
    if (gameRunning && !selectingSpellTarget) {
        player.regenerateMana();
        processEnemiesTurn();
        checkGameOver();
    }
}

void GameManager::processPlayerTurn() {
    if (player.getCombatMode() == CombatMode::CASTING) {
        processSpellSelection();
        return;
    }
    
    std::cout << "\nYour turn! Enter command:" << std::endl;
    std::cout << "  WASD - Move" << std::endl;
    std::cout << "  C - Switch combat mode" << std::endl;
    std::cout << "  F - Attack" << std::endl;
    std::cout << "  K - Save game" << std::endl;
    std::cout << "  L - Load game" << std::endl;
    std::cout << "  M - Main menu" << std::endl;
    std::cout << "  Q - Quit" << std::endl;
    std::cout << "Choice: ";
    
    char input;
    std::cin >> input;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    int newX = player.getPositionX();
    int newY = player.getPositionY();
    bool shouldMove = false;
    
    switch (input) {
        case 'w': case 'W': 
            newY--; 
            shouldMove = true;
            break;
        case 'a': case 'A': 
            newX--; 
            shouldMove = true;
            break;
        case 's': case 'S': 
            newY++; 
            shouldMove = true;
            break;
        case 'd': case 'D': 
            newX++; 
            shouldMove = true;
            break;
        case 'k': case 'K':
            if (gameEngine) {
                gameEngine->requestSave();
                std::cout << "Saving game..." << std::endl;
            } else {
                std::cout << "ERROR: Cannot save - game engine not connected!" << std::endl;
            }
            return;
        case 'l': case 'L':
            if (gameEngine) {
                gameEngine->requestLoad();
                std::cout << "Loading game..." << std::endl;
            } else {
                std::cout << "ERROR: Cannot load - game engine not connected!" << std::endl;
            }
            return;
        case 'm': case 'M':
            if (gameEngine) {
                gameRunning = false;
                std::cout << "Returning to main menu..." << std::endl;
            }
            return;
        case 'c': case 'C':
            player.switchCombatMode();
            std::cout << "Switched to ";
            switch (player.getCombatMode()) {
                case CombatMode::MELEE: std::cout << "MELEE"; break;
                case CombatMode::RANGED: std::cout << "RANGED"; break;
                case CombatMode::CASTING: std::cout << "SPELL CASTING"; break;
            }
            std::cout << " combat mode!";
            if (player.getCombatMode() == CombatMode::MELEE || player.getCombatMode() == CombatMode::RANGED) {
                std::cout << " Damage: " << player.getDamage() 
                          << ", Range: " << player.getAttackRange();
            }
            std::cout << std::endl;
            return;
        case 'f': case 'F':
            performAttack();
            return;
        case 'q': case 'Q':
            gameRunning = false;
            std::cout << "Game ended by player." << std::endl;
            return;
        default:
            std::cout << "Invalid command!" << std::endl;
            return;
    }
    
    if (shouldMove) {
        int oldX = player.getPositionX();
        int oldY = player.getPositionY();
        
        bool enemyOnTargetCell = false;
        for (auto& enemy : enemies) {
            if (enemy.isAlive() && enemy.getPositionX() == newX && enemy.getPositionY() == newY) {
                enemyOnTargetCell = true;
                
                if (player.getCombatMode() == CombatMode::MELEE) {
                    int oldEnemyHealth = enemy.getHealth();
                    enemy.takeDamage(player.getDamage());
                    std::cout << "You attacked enemy in melee for " << player.getDamage() << " damage!" << std::endl;
                    std::cout << "Enemy health: " << oldEnemyHealth << " -> " << enemy.getHealth() << std::endl;
                    
                    if (!enemy.isAlive()) {
                        std::cout << "Enemy defeated! +10 points" << std::endl;
                        player.addScore(10);
                        player.addSpellPoints(10);
                        field.removeEnemy(enemy.getPositionX(), enemy.getPositionY());
                        
                        std::random_device rd;
                        std::mt19937 gen(rd());
                        std::uniform_int_distribution<> dist(0, 99);
                        if (dist(gen) < 30 && player.getSpellHand().canAddSpell()) {
                            addRandomSpellToPlayer();
                        }
                    }
                } else {
                    std::cout << "Cannot melee attack in ranged mode! Use attack command (F)." << std::endl;
                }
                break;
            }
        }
        
        if (!enemyOnTargetCell && field.isCellPassable(newX, newY)) {
            field.removePlayer(oldX, oldY);
            field.placePlayer(newX, newY);
            player.setPosition(newX, newY);
            std::cout << "Moved to (" << newX << ", " << newY << ")" << std::endl;
        } else if (!enemyOnTargetCell) {
            std::cout << "Cannot move there!" << std::endl;
        }
    }
}

void GameManager::processSpellSelection() {
    std::cout << "\nSpell mode! Select spell (1-5), WASD to move, C to switch mode, T to target spell, Q to quit: ";
    char input;
    std::cin >> input;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    int newX = player.getPositionX();
    int newY = player.getPositionY();
    
    switch (input) {
        case 'w': case 'W': newY--; break;
        case 'a': case 'A': newX--; break;
        case 's': case 'S': newY++; break;
        case 'd': case 'D': newX++; break;
        case 'c': case 'C':
            player.switchCombatMode();
            std::cout << "Switched to " << (player.getCombatMode() == CombatMode::MELEE ? "MELEE" : "RANGED") 
                      << " combat mode! Damage: " << player.getDamage() 
                      << ", Range: " << player.getAttackRange() << std::endl;
            return;
        case '1': case '2': case '3': case '4': case '5': {
            int spellIndex = input - '1';
            player.getSpellHand().selectSpell(spellIndex);
            const Spell* spell = player.getSpellHand().getSelectedSpell();
            std::cout << "Selected spell " << (spellIndex + 1) << ": " 
                      << (spell ? spell->getName() : "None") << std::endl;
            return;
        }
        case 't': case 'T':
            if (player.getSpellHand().getSelectedSpell()) {
                selectingSpellTarget = true;
                spellTargetX = player.getPositionX();
                spellTargetY = player.getPositionY();
                std::cout << "Targeting mode activated for " 
                          << player.getSpellHand().getSelectedSpell()->getName() << std::endl;
                std::cout << "Use WASD to move cursor, SPACE to cast, Q to cancel" << std::endl;
            } else {
                std::cout << "No spell selected! Press 1-5 to select a spell first." << std::endl;
            }
            return;
        case 'q': case 'Q':
            gameRunning = false;
            std::cout << "Game ended by player." << std::endl;
            return;
        default:
            std::cout << "Invalid command!" << std::endl;
            return;
    }
    
    int oldX = player.getPositionX();
    int oldY = player.getPositionY();
    
    if (field.isCellPassable(newX, newY)) {
        field.removePlayer(oldX, oldY);
        field.placePlayer(newX, newY);
        player.setPosition(newX, newY);
        std::cout << "Moved to (" << newX << ", " << newY << ")" << std::endl;
    } else {
        std::cout << "Cannot move there!" << std::endl;
    }
}

void GameManager::processSpellTargeting() {
    if (!player.getSpellHand().getSelectedSpell()) {
        selectingSpellTarget = false;
        std::cout << "No spell selected. Returning to spell selection." << std::endl;
        return;
    }
    
    const Spell* spell = player.getSpellHand().getSelectedSpell();
    
    std::cout << "\n=== TARGETING MODE ===" << std::endl;
    std::cout << "Spell: " << spell->getName() << " (Range: " << spell->getCastRange() << ")" << std::endl;
    std::cout << "Your position: (" << player.getPositionX() << ", " << player.getPositionY() << ")" << std::endl;
    std::cout << "Target position: (" << spellTargetX << ", " << spellTargetY << ")" << std::endl;
    
    int distance = std::abs(spellTargetX - player.getPositionX()) + std::abs(spellTargetY - player.getPositionY());
    std::cout << "Distance: " << distance << "/" << spell->getCastRange() << std::endl;
    
    std::cout << "\nCommand (WASD move cursor, SPACE cast, Q cancel): ";
    
    std::string input;
    std::getline(std::cin, input);
    
    if (input.empty()) {
        std::cout << "Please enter a command." << std::endl;
        return;
    }
    
    char command = input[0];
    
    switch (command) {
        case 'w': case 'W': 
            spellTargetY--;
            std::cout << "Cursor moved UP to (" << spellTargetX << ", " << spellTargetY << ")" << std::endl;
            break;
        case 'a': case 'A': 
            spellTargetX--;
            std::cout << "Cursor moved LEFT to (" << spellTargetX << ", " << spellTargetY << ")" << std::endl;
            break;
        case 's': case 'S': 
            spellTargetY++;
            std::cout << "Cursor moved DOWN to (" << spellTargetX << ", " << spellTargetY << ")" << std::endl;
            break;
        case 'd': case 'D': 
            spellTargetX++;
            std::cout << "Cursor moved RIGHT to (" << spellTargetX << ", " << spellTargetY << ")" << std::endl;
            break;
        case ' ': {
            std::cout << "Attempting to cast " << spell->getName() << "..." << std::endl;
            
            if (!player.hasEnoughMana(spell->getManaCost())) {
                std::cout << "Not enough mana! Need " << spell->getManaCost() 
                          << " mana, but have " << player.getMana() << std::endl;
                return;
            }
            
            if (spell->canCast(player.getPositionX(), player.getPositionY(), spellTargetX, spellTargetY)) {
                spell->cast(*this, player.getPositionX(), player.getPositionY(), spellTargetX, spellTargetY);
                player.spendMana(spell->getManaCost());
                selectingSpellTarget = false;
                std::cout << "Returning to normal mode." << std::endl;
            } else {
                std::cout << "Cannot cast! Distance: " << distance 
                          << ", Max range: " << spell->getCastRange() << std::endl;
            }
            return;
        }
        case 'q': case 'Q':
            selectingSpellTarget = false;
            std::cout << "Spell targeting cancelled. Returning to spell selection." << std::endl;
            return;
        default:
            std::cout << "Invalid command! Use: W(up), A(left), S(down), D(right), SPACE(cast), Q(cancel)" << std::endl;
            return;
    }
    
    if (spellTargetX < 0) {
        spellTargetX = 0;
        std::cout << "Cursor hit left border." << std::endl;
    }
    if (spellTargetX >= field.getWidth()) {
        spellTargetX = field.getWidth() - 1;
        std::cout << "Cursor hit right border." << std::endl;
    }
    if (spellTargetY < 0) {
        spellTargetY = 0;
        std::cout << "Cursor hit top border." << std::endl;
    }
    if (spellTargetY >= field.getHeight()) {
        spellTargetY = field.getHeight() - 1;
        std::cout << "Cursor hit bottom border." << std::endl;
    }
}

void GameManager::performAttack() {
    if (player.getCombatMode() == CombatMode::MELEE) {
        std::cout << "You are in melee mode. Move into enemies to attack!" << std::endl;
        return;
    }
    
    int playerX = player.getPositionX();
    int playerY = player.getPositionY();
    int attackRange = player.getAttackRange();
    bool attacked = false;
    
    for (auto& enemy : enemies) {
        if (enemy.isAlive()) {
            int enemyX = enemy.getPositionX();
            int enemyY = enemy.getPositionY();
            
            int distance = std::abs(enemyX - playerX) + std::abs(enemyY - playerY);
            
            if (distance <= attackRange && distance > 0) {
                int oldEnemyHealth = enemy.getHealth();
                enemy.takeDamage(player.getDamage());
                std::cout << "You ranged attacked enemy at (" << enemyX << ", " << enemyY 
                          << ") for " << player.getDamage() << " damage! Distance: " << distance << std::endl;
                std::cout << "Enemy health: " << oldEnemyHealth << " -> " << enemy.getHealth() << std::endl;
                attacked = true;
                
                if (!enemy.isAlive()) {
                    std::cout << "Enemy defeated! +10 points" << std::endl;
                    player.addScore(10);
                    player.addSpellPoints(10);
                    field.removeEnemy(enemy.getPositionX(), enemy.getPositionY());
                    
                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_int_distribution<> dist(0, 99);
                    if (dist(gen) < 30 && player.getSpellHand().canAddSpell()) {
                        addRandomSpellToPlayer();
                    }
                }
            }
        }
    }
    
    if (!attacked) {
        std::cout << "No enemies in attack range (" << attackRange << " cells)!" << std::endl;
    }
}

void GameManager::castDirectDamageSpell(const DirectDamageSpell& spell, int targetX, int targetY) {
    bool hitEnemy = false;
    
    for (auto& enemy : enemies) {
        if (enemy.isAlive() && 
            enemy.getPositionX() == targetX && 
            enemy.getPositionY() == targetY) {
            
            int oldEnemyHealth = enemy.getHealth();
            enemy.takeDamage(spell.getDamageAmount());
            
            std::cout << spell.getName() << " hit enemy at (" 
                      << targetX << ", " << targetY 
                      << ") for " << spell.getDamageAmount() << " damage!" << std::endl;
            std::cout << "Enemy health: " << oldEnemyHealth << " -> " 
                      << enemy.getHealth() << std::endl;
            
            if (!enemy.isAlive()) {
                std::cout << "Enemy defeated! +10 points" << std::endl;
                player.addScore(10);
                player.addSpellPoints(10);
                field.removeEnemy(enemy.getPositionX(), enemy.getPositionY());
            }
            
            hitEnemy = true;
            break;
        }
    }
    
    if (!hitEnemy) {
        std::cout << spell.getName() << " hit empty space at (" 
                  << targetX << ", " << targetY << ")" << std::endl;
    }
}

void GameManager::castAreaDamageSpell(const AreaDamageSpell& spell, int targetX, int targetY) {
    std::cout << spell.getName() << " explodes at (" 
              << targetX << ", " << targetY << ")!" << std::endl;
    
    int enemiesHit = 0;
    int radius = spell.getAreaRadius();
    
    for (int dx = -radius; dx <= radius; ++dx) {
        for (int dy = -radius; dy <= radius; ++dy) {
            int checkX = targetX + dx;
            int checkY = targetY + dy;
            
            if (!field.isValidPosition(checkX, checkY)) {
                continue;
            }
            
            for (auto& enemy : enemies) {
                if (enemy.isAlive() && 
                    enemy.getPositionX() == checkX && 
                    enemy.getPositionY() == checkY) {
                    
                    int oldEnemyHealth = enemy.getHealth();
                    enemy.takeDamage(spell.getDamageAmount());
                    
                    std::cout << "  Enemy at (" << checkX << ", " << checkY 
                              << ") takes " << spell.getDamageAmount() << " damage!" << std::endl;
                    
                    if (!enemy.isAlive()) {
                        std::cout << "  Enemy defeated! +10 points" << std::endl;
                        player.addScore(10);
                        player.addSpellPoints(10);
                        field.removeEnemy(enemy.getPositionX(), enemy.getPositionY());
                    }
                    
                    enemiesHit++;
                }
            }
        }
    }
    
    if (enemiesHit == 0) {
        std::cout << "The spell hits nothing but empty ground." << std::endl;
    } else {
        std::cout << "Total enemies hit: " << enemiesHit << std::endl;
    }
}

void GameManager::processEnemiesTurn() {
    for (auto& enemy : enemies) {
        if (enemy.isAlive()) {
            int oldX = enemy.getPositionX();
            int oldY = enemy.getPositionY();
            
            field.removeEnemy(oldX, oldY);
            
            enemy.moveTowardsPlayer(player.getPositionX(), player.getPositionY());
            int newX = enemy.getPositionX();
            int newY = enemy.getPositionY();
            
            if (newX == player.getPositionX() && newY == player.getPositionY()) {
                int oldPlayerHealth = player.getHealth();
                player.takeDamage(enemy.getDamage());
                std::cout << "Enemy attacked you for " << enemy.getDamage() << " damage!" << std::endl;
                std::cout << "Your health: " << oldPlayerHealth << " -> " << player.getHealth() << "/100" << std::endl;
                
                enemy.setPosition(oldX, oldY);
                field.placeEnemy(oldX, oldY);
            }
            else if (field.isCellPassable(newX, newY)) {
                field.placeEnemy(newX, newY);
                std::cout << "Enemy moved to (" << newX << ", " << newY << ")" << std::endl;
            } else {
                enemy.setPosition(oldX, oldY);
                field.placeEnemy(oldX, oldY);
                std::cout << "Enemy cannot move to (" << newX << ", " << newY << ")" << std::endl;
            }
        }
    }
}

void GameManager::checkGameOver() {
    if (!player.isAlive()) {
        gameRunning = false;
        std::cout << "\n=== GAME OVER ===" << std::endl;
        std::cout << "Final score: " << player.getScore() << std::endl;
        return;
    }
    
    bool allEnemiesDefeated = true;
    for (const auto& enemy : enemies) {
        if (enemy.isAlive()) {
            allEnemiesDefeated = false;
            break;
        }
    }
    
    if (allEnemiesDefeated) {
        gameRunning = false;
        std::cout << "\n=== VICTORY! ===" << std::endl;
        std::cout << "You defeated all enemies! Final score: " << player.getScore() << std::endl;
    }
}

bool GameManager::isGameRunning() const {
    return gameRunning;
}

void GameManager::addRandomSpellToPlayer() {
    auto randomSpell = SpellBook::createRandomSpell();
    if (randomSpell && player.getSpellHand().canAddSpell()) {
        if (player.learnSpell(std::move(randomSpell))) {
            std::cout << "You learned a new spell: " 
                      << player.getSpellHand().getSpell(player.getSpellHand().getSpellCount() - 1)->getName() 
                      << "!" << std::endl;
        }
    }
}

GameSaveData GameManager::getSaveData() const {
    GameSaveData data;
    
    data.playerName = player.getName();
    data.playerHealth = player.getHealth();
    data.playerMaxHealth = player.getMaxHealth();
    data.playerMana = player.getMana();
    data.playerMaxMana = player.getMaxMana();
    data.playerScore = player.getScore();
    data.playerX = player.getPositionX();
    data.playerY = player.getPositionY();
    data.combatMode = static_cast<int>(player.getCombatMode());
    
    for (const auto& enemy : enemies) {
        GameSaveData::EnemyData enemyData;
        enemyData.x = enemy.getPositionX();
        enemyData.y = enemy.getPositionY();
        enemyData.health = enemy.getHealth();
        enemyData.damage = enemy.getDamage();
        enemyData.alive = enemy.isAlive();
        data.enemies.push_back(enemyData);
    }
    
    for (int y = 0; y < field.getHeight(); ++y) {
        for (int x = 0; x < field.getWidth(); ++x) {
            if (field.getCell(x, y).getType() == CellType::IMPASSABLE) {
                GameSaveData::CellData cellData;
                cellData.x = x;
                cellData.y = y;
                cellData.type = static_cast<int>(CellType::IMPASSABLE);
                data.impassableCells.push_back(cellData);
            }
        }
    }
    
    const SpellHand& hand = player.getSpellHand();
    for (int i = 0; i < hand.getSpellCount(); ++i) {
        const Spell* spell = hand.getSpell(i);
        GameSaveData::SpellData spellData;
        spellData.name = spell->getName();
        spellData.description = spell->getDescription();
        spellData.manaCost = spell->getManaCost();
        spellData.range = spell->getCastRange();
        
        const DirectDamageSpell* direct = dynamic_cast<const DirectDamageSpell*>(spell);
        if (direct) {
            spellData.type = 0;
            spellData.damage = direct->getDamageAmount();
            spellData.radius = 0;
        } else {
            const AreaDamageSpell* area = dynamic_cast<const AreaDamageSpell*>(spell);
            if (area) {
                spellData.type = 1;
                spellData.damage = area->getDamageAmount();
                spellData.radius = area->getAreaRadius();
            }
        }
        
        data.spells.push_back(spellData);
    }
    data.selectedSpellIndex = hand.getSelectedSpellIndex();
    
    int width = field.getWidth();
    int height = field.getHeight();
    
    if (width < 10) width = 10;
    if (width > 25) width = 25;
    if (height < 10) height = 10;
    if (height > 25) height = 25;
    
    data.fieldWidth = width;
    data.fieldHeight = height;
    data.level = 1;
    
    return data;
}

bool GameManager::loadFromData(const GameSaveData& data) {
    try {
        int newWidth = data.fieldWidth;
        int newHeight = data.fieldHeight;
        
        if (newWidth < 10) newWidth = 10;
        if (newWidth > 25) newWidth = 25;
        if (newHeight < 10) newHeight = 10;
        if (newHeight > 25) newHeight = 25;
        
        field = GameField(newWidth, newHeight);
        
        for (const auto& cell : data.impassableCells) {
            if (cell.x >= 0 && cell.x < newWidth && cell.y >= 0 && cell.y < newHeight) {
                field.setCellType(cell.x, cell.y, CellType::IMPASSABLE);
            }
        }
        
        int playerX = data.playerX;
        int playerY = data.playerY;
        
        if (playerX >= 0 && playerX < newWidth && playerY >= 0 && playerY < newHeight) {
            if (field.getCell(playerX, playerY).getType() == CellType::IMPASSABLE) {
                bool found = false;
                for (int dy = -1; dy <= 1 && !found; ++dy) {
                    for (int dx = -1; dx <= 1 && !found; ++dx) {
                        int nx = playerX + dx;
                        int ny = playerY + dy;
                        if (nx >= 0 && nx < newWidth && ny >= 0 && ny < newHeight) {
                            if (field.getCell(nx, ny).getType() != CellType::IMPASSABLE) {
                                playerX = nx;
                                playerY = ny;
                                found = true;
                            }
                        }
                    }
                }
            }
        }
        
        player.setPosition(playerX, playerY);
        field.placePlayer(playerX, playerY);
        
        player.heal(data.playerHealth - player.getHealth());
        player.restoreMana(data.playerMana - player.getMana());
        
        int currentScore = player.getScore();
        player.addScore(data.playerScore - currentScore);
        
        enemies.clear();
        for (const auto& enemyData : data.enemies) {
            if (enemyData.alive && enemyData.health > 0) {
                int enemyX = enemyData.x;
                int enemyY = enemyData.y;
                
                if (enemyX >= 0 && enemyX < newWidth && enemyY >= 0 && enemyY < newHeight) {
                    if (field.getCell(enemyX, enemyY).getType() != CellType::IMPASSABLE && 
                        !field.getCell(enemyX, enemyY).getHasEnemy()) {
                        
                        Enemy enemy(enemyX, enemyY, enemyData.health, enemyData.damage);
                        enemies.push_back(enemy);
                        field.placeEnemy(enemyX, enemyY);
                    }
                }
            }
        }
        
        gameRunning = true;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading game data: " << e.what() << std::endl;
        return false;
    }
}

bool GameManager::areAllEnemiesDefeated() const {
    for (const auto& enemy : enemies) {
        if (enemy.isAlive()) {
            return false;
        }
    }
    return true;
}

const Player& GameManager::getPlayer() const {
    return player;
}

void GameManager::nextLevel() {
    int newWidth = field.getWidth() + 2;
    int newHeight = field.getHeight() + 2;
    if (newWidth > 25) newWidth = 25;
    if (newHeight > 25) newHeight = 25;
    
    field = GameField(newWidth, newHeight);
    field.generateImpassableCells(15);
    
    player.heal(player.getMaxHealth());
    
    SpellHand& hand = player.getSpellHand();
    int spellsToRemove = hand.getSpellCount() / 2;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    
    for (int i = 0; i < spellsToRemove; ++i) {
        if (hand.getSpellCount() > 0) {
            std::uniform_int_distribution<> dist(0, hand.getSpellCount() - 1);
            hand.removeSpell(dist(gen));
        }
    }
    
    int enemyCount = 3 + (newWidth - 10) / 2;
    enemies.clear();
    
    for (int i = 0; i < enemyCount; ++i) {
        int x, y;
        do {
            x = rand() % newWidth;
            y = rand() % newHeight;
        } while (!field.isCellPassable(x, y));
        
        int enemyHealth = 30 + (newWidth - 10) * 2;
        int enemyDamage = 5 + (newWidth - 10) / 2;
        
        Enemy enemy(x, y, enemyHealth, enemyDamage);
        enemies.push_back(enemy);
        field.placeEnemy(x, y);
    }
    
    int centerX = newWidth / 2;
    int centerY = newHeight / 2;
    
    if (field.isCellPassable(centerX, centerY)) {
        field.placePlayer(centerX, centerY);
        player.setPosition(centerX, centerY);
    } else {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                int nx = centerX + dx;
                int ny = centerY + dy;
                if (field.isCellPassable(nx, ny)) {
                    field.placePlayer(nx, ny);
                    player.setPosition(nx, ny);
                    break;
                }
            }
        }
    }
}