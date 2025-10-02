#pragma once

typedef struct {
    int active;
    int lane;
    float x, y, z;
    float w, h, d;
    float angle;
} Coin;

#define MODEL_PATH_COIN "models/coin/coin.obj"

#define MAX_COINS 128

extern Coin coinPool[MAX_COINS];

/* Inicializa o pool de moedas */
void initCoins();

/* Spawn de uma nova moeda em faixa randomizada */
void spawnCoin();

/* Atualiza posição e rotação das moedas, e verifica colisões */
void updateCoins(float dt);

/* Desenha as moedas em 3D na cena */
void drawCoins3D();

/* Retorna o número atual de moedas coletadas */
int getCoinCount();

// WATCH -> Está sendo usado?
void drawSolidCubeWithNormals(float size);

void initCoinModel();
void free_coin_model();
