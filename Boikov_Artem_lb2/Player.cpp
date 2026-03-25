#include "Player.h"
#include "SpellBook.h"
#include <iostream>

Player::Player(const std::string& playerName, int startX, int startY) 
    : name(playerName), health(100), maxHealth(100), damage(15), score(0), 
      positionX(startX), positionY(startY), combatMode(CombatMode::MELEE), attackRange(1),
      mana(50), maxMana(100), manaRegenerationRate(5), spellHand(5) {
    initializeSpells();
}

void Player::initializeSpells() {
    // Игрок начинает с одним случайным заклинанием
    auto randomSpell = SpellBook::createRandomSpell();
    if (randomSpell) {
        spellHand.addSpell(std::move(randomSpell));
    }
}

std::string Player::getName() const { return name; }
int Player::getHealth() const { return health; }
int Player::getMaxHealth() const { return maxHealth; }
int Player::getDamage() const { return damage; }
int Player::getScore() const { return score; }
int Player::getPositionX() const { return positionX; }
int Player::getPositionY() const { return positionY; }
CombatMode Player::getCombatMode() const { return combatMode; }
int Player::getAttackRange() const { return attackRange; }

int Player::getMana() const { return mana; }
int Player::getMaxMana() const { return maxMana; }
int Player::getManaRegenerationRate() const { return manaRegenerationRate; }

bool Player::hasEnoughMana(int cost) const {
    return mana >= cost;
}

void Player::spendMana(int amount) {
    mana -= amount;
    if (mana < 0) mana = 0;
}

void Player::regenerateMana() {
    mana += manaRegenerationRate;
    if (mana > maxMana) mana = maxMana;
}

void Player::restoreMana(int amount) {
    mana += amount;
    if (mana > maxMana) mana = maxMana;
}

const SpellHand& Player::getSpellHand() const {
    return spellHand;
}

SpellHand& Player::getSpellHand() {
    return spellHand;
}

bool Player::learnSpell(std::unique_ptr<Spell> spell) {
    return spellHand.addSpell(std::move(spell));
}

bool Player::castSpell(int spellIndex, GameManager& game, int targetX, int targetY) {
    const Spell* spell = spellHand.getSpell(spellIndex);
    if (!spell) {
        return false;
    }
    
    if (!hasEnoughMana(spell->getManaCost())) {
        std::cout << "Not enough mana! Need " << spell->getManaCost() 
                  << " mana, but have " << mana << std::endl;
        return false;
    }
    
    if (!spell->canCast(positionX, positionY, targetX, targetY)) {
        std::cout << "Cannot cast spell at that range!" << std::endl;
        return false;
    }
    
    // Метод cast теперь const, можно вызывать напрямую
    spell->cast(game, positionX, positionY, targetX, targetY);
    spendMana(spell->getManaCost());
    return true;
}

void Player::addSpellPoints(int points) {
    // Здесь можно реализовать покупку заклинаний за очки
    // Например, каждые 20 очков дают возможность выучить новое заклинание
    static int spellPointsAccumulated = 0;
    spellPointsAccumulated += points;
    
    if (spellPointsAccumulated >= 20) {
        std::cout << "You've earned " << spellPointsAccumulated 
                  << " spell points! You can learn a new spell." << std::endl;
        spellPointsAccumulated = 0;
        // Здесь можно предложить игроку выбрать заклинание
    }
}

void Player::setPosition(int x, int y) {
    positionX = x;
    positionY = y;
}

void Player::takeDamage(int damageAmount) {
    health -= damageAmount;
    if (health < 0) health = 0;
}

void Player::heal(int amount) {
    health += amount;
    if (health > maxHealth) health = maxHealth;
}

void Player::addScore(int points) {
    score += points;
}

void Player::switchCombatMode() {
    if (combatMode == CombatMode::MELEE) {
        combatMode = CombatMode::RANGED;
        damage = 8;
        attackRange = 3;
    } else if (combatMode == CombatMode::RANGED) {
        combatMode = CombatMode::CASTING;
        damage = 0; // В режиме каста урон от оружия не используется
        attackRange = 0;
    } else {
        combatMode = CombatMode::MELEE;
        damage = 15;
        attackRange = 1;
    }
}

bool Player::isAlive() const {
    return health > 0;
}

bool Player::move(int deltaX, int deltaY) {
    positionX += deltaX;
    positionY += deltaY;
    return true;
}