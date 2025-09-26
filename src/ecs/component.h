#pragma once

#include "utils/basics.h"

#include "components/model.h"
#include "components/coin.h"


#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#pragma region Unities
    #define rad_to_deg(radians) ((float)(radians) * 180 / M_PI)
    #define deg_to_rad(degrees) ((float)(degrees) * M_PI / 180)

    #define _ms (1.0e3)

    typedef float Angle;

    typedef float DeltaTime;
#pragma endregion

#pragma region Interpolation
    /* Interpolação Linear de `from` para `to` em intervalos `delta` */
    #define lerp(from, to, delta) ((from) + ((to) - (from)) * (delta))

    /* Smooth interpolation */
    #define smoothstep(edge0, edge1, x) ((x) <= (edge0) ? 0 : ((x) >= (edge1) ? 1 : ((x) - (edge0)) / ((edge1) - (edge0))))

    /* Cubic interpolation */
    #define cubicstep(edge0, edge1, x) ((x) <= (edge0) ? 0 : ((x) >= (edge1) ? 1 : ((x) - (edge0)) / ((edge1) - (edge0)) * ((x) - (edge0))))

    /* Spline interpolation */
    #define splinestep(edge0, edge1, x) ((x) <= (edge0) ? 0 : ((x) >= (edge1) ? 1 : ((x) - (edge0)) / ((edge1) - (edge0)) * ((x) - (edge0)) * ((x) - (edge0))))

    /* See graphics at https://easings.net/ */
    #pragma Region Easing
        #define ease_in_quad(x) ((x) * (x))
        #define ease_in_cubic(x) ((x) * (x) * (x))
        #define ease_in_sine(x) (1 - cos((x) * M_PI / 2))

        #define ease_out_quad(x) (1 - (1 - (x)) * (1 - (x)))
        #define ease_out_cubic(x) (1 - pow(1 - (x), 3))
        #define ease_out_sine(x) (sin((x) * M_PI / 2))

        #define ease_in_out_quad(x) (x < 0.5 ? 2 * x * x : 1 - pow(-2 * x + 2, 2) / 2)
        #define ease_in_out_sine(x) (-(cos(M_PI * x) - 1) / 2)
        #define ease_in_out_circle(x) (1 - sqrt(1 - (x) * (x)))
    #pragma endregion
#pragma endregion

#define MAX_ENTITIES 256
#define INVALID_ENTITY MAX_ENTITIES

typedef unsigned long EntityId; // Used for Game Objects and other components
typedef EntityId GeometryId; // Entities of type Geometry


#pragma region Primitives
    typedef struct {
        float x, y, z;
    } Position;
    #define position_zero (Position){0.0f, 0.0f, 0.0f}

    typedef Position Vector;
    typedef Vector Velocity;

    #define vector_sum(v, u) (Vector){(v).x + (u).x, (v).y + (u).y, (v).z + (u).z}
    #define vector_sub(v, u) (Vector){(v).x - (u).x, (v).y - (u).y, (v).z - (u).z}
    #define vector_mul(v, scalar) (Vector){(v).x * (scalar), (v).y * (scalar), (v).z * (scalar)}
    #define vector_div(v, scalar) (Vector){(v).x / (scalar), (v).y / (scalar), (v).z / (scalar)}

    // === Vector Constants ===
    #define vector_zero  (Vector)(position_zero)
    #define vector_one   (Vector){1.0f, 1.0f, 1.0f}
    #define vector_up    (Vector){0.0f, 1.0f, 0.0f}
    #define vector_down  (Vector){0.0f, -1.0f, 0.0f}
    #define vector_right (Vector){1.0f, 0.0f, 0.0f}
    #define vector_left  (Vector){-1.0f, 0.0f, 0.0f}

    typedef struct {
        float r, g, b;
    } Color;

    // === Color Constants ===
    #define color_black   (Color){0.0f, 0.0f, 0.0f}
    #define color_white   (Color){1.0f, 1.0f, 1.0f}
    #define color_red     (Color){1.0f, 0.0f, 0.0f}
    #define color_green   (Color){0.0f, 1.0f, 0.0f}
    #define color_blue    (Color){0.0f, 0.0f, 1.0f}
    #define color_magenta (Color){1.0f, 0.0f, 1.0f}
    #define color_yellow  (Color){1.0f, 1.0f, 0.0f}
    #define color_cyan    (Color){0.0f, 1.0f, 1.0f}
#pragma endregion

typedef struct {
    char* text;
    Position position;
    Color color;
} Label;

#define label_empty (Label){(char *)NULL, position_zero, color_white}

#pragma region Geometry

#pragma region Primitives
    // Matriz de transformação 4x4 [coordenadas homogêneas ocultas]
    typedef struct {
        Vector columns[4];
    } Transform;

    #define transform_identity (Transform){ \
        (Vector){ 1.0f, 0.0f, 0.0f }, \
        (Vector){ 0.0f, 1.0f, 0.0f }, \
        (Vector){ 0.0f, 0.0f, 1.0f }, \
        (Position){ 0.0f, 0.0f, 0.0f } \
    } // default Transform matrix value.

    // === Aliases para nomear cada vetor-coluna da matriz. ===
    // Ex.: model.x_axis = models.columns[0], model.origin = model.columns[3]
    #define x_axis columns[0]
    #define y_axis columns[1]
    #define z_axis columns[2]
    #define origin columns[3]

    // === Type conversion macros ===
    #define to_GLfloat_matrix(m) { \
        model->x_axis.x, model->x_axis.y, model->x_axis.z, 0.0f, model->y_axis.x, \
        model->y_axis.y, model->y_axis.z, 0.0f, model->z_axis.x, model->z_axis.y, \
        model->z_axis.z, 0.0f, model->origin.x, model->origin.y, model->origin.z, 1.0f \
    } // converts Transform to glMatrix

    Transform transform(Transform* by, Transform* m); // Matrix transformation 3D
    void set_origin(Transform* m, Position at);

    typedef struct {
        size_t vertices_count;
        Position *vertices; // lista de vértices
    } C_Polygon;
#pragma endregion

    typedef enum geometry_type {
        NONE, MESH, SPHERE,
        CUBOID, CUBE, TORUS,
    } GeometryType;

    typedef struct {
        EntityId id;
        GeometryType type;
        Transform model; // Matriz de transformação 4x4
    } Geometry;

    #define geometry_empty (Geometry){INVALID_ENTITY, NONE, transform_identity}

    typedef struct {
        Color color;
        C_Polygon polygon;
    } Mesh;

    #define mesh_empty (Mesh){color_white, (C_Polygon){0, NULL}}

    #define default_slices 32

    typedef struct {
        float radius;
        Position center;
        Color color;
        unsigned slices;
        unsigned stacks;
        bool isSolid;
    } Sphere;

    #define sphere_default (Sphere){1.0f, position_zero, color_white, default_slices, default_slices, true}

    typedef struct {
        // The dimensions + rotation, and scale are determined from unity
        // cube using the `Geometry` `model`'s `Transform` matrix.
        Position center;
        Color color;
        bool isSolid;
    } Cuboid;

    #define cuboid_default (Cuboid){position_zero, color_white, true}

    typedef struct {
        Position center;
        Vector dimensions;
        Color color;
        bool isSolid;
    } Cube;

    #define cube_default (Cube){position_zero, vector_one, color_white, true}

    typedef struct {
        Position center;
        float innerRadius;
        float outerRadius;
        Color color;
        unsigned slices;
        unsigned stacks;
        bool isSolid;
    } Torus;

    #define torus_default (Torus){position_zero, 0.5f, 1.0f, color_white, default_slices, default_slices, true}
#pragma endregion


enum InputType {
    KEYBOARD,
    SPECIAL,
    MOUSE,
};

typedef struct {
    int key;
    int state;
    Position position; // Screen Position
    enum InputType type;
} InputEvent;

typedef void(* ProcessFunction)(DeltaTime);
typedef void(* InputFunction)(InputEvent);

#pragma region Archetypes
    typedef struct {
        Geometry geometry;
        Velocity velocity;
        Velocity rotationVelocity;
        ProcessFunction process;
        InputFunction input;
    } GameObject;

    #define game_object_empty (GameObject){geometry_empty, vector_zero, vector_zero, (ProcessFunction)nullptr, (InputFunction)NULL}
#pragma endregion

#pragma region Component Arrays
    extern Label labels[MAX_ENTITIES];

    extern Mesh meshes[MAX_ENTITIES];
    extern Sphere spheres[MAX_ENTITIES];
    extern Cuboid cuboids[MAX_ENTITIES];
    extern Cube cubes[MAX_ENTITIES];
    extern Torus toruses[MAX_ENTITIES];

    extern GameObject game_objects[MAX_ENTITIES];
#pragma endregion

extern float fps; // measured in main
