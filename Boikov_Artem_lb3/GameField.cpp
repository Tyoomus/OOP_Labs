#include "GameField.h"
#include <stdexcept>
#include <random>

GameField::GameField(int fieldWidth, int fieldHeight) 
    : width(fieldWidth), height(fieldHeight) {
    if (width < 10 || height < 10 || width > 25 || height > 25) {
        throw std::invalid_argument("Field size must be between 10x10 and 25x25");
    }
    initializeField();
}

GameField::GameField(const GameField& other) 
    : width(other.width), height(other.height) {
    copyFrom(other);
}

GameField::GameField(GameField&& other) noexcept
    : width(other.width), height(other.height), grid(std::move(other.grid)) {
    other.width = 0;
    other.height = 0;
}

GameField& GameField::operator=(const GameField& other) {
    if (this != &other) {
        width = other.width;
        height = other.height;
        copyFrom(other);
    }
    return *this;
}

GameField& GameField::operator=(GameField&& other) noexcept {
    if (this != &other) {
        width = other.width;
        height = other.height;
        grid = std::move(other.grid);
        other.width = 0;
        other.height = 0;
    }
    return *this;
}

void GameField::initializeField() {
    grid.resize(height, std::vector<Cell>(width));
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            grid[y][x] = Cell();
        }
    }
}

void GameField::copyFrom(const GameField& other) {
    grid = other.grid;
}

int GameField::getWidth() const { return width; }
int GameField::getHeight() const { return height; }

Cell& GameField::getCell(int x, int y) {
    if (!isValidPosition(x, y)) {
        throw std::out_of_range("Cell position out of bounds");
    }
    return grid[y][x];
}

const Cell& GameField::getCell(int x, int y) const {
    if (!isValidPosition(x, y)) {
        throw std::out_of_range("Cell position out of bounds");
    }
    return grid[y][x];
}

bool GameField::isValidPosition(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

bool GameField::isCellPassable(int x, int y) const {
    return isValidPosition(x, y) && getCell(x, y).isPassable() && getCell(x, y).isEmpty();
}

void GameField::setCellType(int x, int y, CellType type) {
    if (isValidPosition(x, y)) {
        getCell(x, y).setType(type);
    }
}

void GameField::generateImpassableCells(int count) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distX(0, width - 1);
    std::uniform_int_distribution<> distY(0, height - 1);
    
    int placed = 0;
    while (placed < count) {
        int x = distX(gen);
        int y = distY(gen);
        
        if (getCell(x, y).getType() == CellType::EMPTY && getCell(x, y).isEmpty()) {
            setCellType(x, y, CellType::IMPASSABLE);
            placed++;
        }
    }
}

bool GameField::placePlayer(int x, int y) {
    if (!isValidPosition(x, y) || !isCellPassable(x, y)) {
        return false;
    }
    getCell(x, y).setHasPlayer(true);
    return true;
}

bool GameField::removePlayer(int x, int y) {
    if (!isValidPosition(x, y)) return false;
    getCell(x, y).setHasPlayer(false);
    return true;
}

bool GameField::placeEnemy(int x, int y) {
    if (!isValidPosition(x, y) || !isCellPassable(x, y)) {
        return false;
    }
    getCell(x, y).setHasEnemy(true);
    return true;
}

bool GameField::removeEnemy(int x, int y) {
    if (!isValidPosition(x, y)) return false;
    getCell(x, y).setHasEnemy(false);
    return true;
}

bool GameField::movePlayer(int fromX, int fromY, int toX, int toY) {
    if (!isValidPosition(toX, toY) || !isCellPassable(toX, toY)) {
        return false;
    }
    
    removePlayer(fromX, fromY);
    return placePlayer(toX, toY);
}

bool GameField::moveEnemy(int fromX, int fromY, int toX, int toY) {
    if (!isValidPosition(toX, toY) || !isCellPassable(toX, toY)) {
        return false;
    }
    
    removeEnemy(fromX, fromY);
    return placeEnemy(toX, toY);
}