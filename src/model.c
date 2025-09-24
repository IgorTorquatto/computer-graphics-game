#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>
#include "model.h"

// Função auxiliar para contar elementos em arquivo OBJ (vértices e faces)
static void countElements(FILE* file, int* outVertCount, int* outFaceCount) {
    char line[256];
    *outVertCount = 0;
    *outFaceCount = 0;

    while(fgets(line, sizeof(line), file)) {
        // Contar vértices
        if(strncmp(line, "v ", 2) == 0) (*outVertCount)++;
        // Contar faces
        else if(strncmp(line, "f ", 2) == 0) (*outFaceCount)++;
    }
}

// Função para carregar OBJ simples (vértices + faces triangulares)
int loadOBJ(const char* filename, Model* model) {
    FILE* file = fopen(filename, "r");
    if(!file) {
        fprintf(stderr, "Erro ao abrir arquivo OBJ: %s\n", filename);
        return 0;
    }

    int vertCount = 0;
    int faceCount = 0;
    countElements(file, &vertCount, &faceCount);
    rewind(file);

    model->numVertices = vertCount;
    model->numFaces = faceCount;
    model->vertices = (float*)malloc(sizeof(float)*3*vertCount);
    model->faces = (int*)malloc(sizeof(int)*3*faceCount); // 3 indices por face

    if(!model->vertices || !model->faces) {
        fclose(file);
        return 0;
    }

    int vIdx = 0, fIdx = 0;
    char line[256];
    while(fgets(line, sizeof(line), file)) {
        if(strncmp(line, "v ", 2) == 0) {
            float x,y,z;
            sscanf(line+2, "%f %f %f", &x, &y, &z);
            model->vertices[vIdx++] = x;
            model->vertices[vIdx++] = y;
            model->vertices[vIdx++] = z;
        } else if(strncmp(line, "f ", 2) == 0) {
            // Aqui assumimos faces triangulares sem normais ou texturas
            // Formato comum: f v1 v2 v3 ou f v1/... v2/... v3/...
            int vi[3] = {0,0,0};
            if(sscanf(line+2, "%d %d %d", &vi[0], &vi[1], &vi[2]) < 3) {
                // Tentar formato com barras: "f 1/1/1 2/2/2 3/3/3"
                sscanf(line+2, "%d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &vi[0], &vi[1], &vi[2]);
            }
            // OBJ é 1-indexado, converter para 0-index
            model->faces[fIdx++] = vi[0]-1;
            model->faces[fIdx++] = vi[1]-1;
            model->faces[fIdx++] = vi[2]-1;
        }
    }
    fclose(file);

    // Calcular altura mínima e máxima (eixo Y)
    model->minY = model->vertices[1];
    model->maxY = model->vertices[1];
    for(int i=0; i<model->numVertices; i++) {
        float y = model->vertices[i*3 + 1];
        if(y < model->minY) model->minY = y;
        if(y > model->maxY) model->maxY = y;
    }

    return 1;
}

// Desenha o modelo usando GL_TRIANGLES
void drawModel(const Model* model) {
    glBegin(GL_TRIANGLES);
    for(int i=0; i < model->numFaces; i++) {
        int idx0 = model->faces[i*3] * 3;
        int idx1 = model->faces[i*3 + 1] * 3;
        int idx2 = model->faces[i*3 + 2] * 3;

        // Nessa versão simples não tem normais — pode calcular normais para iluminação após
        // Por enquanto, chamamos vertex diretamente

        glVertex3f(model->vertices[idx0], model->vertices[idx0+1], model->vertices[idx0+2]);
        glVertex3f(model->vertices[idx1], model->vertices[idx1+1], model->vertices[idx1+2]);
        glVertex3f(model->vertices[idx2], model->vertices[idx2+1], model->vertices[idx2+2]);
    }
    glEnd();
}

void freeModel(Model* model) {
    if(model->vertices) free(model->vertices);
    if(model->faces) free(model->faces);
    model->vertices = NULL;
    model->faces = NULL;
    model->numVertices = 0;
    model->numFaces = 0;
}
