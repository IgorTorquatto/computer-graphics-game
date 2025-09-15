#pragma once
#include "stdlib.h" // size_t
#include "math.h"

#define rad_to_deg(radians) ((float)(radians) * 180 / M_PI)
#define deg_to_rad(degrees) ((float)(degrees) * M_PI / 180)

#define ms (1e3) // milliseconds

#define MAX_ENTITIES 256
#define INVALID_ENTITY MAX_ENTITIES

typedef unsigned long EntityId; // Used for Game Objects and other components
typedef EntityId GeometryId; // Entities of type Geometry
typedef float Angle;

/* === Components === */

typedef struct {
    float x = 0.0f, y = 0.0f, z = 0.0f;
} Position;

inline Position sum(Position p, Position q)
{
    return Position{p.x + q.x, p.y + q.y, p.z + q.z};
}

inline Position sub(Position p, Position q)
{
    return Position{p.x - q.x, p.y - q.y, p.z - q.z};
}

inline Position mul(Position p, float scalar)
{
    return Position{p.x * scalar, p.y * scalar, p.z * scalar};
}

inline Position div(Position p, float scalar)
{
    return Position{p.x / scalar, p.y / scalar, p.z / scalar};
}

typedef Position Vector;
typedef Vector Velocity;

// === Vector Constants ===
#define vector_zero  Vector{}
#define vector_one   Vector{1.0f, 1.0f, 1.0f}
#define vector_up    Vector{0.0f, 1.0f, 0.0f}
#define vector_down  Vector{0.0f, -1.0f, 0.0f}
#define vector_right Vector{1.0f, 0.0f, 0.0f}
#define vector_left  Vector{-1.0f, 0.0f, 0.0f}

typedef struct {
    float r = 1.0f, g = 1.0f, b = 1.0f;
} Color;

// === Color Constants ===
#define color_black   Color{0.0f, 0.0f, 0.0f}
#define color_white   Color{1.0f, 1.0f, 1.0f}
#define color_red     Color{1.0f, 0.0f, 0.0f}
#define color_green   Color{0.0f, 1.0f, 0.0f}
#define color_blue    Color{0.0f, 0.0f, 1.0f}
#define color_magenta Color{1.0f, 0.0f, 1.0f}
#define color_yellow  Color{1.0f, 1.0f, 0.0f}
#define color_cyan    Color{0.0f, 1.0f, 1.0f}

// Matriz de transformação 4x4 [coordenadas homogêneas ocultas]
typedef struct {
    Vector columns[4]{
        { 1.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f }
    }; // default to identity
} Transform;

// === Aliases para nomear cada vetor-coluna da matriz. ===
// Ex.: model.x_axis = models.columns[0], model.origin = model.columns[3]
#define x_axis columns[0]
#define y_axis columns[1]
#define z_axis columns[2]
#define origin columns[3]

// Type conversion macros

// converts Transform to glMatrix
#define to_GLfloat_matrix(m) { \
    model.x_axis.x, model.x_axis.y, model.x_axis.z, 0.0f, model.y_axis.x, \
    model.y_axis.y, model.y_axis.z, 0.0f, model.z_axis.x, model.z_axis.y, \
    model.z_axis.z, 0.0f, model.origin.x, model.origin.y, model.origin.z, 1.0f \
}

typedef struct {
    char* text = nullptr;
    Position position{};
    Color color{};
} Label;

//#region === Geometry primitives ===
#pragma region // TODO -> Check if it is available in code blocks Mingw
    Transform transform(Transform* by, Transform* m); // Matrix transformation 3D
    void set_origin(Transform* m, Position at);

    typedef struct {
        size_t vertices_count = 0;
        Position *vertices = nullptr; // lista de vértices
    } C_Polygon;
#pragma endregion
//#endregion

// === Geometry ===
enum class GeometryType {
    NONE, MESH, SPHERE,
    CUBOID, CUBE, TORUS,
};
// TODO: Add text label component

typedef struct {
    EntityId id = INVALID_ENTITY;
    GeometryType type = GeometryType::NONE;
    Transform model; // Matriz de transformação 4x4
} Geometry;

typedef struct {
    Color color{};
    C_Polygon polygon{};
} Mesh;

typedef struct {
    float radius;
    Position center;
    Color color{};
    unsigned slices = 32;
    unsigned stacks = 32;
    bool isSolid = true;
} Sphere;

typedef struct {
    // The dimensions + rotation, and scale are determined from unity
    // cube using the `Geometry` `model`'s `Transform` matrix.
    Position center;
    Color color{};
    bool isSolid = true;
} Cuboid;

typedef struct {
    Position center;
    Vector dimensions;
    Color color{};
    bool isSolid = true;
} Cube;

typedef struct {
    Position center;
    float innerRadius;
    float outerRadius;
    Color color{};
    unsigned slices = 32;
    unsigned stacks = 32;
    bool isSolid = true;
} Torus;


enum class InputType {
    KEYBOARD,
    SPECIAL,
    MOUSE,
};

struct InputEvent {
    int key;
    int state;
    Position position; // Screen Position
    InputType type;
};

typedef float DeltaTime;
typedef void(* ProcessFunction)(DeltaTime);
typedef void(* InputFunction)(InputEvent);

// === Archetypes ===
typedef struct {
    Geometry geometry{};
    Velocity velocity{};
    Velocity rotationVelocity{};
    ProcessFunction process = nullptr;
    InputFunction input = nullptr;
} GameObject;

// === Component Arrays ===
extern Label labels[MAX_ENTITIES];

extern Mesh meshes[MAX_ENTITIES];
extern Sphere spheres[MAX_ENTITIES];
extern Cuboid cuboids[MAX_ENTITIES];
extern Cube cubes[MAX_ENTITIES];
extern Torus toruses[MAX_ENTITIES];

extern GameObject game_objects[MAX_ENTITIES];

extern float fps; // measured in main
