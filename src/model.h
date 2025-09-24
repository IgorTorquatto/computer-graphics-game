#ifndef MODEL_H
#define MODEL_H

typedef struct {
    int numVertices;
    float* vertices;    // x,y,z por vértice
    int numFaces;
    int* faces;        // índices dos vértices, 3 por face (triângulos)
    float minY, maxY;  // altura para escala
} Model;

int loadOBJ(const char* filename, Model* model);
void drawModel(const Model* model);
void freeModel(Model* model);

#endif
