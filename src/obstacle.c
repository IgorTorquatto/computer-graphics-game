#include "obstacle.h"
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include "model.h"

#define LANE_WIDTH 2.5f
#define TARGET_HEIGHT_LOG 1.0f
#define TARGET_HEIGHT_ROCK 1.0f

static Obstacle obstacles[MAX_OBSTACLES];
extern float worldSpeed;  // Variável global do jogo para velocidade do mundo
static float spawnTimer = 0.0f;
static float spawnInterval = 1.0f;

static float escalaLog = 1.0f;
static float escalaRock = 1.0f;

Model rockModel;
Model logModel;

static float laneToX(int lane) {
    return lane * LANE_WIDTH;
}

void initObstacles() {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        obstacles[i].active = 0;
    }
    spawnTimer = 0.0f;
    spawnInterval = 1.0f;

    float alturaLog = logModel.maxY - logModel.minY;
    if (alturaLog > 0.0f) {
        escalaLog = TARGET_HEIGHT_LOG / alturaLog;
    }

    float alturaRock = rockModel.maxY - rockModel.minY;
    if (alturaRock > 0.0f) {
        escalaRock = TARGET_HEIGHT_ROCK / alturaRock;
    }
}

void updateObstacles(float dt) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (!obstacles[i].active) continue;

        obstacles[i].z += worldSpeed * dt;
        if (obstacles[i].z > 10.0f) {
            obstacles[i].active = 0;
        }
    }
}

void drawObstacles() {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (!obstacles[i].active) continue;

        glPushMatrix();
        float drawX = obstacles[i].x;
        float drawY = obstacles[i].y + obstacles[i].h * 0.5f;
        float drawZ = obstacles[i].z;
        glTranslatef(drawX, drawY, drawZ);

        if (obstacles[i].type == OBST_SINGLE) {
            glScalef(escalaRock, escalaRock, escalaRock);
        } else {
            glScalef(escalaLog, escalaLog, escalaLog);
        }

        glColor3f(0.8f, 0.7f, 0.5f);  // Cor básica opcional
        drawModel(obstacles[i].model);

        glPopMatrix();
    }
}

void spawnObstacle() {
    int lanes[3] = {0, 0, 0};

    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (!obstacles[i].active) continue;
        if (obstacles[i].z < -58.0f) {
            if (obstacles[i].type == OBST_SINGLE) {
                lanes[obstacles[i].lane] = 1;
            } else if (obstacles[i].type == OBST_DOUBLE) {
                lanes[obstacles[i].lane] = 1;
                lanes[obstacles[i].lane + 1] = 1;
            }
        }
    }

    int ocupadas = lanes[0] + lanes[1] + lanes[2];
    if (ocupadas >= 2) {
        return;
    }

    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (!obstacles[i].active) {
            obstacles[i].active = 1;

            int tipo = rand() % 2;
            if (tipo == 0) {  // Single
                int livres[3], nlivres = 0;
                for (int j = 0; j < 3; j++)
                    if (!lanes[j]) livres[nlivres++] = j;
                if (nlivres == 0)
                    return;

                obstacles[i].type = OBST_SINGLE;
                obstacles[i].lane = livres[rand() % nlivres];
                obstacles[i].x = laneToX(obstacles[i].lane);
                obstacles[i].w = 1.0f;
                obstacles[i].model = &rockModel;
            } else {  // Double
                int duplas[2], nduplas = 0;
                if (!lanes[0] && !lanes[1]) duplas[nduplas++] = 0;
                if (!lanes[1] && !lanes[2]) duplas[nduplas++] = 1;
                if (nduplas == 0)
                    return;

                obstacles[i].type = OBST_DOUBLE;
                obstacles[i].lane = duplas[rand() % nduplas];
                obstacles[i].x = laneToX(obstacles[i].lane) + LANE_WIDTH * 0.5f;
                obstacles[i].w = 2.0f;
                obstacles[i].model = &logModel;
            }

            obstacles[i].y = 0.0f;
            obstacles[i].h = 2.0f + (float)(rand() % 20) * 0.05f;
            obstacles[i].d = 1.0f;
            obstacles[i].z = -60.0f - (rand() % 40);
            return;
        }
    }
}

Obstacle* getObstacles() {
    return obstacles;
}

int getMaxObstacles() {
    return MAX_OBSTACLES;
}

void obstacleUpdate(float dt) {
    spawnTimer += dt;
    updateObstacles(dt);

    if (spawnTimer >= spawnInterval) {
        spawnTimer = 0.0f;
        spawnObstacle();
        spawnInterval = 0.8f - fmin(0.5f, worldSpeed * 0.01f);
        if (spawnInterval < 0.25f)
            spawnInterval = 0.25f;
    }
}

void initObstacleModels() {
    if (!loadOBJ("rock.obj", &rockModel)) {
        fprintf(stderr, "Erro ao carregar rock.obj\n");
    }

    if (!loadOBJ("log.obj", &logModel)) {
        fprintf(stderr, "Erro ao carregar log.obj\n");
    }

    float alturaLog = logModel.maxY - logModel.minY;
    if (alturaLog > 0.0f) {
        escalaLog = TARGET_HEIGHT_LOG / alturaLog;
    }

    float alturaRock = rockModel.maxY - rockModel.minY;
    if (alturaRock > 0.0f) {
        escalaRock = TARGET_HEIGHT_ROCK / alturaRock;
    }
}

