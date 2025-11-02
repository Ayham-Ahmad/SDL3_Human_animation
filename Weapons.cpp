#include "Weapons.h"

void Weapon::pistol(SDL_Renderer *r, Player &p)
{
    damage = 5;
    ammoCount = 8;
    magazine = 50;
    rebound = 0.0f;
    bulletSpeed = 1.0f;

    drawPistol(r, p);
}

void Weapon::shotgun(SDL_Renderer *r, Player &p)
{
    damage = 10;
    ammoCount = 4;
    magazine = 20;
    rebound = 0.2f;
    bulletSpeed = 0.8f;

    drawPistol(r, p); // or drawShotgun(r, p)
}

void Weapon::drawPistol(SDL_Renderer *r, Player &p)
{
    SDL_SetRenderDrawColor(r, 128, 128, 128, 255);
    drawRotatedRect(r, p.rightArmXPosition, p.rightArmYPosition, 50, 20, 0);
    drawRotatedRect(r, p.rightArmXPosition, p.rightArmYPosition + 20, 10, 15, 0);
}
