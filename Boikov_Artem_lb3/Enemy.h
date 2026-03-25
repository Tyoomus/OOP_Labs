#ifndef ENEMY_H
#define ENEMY_H

class Enemy {
private:
    int health;
    int damage;
    int positionX;
    int positionY;
    bool isAliveFlag;

public:
    Enemy(int startX, int startY, int enemyHealth, int enemyDamage);
    
    int getHealth() const;
    int getDamage() const;
    int getPositionX() const;
    int getPositionY() const;
    bool isAlive() const;
    
    void setPosition(int x, int y);
    
    void takeDamage(int damageAmount);
    bool moveTowardsPlayer(int playerX, int playerY);
};

#endif