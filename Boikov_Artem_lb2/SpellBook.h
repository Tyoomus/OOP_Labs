#ifndef SPELLBOOK_H
#define SPELLBOOK_H

#include <vector>
#include <string>
#include <memory>

class Spell;

class SpellBook {
public:
    static std::unique_ptr<Spell> createRandomSpell();
    static std::vector<std::unique_ptr<Spell>> getAllAvailableSpells();
    static std::unique_ptr<Spell> createSpellByName(const std::string& spellName);
    
private:
    SpellBook() = delete;
};

#endif
