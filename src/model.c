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
    model->numFaces = 0;  // contar faces válidas

    model->vertices = (float*)malloc(sizeof(float)*3*vertCount);
    // Alocar o máximo possível: faceCount faces, depois ajustamos numFaces
    model->faces = (int*)malloc(sizeof(int)*3*faceCount);

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
            int vi[3] = {0,0,0};
            if(sscanf(line+2, "%d %d %d", &vi[0], &vi[1], &vi[2]) < 3) {
                sscanf(line+2, "%d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &vi[0], &vi[1], &vi[2]);
            }
            int v0 = vi[0] - 1;
            int v1 = vi[1] - 1;
            int v2 = vi[2] - 1;

            // Validar índices
            if (v0 < 0 || v0 >= vertCount ||
                v1 < 0 || v1 >= vertCount ||
                v2 < 0 || v2 >= vertCount) {
                fprintf(stderr, "Face ignorada por índice inválido: %s", line);
                continue;  // pular essa face inválida
            }

            model->faces[fIdx++] = v0;
            model->faces[fIdx++] = v1;
            model->faces[fIdx++] = v2;
            model->numFaces++;
        }
    }
    fclose(file);

    // Calcular minY e maxY
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
    for (int i = 0; i < model->numFaces; i++) {
        int vi0 = model->faces[i * 3];
        int vi1 = model->faces[i * 3 + 1];
        int vi2 = model->faces[i * 3 + 2];

        if (vi0 < 0 || vi0 >= model->numVertices ||
            vi1 < 0 || vi1 >= model->numVertices ||
            vi2 < 0 || vi2 >= model->numVertices) {
            fprintf(stderr, "Índice de face inválido em drawModel: face %d\n", i);
            continue;
        }

        int idx0 = vi0 * 3;
        int idx1 = vi1 * 3;
        int idx2 = vi2 * 3;

        glVertex3f(model->vertices[idx0], model->vertices[idx0 + 1], model->vertices[idx0 + 2]);
        glVertex3f(model->vertices[idx1], model->vertices[idx1 + 1], model->vertices[idx1 + 2]);
        glVertex3f(model->vertices[idx2], model->vertices[idx2 + 1], model->vertices[idx2 + 2]);
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
