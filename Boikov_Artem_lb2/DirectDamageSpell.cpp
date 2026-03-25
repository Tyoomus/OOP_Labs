#include "DirectDamageSpell.h"
#include "GameManager.h"
#include <cmath>
#include <iostream>

DirectDamageSpell::DirectDamageSpell(const std::string& spellName, 
                                    const std::string& spellDescription,
                                    int cost, int damage, int range)
    : name(spellName), description(spellDescription), 
      manaCost(cost), damageAmount(damage), castRange(range) {}

std::string DirectDamageSpell::getName() const { return name; }
std::string DirectDamageSpell::getDescription() const { return description; }
int DirectDamageSpell::getManaCost() const { return manaCost; }
int DirectDamageSpell::getCastRange() const { return castRange; }
int DirectDamageSpell::getDamageAmount() const { return damageAmount; }

bool DirectDamageSpell::canCast(int casterX, int casterY, int targetX, int targetY) const {
    int distance = std::abs(targetX - casterX) + std::abs(targetY - casterY);
    return distance <= castRange;
}

void DirectDamageSpell::cast(GameManager& game, int casterX, int casterY, 
                           int targetX, int targetY) const {
    game.castDirectDamageSpell(*this, targetX, targetY);
}

std::unique_ptr<Spell> DirectDamageSpell::clone() const {
    return std::make_unique<DirectDamageSpell>(*this);
}