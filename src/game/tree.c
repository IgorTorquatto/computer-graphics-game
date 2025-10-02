#pragma region Libs
    #include <GL/glut.h>
#pragma endregion
#pragma region Local Includes
    #include "utils/basics.h" // math, bool, stdlib
    #include "utils/print.h"
    #pragma region Dependencies
        #include "ecs/components/model.h"
    #pragma endregion
#pragma endregion
#include "tree.h"


#pragma region Globals
    extern float escalaArvoreDefault;
    extern float escalaBushDefault;
#pragma endregion
#pragma region Locals
    enum { TREE = 1, BUSH = 2 };
    static unsigned model_not_loaded = TREE | BUSH;
    static Model treeModel;
    static Model bushModel;
    static Tree treesPool[MAX_TREES];
    static Bush bushesPool[MAX_BUSHES];
#pragma endregion


void initTrees() {
    for (int i = 0; i < MAX_TREES; i++)
        treesPool[i].active = 0;

    if (load_obj(MODEL_PATH_TREE, &treeModel)) {
        float alturaTree = treeModel.max.y - treeModel.min.y;
        if (alturaTree > 0.1f)
            escalaArvoreDefault = 2.0f / alturaTree;
        print_success("Modelo " MODEL_PATH_TREE " carregado com sucesso!");
        model_not_loaded &= ~TREE; // mark as loaded
    } else
        print_error("Falha ao carregar modelo " MODEL_PATH_TREE "!");
}

void initBushes() {
    for(int i=0; i<MAX_BUSHES; i++)
        bushesPool[i].active = 0;

    if(load_obj(MODEL_PATH_BUSH, &bushModel)) {
        print_success("Modelo " MODEL_PATH_BUSH " carregado com sucesso!");
        model_not_loaded &= ~BUSH; // mark as loaded
    }
    else
        print_error("Falha ao carregar modelo " MODEL_PATH_BUSH "!");
}

void updateTrees(float dt, float worldSpeed) {
    static float spawnTimer = 0.0f;
    spawnTimer += dt;

    if(spawnTimer > 0.5f) {
        spawnTimer = 0.0f;
        for(int i=0; i<MAX_TREES; i++) {
            if(!treesPool[i].active) {
                treesPool[i].active = 1;
                treesPool[i].x = (rand() % 2 == 0) ? -3.0f : 8.0f;
                treesPool[i].y = 0.0f;
                treesPool[i].z = -60.0f - (rand() % 20);
                treesPool[i].escala = 2.5f + ((float)(rand() % 100) / 100.0f) * 2.0f;

                // Spawn bush ao lado da árvore
                for(int j=0; j<MAX_BUSHES; j++) {
                    if(!bushesPool[j].active) {
                        bushesPool[j].active = 1;
                        // Coloca o bush ao lado da árvore (deslocamento lateral)
                        float deslocamento = (treesPool[i].x < 0) ? 0.5f : -0.5f;
                        bushesPool[j].x = treesPool[i].x + deslocamento;
                        bushesPool[j].y = treesPool[i].y;
                        bushesPool[j].z = treesPool[i].z;
                        bushesPool[j].escala = 1.0f + ((float)(rand() % 100) / 100.0f) * 0.7f;
                        break;
                    }
                }
                break;
            }
        }
    }

    for(int i = 0; i < MAX_TREES; i++) {
        if(treesPool[i].active) {
            treesPool[i].z += worldSpeed * dt;
            if(treesPool[i].z > 10.0f) {
                treesPool[i].active = 0;
            }
        }
    }
}

void updateBushes(float dt, float worldSpeed) {
    for(int i = 0; i < MAX_BUSHES; i++) {
        if(bushesPool[i].active) {
            bushesPool[i].z += worldSpeed * dt;
            if(bushesPool[i].z > 10.0f) {
                bushesPool[i].active = 0;
            }
        }
    }
}

void drawTrees() {
    glColor3f(0.0f, 0.6f, 0.0f); // cor de tree
    for(int i = 0; i < MAX_TREES; i++) {
        if(!treesPool[i].active)
            continue;
        if (model_not_loaded & TREE)
            continue;

        glPushMatrix();
        glTranslatef(treesPool[i].x, treesPool[i].y, treesPool[i].z);
        glScalef(treesPool[i].escala, treesPool[i].escala, treesPool[i].escala);
        draw_model(&treeModel);
        glPopMatrix();
    }
}

void drawBushes() {
    glColor3f(0.2f, 0.7f, 0.2f); // cor de bush
    for(int i = 0; i < MAX_BUSHES; i++) {
        if(!bushesPool[i].active)
            continue;
        if (model_not_loaded & BUSH)
            continue;

        glPushMatrix();
        glTranslatef(bushesPool[i].x, bushesPool[i].y, bushesPool[i].z);
        glScalef(bushesPool[i].escala, bushesPool[i].escala, bushesPool[i].escala);
        draw_model(&bushModel);
        glPopMatrix();
    }
}

void free_tree_model()
{
    free_model(&treeModel);
    model_not_loaded |= TREE;
}

void free_bush_model()
{
    free_model(&bushModel);
    model_not_loaded |= BUSH;
}
