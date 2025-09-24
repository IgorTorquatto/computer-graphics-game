#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "model.h"

#define MAX_OBSTACLES 64
#define LANE_WIDTH 2.5f
#define TARGET_HEIGHT_LOG 1.0f
#define TARGET_HEIGHT_ROCK 1.0f

typedef enum {
    OBST_SINGLE,
    OBST_DOUBLE
} ObstacleType;

typedef struct {
    int active;
    ObstacleType type;
    int lane;
    float x, y, z;
    float w, h, d;
    Model *model;
} Obstacle;

void initObstacles();
void updateObstacles(float dt);
void obstacleUpdate(float dt);
void drawObstacles();
void spawnObstacle();
Obstacle* getObstacles();
int getMaxObstacles();
void initObstacleModels();

extern Model rockModel;
extern Model logModel;

#endif
