#pragma once

typedef struct {
    int active;
    int lane;
    float x, y, z;
    float w, h, d;
    float angle;
} Coin;

#define MAX_COINS 128

extern Coin coinPool[MAX_COINS];

void initCoins();
void spawnCoin();
void updateCoins(float dt);
void drawCoins3D();
int getCoinCount();
