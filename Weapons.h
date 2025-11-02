#ifndef WEAPONS_H
#define WEAPONS_H

#include <SDL3/SDL.h>
#include <cmath>
#include <iostream>
#include "Player.h"
#include "Utility.h"

class Weapon
{
private:
    int damage;
    int bulletsCount;
    int ammoCount;
    int magazine;

    float rebound;
    float bulletSpeed;

public:
    // --- weapon behaviors ---
    void pistol(SDL_Renderer *r, Player &p);
    void shotgun(SDL_Renderer *r, Player &p); // Example: another weapon
    void drawPistol(SDL_Renderer *r, Player &p);

    // --- getters ---
    int getDamage() const { return damage; }
    int getAmmoCount() const { return ammoCount; }
    int getMagazine() const { return magazine; }
    float getRebound() const { return rebound; }
    float getBulletSpeed() const { return bulletSpeed; }
};

#endif // WEAPONS_H
