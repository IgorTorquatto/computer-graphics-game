#include "coin.h"
#include "player.h"
#include <GL/glut.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>


#define MAX_COINS 128

Coin coinPool[MAX_COINS];
static int coinCount = 0;
static float coinSpawnTimer = 0.0f;
static float coinSpawnInterval = 1.0f;
extern float worldSpeed; // declarado em main.c ou game.c para velocidade global
extern Player player;

/* Inicializa o pool de moedas */
void initCoins() {
    for(int i = 0; i < MAX_COINS; i++) {
        coinPool[i].active = 0;
    }
    coinCount = 0;
    coinSpawnTimer = 0.0f;
    coinSpawnInterval = 0.45f;
}

/* Spawn de uma nova moeda em faixa randomizada */
void spawnCoin() {
    for(int i = 0; i < MAX_COINS; i++) {
        if(!coinPool[i].active) {
            coinPool[i].active = 1;
            coinPool[i].lane = rand() % 3;
            coinPool[i].x = coinPool[i].lane * 2.5f;
            coinPool[i].y = 0.0f;
            coinPool[i].z = -40.0f - (rand() % 20);
            coinPool[i].w = 0.6f;
            coinPool[i].h = 0.6f;
            coinPool[i].d = 0.2f;
            coinPool[i].angle = (float)(rand() % 360);
            break;
        }
    }
}

/* Atualiza posição e rotação das moedas, e verifica colisões */
void updateCoins(float dt) {
    for(int i = 0; i < MAX_COINS; i++) {
        if(!coinPool[i].active) continue;

        coinPool[i].z += worldSpeed * dt;
        coinPool[i].angle += 180.0f * dt;

        if(coinPool[i].z > 10.0f) {
            coinPool[i].active = 0;
            continue;
        }

        float ph = 2.0f; // altura player padrão (ou obtenha via player.height)
        float pw = 1.0f;
        float pd = 1.0f;

        float coinCenterY = coinPool[i].y + coinPool[i].h * 0.5f;

        /* Colisão AABB simples, utilize a função aabbCollision do jogo principal */
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

    /* Controle de tempo para spawn */
    coinSpawnTimer += dt;
    if(coinSpawnTimer >= coinSpawnInterval) {
        coinSpawnTimer = 0.0f;
        spawnCoin();
        /* Intervalo diminui conforme velocidade do jogo */
        float minInt = 0.7f;
        coinSpawnInterval = 1.0f - fminf(0.4f, worldSpeed * 0.005f);
        if(coinSpawnInterval < minInt) coinSpawnInterval = minInt;
    }
}

/* Desenha as moedas em 3D na cena */
void drawCoins3D() {
    glColor3f(1.0f, 0.85f, 0.1f);
    for(int i = 0; i < MAX_COINS; i++) {
        if(!coinPool[i].active) continue;

        glPushMatrix();
        glTranslatef(coinPool[i].x, coinPool[i].y + coinPool[i].h * 0.5f, coinPool[i].z);
        glRotatef(coinPool[i].angle, 0.0f, 1.0f, 0.0f);
        glScalef(coinPool[i].w, coinPool[i].h, coinPool[i].d);
        glutSolidCube(1.0f);
        glPopMatrix();
    }
}


/* Retorna o número atual de moedas coletadas */
int getCoinCount() {
    return coinCount;
}
