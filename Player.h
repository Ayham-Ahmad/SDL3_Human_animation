#ifndef PLAYER_H
#define PLAYER_H

#include <SDL3/SDL.h>

// Player and drawing declarations (keeps human drawing helpers)

class Player {
public:
    float screenWidth;
    float screenHeight;

    float X, Y;
    float H, W;
    float top, bottom, right, left;

    float gravity;

    Player(float screenW, float screenH, float gravityValue = 0.5f, float size = 200.0f);

    void updateEdges();
    void setPosition(float x, float y);
};

// collision / thrown direction classes
class Collide {
public:
    bool collideBottom = false;
    bool collideTop = false;
    bool collideRight = false;
    bool collideLeft = false;

    void reset();
};

class ThrownDir {
public:
    bool right = false;
    bool left = false;
};

// Drawing / human helpers
void SDL_RenderCircle(SDL_Renderer *renderer, float cx, float cy, float radius);
void head(SDL_Renderer *r, float x, float y);
void nick(SDL_Renderer *r, float x, float y1, float y2);
void body(SDL_Renderer *renderer, float x1, float y1, float x2, float y2);
void limb(SDL_Renderer *r, float x, float y, float angle, float length, bool rightSide);

void stand(Player &p, float &HY, float &NSP, float &NEP, float &HLL,
           float &RHAngle, float &LHAngle, float &RLAngle, float &LLAngle, float &BEP);

void getHuman(SDL_Renderer *r, const Player &p, float HY, float NSP, float NEP, float HLL,
              float RHAngle, float LHAngle, float BEP, float RLAngle, float LLAngle);

#endif // PLAYER_H
