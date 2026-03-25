#include "Enemy.h"
#include <cmath>

Enemy::Enemy(int startX, int startY, int enemyHealth, int enemyDamage)
    : health(enemyHealth), damage(enemyDamage), positionX(startX), positionY(startY), isAliveFlag(true) {}

int Enemy::getHealth() const { return health; }
int Enemy::getDamage() const { return damage; }
int Enemy::getPositionX() const { return positionX; }
int Enemy::getPositionY() const { return positionY; }
bool Enemy::isAlive() const { return isAliveFlag; }

void Enemy::setPosition(int x, int y) {
    positionX = x;
    positionY = y;
}

void Enemy::takeDamage(int damageAmount) {
    health -= damageAmount;
    if (health <= 0) {
        health = 0;
        isAliveFlag = false;
    }
}

bool Enemy::moveTowardsPlayer(int playerX, int playerY) {
    if (!isAliveFlag) return false;
    
    int deltaX = playerX - positionX;
    int deltaY = playerY - positionY;
    
    if (std::abs(deltaX) > std::abs(deltaY)) {
        positionX += (deltaX > 0) ? 1 : -1;
    } else {
        positionY += (deltaY > 0) ? 1 : -1;
    }
    
    return true;
}