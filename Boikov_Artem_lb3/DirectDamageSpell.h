#ifndef DIRECT_DAMAGE_SPELL_H
#define DIRECT_DAMAGE_SPELL_H

#include "Spell.h"
#include <string>

// Предварительное объявление вместо #include "GameManager.h"
class GameManager;

class DirectDamageSpell : public Spell {
private:
    std::string name;
    std::string description;
    int manaCost;
    int damageAmount;
    int castRange;
    
public:
    DirectDamageSpell(const std::string& spellName, const std::string& spellDescription,
                     int cost, int damage, int range);
    
    std::string getName() const override;
    std::string getDescription() const override;
    int getManaCost() const override;
    int getCastRange() const override;
    bool canCast(int casterX, int casterY, int targetX, int targetY) const override;
    void cast(GameManager& game, int casterX, int casterY, int targetX, int targetY) const override;
    std::unique_ptr<Spell> clone() const override;
    
    int getDamageAmount() const;
};

#endif