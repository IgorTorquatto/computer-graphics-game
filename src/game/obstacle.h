#pragma once

#include "ecs/components/model.h"

#define MAX_OBSTACLES 64
#define LANE_WIDTH 2.5f
#define TARGET_HEIGHT_LOG 1.0f
#define TARGET_HEIGHT_ROCK 1.0f

#define MODEL_PATH_ROCK "models/rock/rock.obj"
#define MODEL_PATH_LOGS "models/logs/logs.obj"

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

void init_logs_model();
void init_rock_model();
void initObstacles();

void updateObstacles(float dt);
void obstacleUpdate(float dt);
void drawObstacles();
void spawnObstacle();

Obstacle* getObstacles();
int getMaxObstacles();

void free_rock_model();
void free_logs_model();
