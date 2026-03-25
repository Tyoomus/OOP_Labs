#ifndef CELL_H
#define CELL_H

enum class CellType {
    EMPTY,
    IMPASSABLE
};

class Cell {
private:
    CellType type;
    bool hasPlayer;
    bool hasEnemy;

public:
    Cell();
    
    CellType getType() const;
    bool getHasPlayer() const;
    bool getHasEnemy() const;
    bool isPassable() const;
    
    void setType(CellType cellType);
    void setHasPlayer(bool has);
    void setHasEnemy(bool has);
    
    bool isEmpty() const;
};

#endif