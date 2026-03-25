#ifndef GAMEFIELD_H
#define GAMEFIELD_H

#include "Cell.h"
#include <vector>
#include <memory>

class GameField {
private:
    int width;
    int height;
    std::vector<std::vector<Cell>> grid;

public:
    GameField(int fieldWidth, int fieldHeight);
    GameField(const GameField& other);
    GameField(GameField&& other) noexcept;
    
    GameField& operator=(const GameField& other);
    GameField& operator=(GameField&& other) noexcept;
    
    int getWidth() const;
    int getHeight() const;
    Cell& getCell(int x, int y);
    const Cell& getCell(int x, int y) const;
    
    bool isValidPosition(int x, int y) const;
    bool isCellPassable(int x, int y) const;
    
    void setCellType(int x, int y, CellType type);
    void generateImpassableCells(int count);
    
    bool placePlayer(int x, int y);
    bool removePlayer(int x, int y);
    bool placeEnemy(int x, int y);
    bool removeEnemy(int x, int y);
    bool movePlayer(int fromX, int fromY, int toX, int toY);
    bool moveEnemy(int fromX, int fromY, int toX, int toY);
    
private:
    void initializeField();
    void copyFrom(const GameField& other);
};

#endif