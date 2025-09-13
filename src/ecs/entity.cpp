#include <math.h> // para sin, cos

#include "component.h"

EntityId create_entity() {
    for (size_t i = 0; i < MAX_ENTITIES; i++)
        if (!entityAlive[i]) {
            entityAlive[i] = true;
            return i;
        }
    return INVALID_ENTITY; // sem espaço
}

void set_position(EntityId e, Position p) {
    set_origin(&meshes[e].model, p);
}

void translate(EntityId e, Position p) {
    auto t_mat = Transform{
        Position{1, 0, 0},
        Position{0, 1, 0},
        Position{0, 0, 1},
        p
    };
    meshes[e].model = transform(&meshes[e].model, &t_mat);
}

void rotate_x(EntityId e, float theta)
{
    auto rotate = [](Transform* t, float theta) {
        float c = cosf(theta);
        float s = sinf(theta);

        // matriz de rotação em X
        Transform r = {
            Vector{1, 0, 0},         // x_axis permanece
            Vector{0, c, -s},        // y_axis rotaciona
            Vector{0, s, c},         // z_axis rotaciona
            Position{0, 0, 0}          // origem é zero (aplicar depois)
        };

        return transform(t, &r);
    };
    meshes[e].model = rotate(&meshes[e].model, theta);
}


void rotate_y(EntityId e, float theta)
{
	auto rotate = [](Transform *t, float theta) {
        float c = cosf(theta);
        float s = sinf(theta);

        Transform r = {Vector{c, 0, s}, // x_axis rotaciona
            Vector{0, 1, 0},            // y_axis permanece
            Vector{-s, 0, c},           // z_axis rotaciona
            Position{0, 0, 0}};

	    return transform(t, &r);
    };

    meshes[e].model = rotate(&meshes[e].model, theta);
}


void rotate_z(EntityId e, float theta)
{
    auto rotate = [](Transform* t, float theta) {
        float c = cosf(theta);
        float s = sinf(theta);

        Transform r = {
            Vector{c, -s, 0},        // x_axis rotaciona
            Vector{s, c, 0},         // y_axis rotaciona
            Vector{0, 0, 1},         // z_axis permanece
            Position{0, 0, 0}
        };

        return transform(t, &r);
    };

    meshes[e].model = rotate(&meshes[e].model, theta);
}

void set_color(EntityId e, Color color) {
    meshes[e].color = color;
}

#include <iostream>

void add_polygon(EntityId e, Position *vertices, size_t count) {
    meshes[e].polygon.vertex_count = count;

    delete[] meshes[e].polygon.vertices; // limpa a memória antiga
    meshes[e].polygon.vertices = vertices;

    // setter method
    //for (size_t i = 0; i < count; i++)
    //    meshes[e].polygon.vertices[i] = {vertices[i].x, vertices[i].y};
}
