#include "SpellBook.h"
#include "DirectDamageSpell.h"
#include "AreaDamageSpell.h"
#include <random>
#include <memory>

std::unique_ptr<Spell> SpellBook::createRandomSpell() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dist(0, 3);
    
    int spellType = dist(gen);
    
    switch (spellType) {
        case 0:
            return std::make_unique<DirectDamageSpell>(
                "Fire Bolt", "Shoots a bolt of fire at a single target",
                10, 25, 4);
        case 1:
            return std::make_unique<DirectDamageSpell>(
                "Ice Lance", "Pierces enemy with sharp ice",
                15, 35, 3);
        case 2:
            return std::make_unique<AreaDamageSpell>(
                "Fireball", "Explodes in a small area, damaging all within",
                25, 20, 4, 1);
        case 3:
            return std::make_unique<AreaDamageSpell>(
                "Chain Lightning", "Lightning that jumps between nearby enemies",
                30, 15, 5, 2);
        default:
            return std::make_unique<DirectDamageSpell>(
                "Magic Missile", "Simple magical projectile",
                5, 15, 3);
    }
}

std::vector<std::unique_ptr<Spell>> SpellBook::getAllAvailableSpells() {
    std::vector<std::unique_ptr<Spell>> spells;
    
    spells.push_back(std::make_unique<DirectDamageSpell>(
        "Fire Bolt", "Shoots a bolt of fire at a single target",
        10, 25, 4));
    
    spells.push_back(std::make_unique<DirectDamageSpell>(
        "Ice Lance", "Pierces enemy with sharp ice",
        15, 35, 3));
    
    spells.push_back(std::make_unique<DirectDamageSpell>(
        "Magic Missile", "Simple magical projectile",
        5, 15, 3));
    
    spells.push_back(std::make_unique<AreaDamageSpell>(
        "Fireball", "Explodes in a small area, damaging all within",
        25, 20, 4, 1));
    
    spells.push_back(std::make_unique<AreaDamageSpell>(
        "Chain Lightning", "Lightning that jumps between nearby enemies",
        30, 15, 5, 2));
    
    spells.push_back(std::make_unique<AreaDamageSpell>(
        "Meteor Shower", "Calls down meteors on a large area",
        50, 40, 6, 2));
    
    return spells;
}

std::unique_ptr<Spell> SpellBook::createSpellByName(const std::string& spellName) {
    auto allSpells = getAllAvailableSpells();
    
    for (auto& spell : allSpells) {
        if (spell->getName() == spellName) {
            return std::move(spell);
        }
    }
    
    return nullptr;
}