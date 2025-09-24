#include "component.h"

#include <GL/freeglut.h>
#include <stdbool.h>

// === Component Arrays ===

Label labels[MAX_ENTITIES];
// Geometry
Mesh meshes[MAX_ENTITIES];
Sphere spheres[MAX_ENTITIES];
Cuboid cuboids[MAX_ENTITIES];
Cube cubes[MAX_ENTITIES];
Torus toruses[MAX_ENTITIES];
// Archetypes
GameObject game_objects[MAX_ENTITIES];

float fps = 0.0f;


static Vector transform_axis_vector(Transform* m, Vector v) {
    return (Vector){ // transformação dos eixos [h = 0]
        m->x_axis.x * v.x + m->y_axis.x * v.y + m->z_axis.x * v.z,
        m->x_axis.y * v.x + m->y_axis.y * v.y + m->z_axis.y * v.z,
        m->x_axis.z * v.x + m->y_axis.z * v.y + m->z_axis.z * v.z
    };
};


static Vector transform_point(Transform* m, Vector v) {
    return (Vector){ // transformação completa no vetor de translação [h = 1]
        m->x_axis.x * v.x + m->y_axis.x * v.y + m->z_axis.x * v.z + m->origin.x,
        m->x_axis.y * v.x + m->y_axis.y * v.y + m->z_axis.y * v.z + m->origin.y,
        m->x_axis.z * v.x + m->y_axis.z * v.y + m->z_axis.z * v.z + m->origin.z
    };
};


Transform transform(Transform* m, Transform* by)
{
    return (Transform){
        transform_axis_vector(m, by->x_axis),
        transform_axis_vector(m, by->y_axis),
        transform_axis_vector(m, by->z_axis),
        transform_point(m, by->origin)
    };
}


void set_origin(Transform* m, Position at)
{
    m->origin.x = at.x;
    m->origin.y = at.y;
    m->origin.z = at.z;
}
