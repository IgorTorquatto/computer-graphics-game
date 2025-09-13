#pragma once

#define MAX_ENTITIES 256
#define INVALID_ENTITY MAX_ENTITIES

typedef unsigned long EntityId;
typedef unsigned long size_t;

/* === Components === */

typedef struct {
    float x = 0.0f, y = 0.0f, z = 0.0f;
} Position;

typedef Position Vector;
typedef Vector Velocity;

typedef struct {
    float r = 1.0f, g = 1.0f, b = 1.0f;
} Color;

// Matriz de transformação 4x4 [coordenadas homogêneas ocultas]
typedef struct {
    Vector columns[4]{
        { 1.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f }
    }; // default to identity
} Transform;

// Aliases para nomear cada vetor-coluna da matriz.
// Ex.: model.x_axis = models.columns[0], model.origin = model.columns[3]
#define x_axis columns[0]
#define y_axis columns[1]
#define z_axis columns[2]
#define origin columns[3]

Transform transform(Transform* by, Transform* m); // Matrix transformation 3D
void set_origin(Transform* m, Position at);

typedef struct {
    size_t vertex_count = 0;
    Position *vertices = nullptr; // lista de vértices
} Polygon;

typedef struct {
    Color color{};
    Transform model{};
    Polygon polygon{};
} Mesh;

// === Component Arrays ===
extern Mesh meshes[MAX_ENTITIES];
// TODO -> Memory Pool

// === Entity Management ===
extern bool entityAlive[MAX_ENTITIES];
