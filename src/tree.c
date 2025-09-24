
#include <stdlib.h>
#include <GL/glut.h>
#include "tree.h"
#include "model.h"  // Para drawModel e carregar OBJ

extern Model treeModel;  // Modelo da árvore carregado externamente
extern float escalaArvoreDefault;

static Tree treesPool[MAX_TREES];

void initTrees() {
    for(int i=0; i<MAX_TREES; i++) {
        treesPool[i].active = 0;
    }
}

void updateTrees(float dt, float worldSpeed) {
    static float spawnTimer = 0.0f;
    spawnTimer += dt;

    if(spawnTimer > 0.5f) {  // Spawna a cada 0.5 segundos
        spawnTimer = 0.0f;
        for(int i=0; i<MAX_TREES; i++) {
            if(!treesPool[i].active) {
                treesPool[i].active = 1;

                treesPool[i].x = (rand() % 2 == 0) ? -3.0f : 8.0f; // esquerda/direita
                treesPool[i].y = 0.0f;
                treesPool[i].z = -60.0f - (rand() % 20);

                // Escala aleatória maior
                treesPool[i].escala = 2.5f + ((float)(rand() % 100) / 100.0f) * 2.0f;

                break;
            }
        }
    }

    for(int i=0; i<MAX_TREES; i++) {
        if(treesPool[i].active) {
            treesPool[i].z += worldSpeed * dt;
            if(treesPool[i].z > 10.0f) {
                treesPool[i].active = 0;
            }
        }
    }
}


void drawTrees() {
    glColor3f(0.0f, 0.6f, 0.0f); // Verde para árvore (pode mudar para cor da textura)

    for(int i=0; i<MAX_TREES; i++) {
        if(!treesPool[i].active) continue;

        glPushMatrix();
        glTranslatef(treesPool[i].x, treesPool[i].y, treesPool[i].z);
        // glRotatef(angulo, eixoX, eixoY, eixoZ);

        glScalef(treesPool[i].escala, treesPool[i].escala, treesPool[i].escala);
        drawModel(&treeModel);
        glPopMatrix();
    }
}
