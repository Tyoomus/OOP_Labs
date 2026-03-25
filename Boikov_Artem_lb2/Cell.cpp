#include "Cell.h"

Cell::Cell() 
    : type(CellType::EMPTY), hasPlayer(false), hasEnemy(false) {}

CellType Cell::getType() const { return type; }
bool Cell::getHasPlayer() const { return hasPlayer; }
bool Cell::getHasEnemy() const { return hasEnemy; }

bool Cell::isPassable() const {
    return type != CellType::IMPASSABLE;
}

void Cell::setType(CellType cellType) { type = cellType; }
void Cell::setHasPlayer(bool has) { hasPlayer = has; }
void Cell::setHasEnemy(bool has) { hasEnemy = has; }

bool Cell::isEmpty() const {
    return !hasPlayer && !hasEnemy;
}