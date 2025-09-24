
#ifndef TREE_H
#define TREE_H

typedef struct {
    int active;
    float x, y, z;
    float escala;
} Tree;

#define MAX_TREES 150

void initTrees();
void updateTrees(float dt, float worldSpeed);
void drawTrees();

#endif
