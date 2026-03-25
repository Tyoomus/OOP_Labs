#ifndef SPELL_H
#define SPELL_H

#include <string>
#include <memory>

// Предварительное объявление GameManager
class GameManager;

// Базовый интерфейс заклинания
class Spell {
public:
    virtual ~Spell() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual int getManaCost() const = 0;
    virtual int getCastRange() const = 0;
    
    virtual bool canCast(int casterX, int casterY, int targetX, int targetY) const = 0;
    
    // Метод const
    virtual void cast(GameManager& game, int casterX, int casterY, int targetX, int targetY) const = 0;
    
    // Создание копии заклинания (для полиморфного копирования)
    virtual std::unique_ptr<Spell> clone() const = 0;
};

#endif