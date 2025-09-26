#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>
#include "model.h"
#include "math.h"

// Função auxiliar para contar elementos em arquivo OBJ (vértices e faces)
static void countElements(FILE* file, int* outVertCount, int* outFaceCount) {
    char line[256];
    *outVertCount = 0;
    *outFaceCount = 0;

    while(fgets(line, sizeof(line), file)) {
        if(strncmp(line, "v ", 2) == 0) (*outVertCount)++;
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
    model->numFaces = 0;
    model->vertices = (float*)malloc(sizeof(float)*3*vertCount);
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
            // Tenta todos os formatos conhecidos de face
            if (sscanf(line+2, "%d %d %d", &vi[0], &vi[1], &vi[2]) == 3) {
                // f v v v
            } else if (sscanf(line+2, "%d//%*d %d//%*d %d//%*d", &vi[0], &vi[1], &vi[2]) == 3) {
                // f v//vn v//vn v//vn (bush)
            } else if (sscanf(line+2, "%d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &vi[0], &vi[1], &vi[2]) == 3) {
                // f v/vt/vn v/vt/vn v/vt/vn (tree)
            } else if (sscanf(line+2, "%d/%*d %d/%*d %d/%*d", &vi[0], &vi[1], &vi[2]) == 3) {
                // f v/vt v/vt v/vt
            } else {
                // formato não suportado
                continue;
            }
            int v0 = vi[0] - 1;
            int v1 = vi[1] - 1;
            int v2 = vi[2] - 1;
            if (v0 < 0 || v0 >= vertCount ||
                v1 < 0 || v1 >= vertCount ||
                v2 < 0 || v2 >= vertCount) {
                fprintf(stderr, "Face ignorada por índice inválido: %s", line);
                continue;
            }
            model->faces[fIdx++] = v0;
            model->faces[fIdx++] = v1;
            model->faces[fIdx++] = v2;
            model->numFaces++;
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

void drawModel(const Model* model) {
    glBegin(GL_TRIANGLES);
    for(int i=0; i < model->numFaces; i++) {
        int idx0 = model->faces[i*3] * 3;
        int idx1 = model->faces[i*3 + 1] * 3;
        int idx2 = model->faces[i*3 + 2] * 3;

        float v0[3] = { model->vertices[idx0], model->vertices[idx0+1], model->vertices[idx0+2] };
        float v1[3] = { model->vertices[idx1], model->vertices[idx1+1], model->vertices[idx1+2] };
        float v2[3] = { model->vertices[idx2], model->vertices[idx2+1], model->vertices[idx2+2] };
        // Calcula normal
        float u[3] = { v1[0]-v0[0], v1[1]-v0[1], v1[2]-v0[2] };
        float v[3] = { v2[0]-v0[0], v2[1]-v0[1], v2[2]-v0[2] };
        float normal[3] = {
            u[1]*v[2] - u[2]*v[1],
            u[2]*v[0] - u[0]*v[2],
            u[0]*v[1] - u[1]*v[0]
        };
        float len = sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);
        if(len > 0.0f) {
            normal[0] /= len; normal[1] /= len; normal[2] /= len;
        }
        glNormal3f(normal[0], normal[1], normal[2]);
        glVertex3fv(v0);
        glVertex3fv(v1);
        glVertex3fv(v2);
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
