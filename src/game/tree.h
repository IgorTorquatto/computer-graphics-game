#pragma once

#include "ecs/components/model.h"

typedef struct {
    int active;
    float x, y, z;
    float escala;
} Tree;

typedef struct {
    int active;
    float x, y, z;
    float escala;
} Bush;

#define MAX_TREES 150
#define MAX_BUSHES 250

void initTrees();
void updateTrees(float dt, float worldSpeed);
void drawTrees();

void initBushes();
void updateBushes(float dt, float worldSpeed);
void drawBushes();
