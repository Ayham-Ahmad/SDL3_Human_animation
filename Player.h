#ifndef PLAYER_H
#define PLAYER_H

#include <SDL3/SDL.h>
#include <iostream>

#define M_PI 3.14159265358979323846

using namespace std;

class Player
{
public:
    float screenWidth;
    float screenHeight;

    float x, y;
    float h, w;
    float top, bottom, right, left;

    float gravity;

    float HY;  // Head Y
    float NSP; // Neck start point
    float NEP; // Neck end point
    float HL;  // Hand length
    float RAX; // Right Arm X
    float RAY; // Right Arm Y
    float LAX; // Left Arm X
    float LAY; // Left Arm Y
    float BEP; // Body end point
    float LA;  // Legs angle
    float LL;  // Legs Length

    Player(float screenW, float screenH, float gravityValue = 0.5f, float size = 200.0f);

    void updateEdges();
    void setPosition(float x, float y);
};

class Collide
{
public:
    bool collideBottom = false;
    bool collideTop = false;
    bool collideRight = false;
    bool collideLeft = false;

    void reset();
};

class ThrownDir
{
public:
    bool right = false;
    bool left = false;
};

void SDL_RenderCircle(SDL_Renderer *renderer, float cx, float cy, float radius);
void head(SDL_Renderer *r, float x, float y);
void nick(SDL_Renderer *r, float x, float y1, float y2);
void body(SDL_Renderer *renderer, float x1, float y1, float x2, float y2);
void limb(SDL_Renderer *r, float x, float y, float angle, float length, bool rightSide);

void stand(Player &p, float &mx, float &my);

void getHuman(SDL_Renderer *r, const Player &p);

void updateArmAngles(float &mx, float &my, Player &p);

#endif // PLAYER_H
