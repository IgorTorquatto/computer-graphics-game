#include "coin.h"

#include "game/player.h"

#include <GL/glut.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "ecs/components/model.h"
#include "game/obstacle.h"


#define MAX_COINS 128

Coin coinPool[MAX_COINS];
static int coinCount = 0;
static float coinSpawnTimer = 0.0f;
static float coinSpawnInterval = 1.0f;

extern float worldSpeed; // declarado em main.c ou game.c para velocidade global

extern Player player;

Model coinModel;

extern int aabbCollision(float ax, float ay, float az, float aw, float ah, float ad,
                         float bx, float by, float bz, float bw, float bh, float bd);


/* Inicializa o pool de moedas */
void initCoins() {
    for(int i = 0; i < MAX_COINS; i++) {
        coinPool[i].active = 0;
    }
    coinCount = 0;
    coinSpawnTimer = 0.0f;
    coinSpawnInterval = 0.45f;
}

void spawnCoin() {
    const float exclusionMargin = 1.0f;
    for(int i = 0; i < MAX_COINS; i++) {
        if(!coinPool[i].active) {
            int tentativas = 20;
            while (tentativas--) {
                coinPool[i].lane = rand() % 3;
                coinPool[i].x = coinPool[i].lane * 2.5f;
                coinPool[i].y = 0.7f;
                coinPool[i].z = -40.0f - (rand() % 20);
                coinPool[i].w = 0.6f;
                coinPool[i].h = 0.5f;
                coinPool[i].d = 0.6f;
                coinPool[i].angle = (float)(rand() % 360);

                int ok = 1;
                Obstacle* obstacles = getObstacles();
                int maxObs = getMaxObstacles();
                float cx = coinPool[i].x;
                float cy = coinPool[i].y + coinPool[i].h * 0.5f;
                float cz = coinPool[i].z;

                for (int j = 0; j < maxObs; j++) {
                    if (!obstacles[j].active) continue;

                    if (obstacles[j].type == OBST_SINGLE || obstacles[j].type == OBST_DOUBLE) {
                        float ox = obstacles[j].x;
                        float oy = obstacles[j].y + obstacles[j].h * 0.5f;
                        float oz = obstacles[j].z;
                        float ow = obstacles[j].w + exclusionMargin;
                        float oh = obstacles[j].h;
                        float od = obstacles[j].d + exclusionMargin;

                        if (aabbCollision(
                                cx, cy, cz, coinPool[i].w, coinPool[i].h, coinPool[i].d,
                                ox, oy, oz, ow, oh, od)) {
                            ok = 0;
                            break;
                        }
                    }
                }

                if (ok) {
                    coinPool[i].active = 1;
                    break;
                }
            }
            break;
        }
    }
}

void updateCoins(float dt) {
    for(int i = 0; i < MAX_COINS; i++) {
        if(!coinPool[i].active) continue;

        coinPool[i].z += worldSpeed * dt;
        coinPool[i].angle += 180.0f * dt;

        if(coinPool[i].z > 10.0f) {
            coinPool[i].active = 0;
            continue;
        }

        float ph = 2.0f;
        float pw = 1.0f;
        float pd = 1.0f;

        float coinCenterY = coinPool[i].y + coinPool[i].h * 0.5f;

        extern int aabbCollision(float ax, float ay, float az, float aw, float ah, float ad,
                                float bx, float by, float bz, float bw, float bh, float bd);

        if (
            aabbCollision(
                coinPool[i].x, coinCenterY, coinPool[i].z, coinPool[i].w, coinPool[i].h, coinPool[i].d,
                player.x, player.y + ph*0.5f, player.z, pw, ph, pd)
           ) {
            coinPool[i].active = 0;
            coinCount++;
        }
    }

    coinSpawnTimer += dt;
    if(coinSpawnTimer >= coinSpawnInterval) {
        coinSpawnTimer = 0.0f;
        spawnCoin();
        float minInt = 0.7f;
        coinSpawnInterval = 1.0f - fminf(0.4f, worldSpeed * 0.005f);
        if(coinSpawnInterval < minInt) coinSpawnInterval = minInt;
    }
}

void drawSolidCubeWithNormals(float size) {
    float s = size * 0.5f;
    glBegin(GL_QUADS);
    // Frente
    glNormal3f(0,0,1);
    glVertex3f(-s,-s, s); glVertex3f( s,-s, s); glVertex3f( s, s, s); glVertex3f(-s, s, s);
    // Trás
    glNormal3f(0,0,-1);
    glVertex3f(-s,-s,-s); glVertex3f(-s, s,-s); glVertex3f( s, s,-s); glVertex3f( s,-s,-s);
    // Direita
    glNormal3f(1,0,0);
    glVertex3f( s,-s,-s); glVertex3f( s, s,-s); glVertex3f( s, s, s); glVertex3f( s,-s, s);
    // Esquerda
    glNormal3f(-1,0,0);
    glVertex3f(-s,-s,-s); glVertex3f(-s,-s, s); glVertex3f(-s, s, s); glVertex3f(-s, s,-s);
    // Topo
    glNormal3f(0,1,0);
    glVertex3f(-s, s,-s); glVertex3f(-s, s, s); glVertex3f( s, s, s); glVertex3f( s, s,-s);
    // Base
    glNormal3f(0,-1,0);
    glVertex3f(-s,-s,-s); glVertex3f( s,-s,-s); glVertex3f( s,-s, s); glVertex3f(-s,-s, s);
    glEnd();
}

/* Desenha as moedas em 3D na cena */
/*void drawCoins3D() {
    glColor3f(1.0f, 0.85f, 0.1f);
    for(int i = 0; i < MAX_COINS; i++) {
        if(!coinPool[i].active) continue;

        glPushMatrix();
        glTranslatef(coinPool[i].x, coinPool[i].y + coinPool[i].h * 0.5f, coinPool[i].z);
        glRotatef(coinPool[i].angle, 0.0f, 1.0f, 0.0f);
        glScalef(coinPool[i].w, coinPool[i].h, coinPool[i].d);
        drawSolidCubeWithNormals(1.0f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }
}*/

void drawCoins3D() {
    glColor3f(1.0f, 0.85f, 0.1f);
    for(int i = 0; i < MAX_COINS; i++) {
        if(!coinPool[i].active) continue;

        glPushMatrix();
        glTranslatef(coinPool[i].x, coinPool[i].y + coinPool[i].h * 0.5f + 0.1f, coinPool[i].z);
        // girar a moeda
        glRotatef(coinPool[i].angle, 0.0f, 1.0f, 0.0f);
        //  em pé
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        glScalef(coinPool[i].w, coinPool[i].h, coinPool[i].d);
        drawModel(&coinModel);
        glPopMatrix();
    }
}

/* Retorna o número atual de moedas coletadas */
int getCoinCount() {
    return coinCount;
}


void initCoinModel() {
    if(!loadOBJ("coinTraingulado.obj", &coinModel)) {
    fprintf(stderr, "Falha ao carregar modelo da moeda.\n");
    }
}

