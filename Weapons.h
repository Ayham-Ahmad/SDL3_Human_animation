#ifndef WEAPONS_H
#define WEAPONS_H

#include <SDL3/SDL.h>
#include "Player.h"

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
    Weapon();

    void pistol(SDL_Renderer *r, Player &p);
    void drawPistol(SDL_Renderer *r, Player &p);
};

#endif // WEAPONS_H
