#ifndef SPELL_HAND_H
#define SPELL_HAND_H

#include "Spell.h"
#include <vector>
#include <memory>
#include <string>

class SpellHand {
private:
    std::vector<std::unique_ptr<Spell>> spells;
    int maxSpells;
    int selectedSpellIndex;
    
public:
    explicit SpellHand(int maxCapacity = 5);
    
    // Запрещаем копирование, но разрешаем перемещение
    SpellHand(const SpellHand&) = delete;
    SpellHand& operator=(const SpellHand&) = delete;
    SpellHand(SpellHand&&) = default;
    SpellHand& operator=(SpellHand&&) = default;
    
    bool addSpell(std::unique_ptr<Spell> spell);
    bool removeSpell(int index);
    bool canAddSpell() const;
    
    int getSpellCount() const;
    int getMaxSpells() const;
    int getSelectedSpellIndex() const;
    
    const Spell* getSpell(int index) const;
    const Spell* getSelectedSpell() const;
    
    void selectNextSpell();
    void selectPreviousSpell();
    void selectSpell(int index);
    
    std::unique_ptr<Spell> takeSpell(int index);
    
    void clear();
    
private:
    bool isValidIndex(int index) const;
};

#endif