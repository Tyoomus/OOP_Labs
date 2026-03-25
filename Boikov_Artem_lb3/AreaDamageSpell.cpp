#include "AreaDamageSpell.h"
#include "GameManager.h"
#include <cmath>
#include <iostream>

AreaDamageSpell::AreaDamageSpell(const std::string& spellName, 
                                const std::string& spellDescription,
                                int cost, int damage, int range, int radius)
    : name(spellName), description(spellDescription), 
      manaCost(cost), damageAmount(damage), castRange(range), areaRadius(radius) {}

std::string AreaDamageSpell::getName() const { return name; }
std::string AreaDamageSpell::getDescription() const { return description; }
int AreaDamageSpell::getManaCost() const { return manaCost; }
int AreaDamageSpell::getCastRange() const { return castRange; }
int AreaDamageSpell::getDamageAmount() const { return damageAmount; }
int AreaDamageSpell::getAreaRadius() const { return areaRadius; }

bool AreaDamageSpell::canCast(int casterX, int casterY, int targetX, int targetY) const {
    int distance = std::abs(targetX - casterX) + std::abs(targetY - casterY);
    return distance <= castRange;
}

void AreaDamageSpell::cast(GameManager& game, int casterX, int casterY, 
                         int targetX, int targetY) const {
    game.castAreaDamageSpell(*this, targetX, targetY);
}

std::unique_ptr<Spell> AreaDamageSpell::clone() const {
    return std::make_unique<AreaDamageSpell>(*this);
}