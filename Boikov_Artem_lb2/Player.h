#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include "SpellHand.h"

enum class CombatMode {
    MELEE,
    RANGED,
    CASTING  // Новый режим - кастование заклинаний
};

class Player {
private:
    std::string name;
    int health;
    int maxHealth;
    int damage;
    int score;
    int positionX;
    int positionY;
    CombatMode combatMode;
    int attackRange;
    
    // Новые поля для системы магии
    int mana;
    int maxMana;
    int manaRegenerationRate;
    SpellHand spellHand;
    
public:
    Player(const std::string& playerName, int startX, int startY);
    
    std::string getName() const;
    int getHealth() const;
    int getMaxHealth() const;
    int getDamage() const;
    int getScore() const;
    int getPositionX() const;
    int getPositionY() const;
    CombatMode getCombatMode() const;
    int getAttackRange() const;
    
    // Методы для работы с маной
    int getMana() const;
    int getMaxMana() const;
    int getManaRegenerationRate() const;
    bool hasEnoughMana(int cost) const;
    void spendMana(int amount);
    void regenerateMana();
    void restoreMana(int amount);
    
    // Методы для работы с заклинаниями
    const SpellHand& getSpellHand() const;
    SpellHand& getSpellHand();
    bool learnSpell(std::unique_ptr<Spell> spell);
    bool castSpell(int spellIndex, class GameManager& game, int targetX, int targetY);
    void addSpellPoints(int points);
    
    void setPosition(int x, int y);
    
    void takeDamage(int damage);
    void heal(int amount);
    void addScore(int points);
    void switchCombatMode();
    bool isAlive() const;
    
    bool move(int deltaX, int deltaY);
    
private:
    void initializeSpells();
};

#endif