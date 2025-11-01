#include "Weapons.h"
#include "Utility.h"
#include <cmath>
#include <iostream>

Weapon::Weapon()
{
    damage = 0;
    bulletsCount = 0;
    ammoCount = 0;
    magazine = 0;
    rebound = 0.0f;
    bulletSpeed = 0.0f;
}

void Weapon::pistol(SDL_Renderer *r, Player &p)
{
    damage = 5;
    ammoCount = 8;
    magazine = 50;

    rebound = 0.0f;
    bulletSpeed = 1.0f;

    drawPistol(r, p);
}

void Weapon::drawPistol(SDL_Renderer *r, Player &p)
{
    SDL_SetRenderDrawColor(r, 128, 128, 128, 255);

    // pistol body
    drawRotatedRect(r, p.RAX, p.RAY, 50, 20, 0);

    // pistol handle
    drawRotatedRect(r, p.RAX, p.RAY + 20, 10, 15, 0);
}
