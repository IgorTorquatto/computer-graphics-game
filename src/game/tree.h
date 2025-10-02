#pragma once

#include "ecs/components/model.h"

#define MODEL_PATH_TREE "models/tree/Tree.obj"
#define MODEL_PATH_BUSH "models/bush/Bush.obj"

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
void updateTrees(float dt, float world_speed);
void drawTrees();

void initBushes();
void updateBushes(float dt, float world_speed);
void drawBushes();

void free_bush_model();
void free_tree_model();
