#include "SpellHand.h"
#include <algorithm>
#include <iostream>

SpellHand::SpellHand(int maxCapacity) 
    : maxSpells(maxCapacity), selectedSpellIndex(-1) {}

bool SpellHand::addSpell(std::unique_ptr<Spell> spell) {
    if (spells.size() >= maxSpells) {
        std::cout << "Spell hand is full! Max capacity: " << maxSpells << std::endl;
        return false;
    }
    
    spells.push_back(std::move(spell));
    
    if (selectedSpellIndex == -1 && !spells.empty()) {
        selectedSpellIndex = 0;
    }
    
    return true;
}

bool SpellHand::removeSpell(int index) {
    if (!isValidIndex(index)) {
        return false;
    }
    
    spells.erase(spells.begin() + index);
    
    if (spells.empty()) {
        selectedSpellIndex = -1;
    } else if (selectedSpellIndex >= spells.size()) {
        selectedSpellIndex = spells.size() - 1;
    }
    
    return true;
}

bool SpellHand::canAddSpell() const {
    return spells.size() < maxSpells;
}

int SpellHand::getSpellCount() const {
    return spells.size();
}

int SpellHand::getMaxSpells() const {
    return maxSpells;
}

int SpellHand::getSelectedSpellIndex() const {
    return selectedSpellIndex;
}

const Spell* SpellHand::getSpell(int index) const {
    if (!isValidIndex(index)) {
        return nullptr;
    }
    return spells[index].get();
}

const Spell* SpellHand::getSelectedSpell() const {
    return getSpell(selectedSpellIndex);
}

void SpellHand::selectNextSpell() {
    if (spells.empty()) {
        selectedSpellIndex = -1;
        return;
    }
    
    selectedSpellIndex = (selectedSpellIndex + 1) % spells.size();
}

void SpellHand::selectPreviousSpell() {
    if (spells.empty()) {
        selectedSpellIndex = -1;
        return;
    }
    
    selectedSpellIndex = (selectedSpellIndex - 1 + spells.size()) % spells.size();
}

void SpellHand::selectSpell(int index) {
    if (isValidIndex(index)) {
        selectedSpellIndex = index;
    }
}

std::unique_ptr<Spell> SpellHand::takeSpell(int index) {
    if (!isValidIndex(index)) {
        return nullptr;
    }
    
    auto spell = std::move(spells[index]);
    spells.erase(spells.begin() + index);
    
    if (spells.empty()) {
        selectedSpellIndex = -1;
    } else if (selectedSpellIndex >= spells.size()) {
        selectedSpellIndex = spells.size() - 1;
    }
    
    return spell;
}

void SpellHand::clear() {
    spells.clear();
    selectedSpellIndex = -1;
}

bool SpellHand::isValidIndex(int index) const {
    return index >= 0 && index < spells.size();
}