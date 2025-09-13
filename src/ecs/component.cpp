#include "component.h"

#include <GL/freeglut.h>
#include <stdbool.h>

// === Component Arrays ===
Mesh meshes[MAX_ENTITIES];

// === Entity Management ===
bool entityAlive[MAX_ENTITIES];


Transform transform(Transform* m, Transform* by)
{
    auto transform_axis_vector = [&](Vector v) {
        return Vector{ // transformação dos eixos [h = 0]
            m->x_axis.x * v.x + m->y_axis.x * v.y + m->z_axis.x * v.z,
            m->x_axis.y * v.x + m->y_axis.y * v.y + m->z_axis.y * v.z,
            m->x_axis.z * v.x + m->y_axis.z * v.y + m->z_axis.z * v.z
        };
    };
    auto transform_point = [&](Vector v) {
        return Vector{ // transformação completa no vetor de translação [h = 1]
            m->x_axis.x * v.x + m->y_axis.x * v.y + m->z_axis.x * v.z + m->origin.x,
            m->x_axis.y * v.x + m->y_axis.y * v.y + m->z_axis.y * v.z + m->origin.y,
            m->x_axis.z * v.x + m->y_axis.z * v.y + m->z_axis.z * v.z + m->origin.z
        };
    };

    return {
        transform_axis_vector(by->x_axis),
        transform_axis_vector(by->y_axis),
        transform_axis_vector(by->z_axis),
        transform_point(by->origin)
    };
}


void set_origin(Transform* m, Position at)
{
    m->origin.x = at.x;
    m->origin.y = at.y;
    m->origin.z = at.z;
}
