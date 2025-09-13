#include <math.h> // para sin, cos

#include "component.h"
#include "../utils/print.h"

// === Entity Management ===
size_t meshes_count = 0;

EntityId create_entity() {
    EntityId e = meshes_count;
    if (++meshes_count == MAX_ENTITIES)
        return INVALID_ENTITY; // sem espaço
    return e; // Using default values for the mesh
}

// Remoção de entidades não é recomendado nesse sistema.
// A ordem dos elementos é alterada na remoção.
void remove_entity(EntityId e)
{
    print_warning("Remoção de entidades não é recomendado nesse sistema.\n"
        "\tA ordem dos elementos é alterada na remoção.");

    auto reset_mesh = [](Mesh* m) {
        m->model = Transform{
            Position{1, 0, 0},
            Position{0, 1, 0},
            Position{0, 0, 1},
            Position{0, 0, 0}
        };
        m->color = Color{1, 1, 1};
        delete[] m->polygon.vertices;
        m->polygon = C_Polygon{};
    };
    auto override_mesh = [](Mesh* from, Mesh* to) {
        to->model = from->model;
        to->color = from->color;
        to->polygon = from->polygon;
    };

    reset_mesh(&static_meshes[e]);
    override_mesh(&static_meshes[e], &static_meshes[--meshes_count]);
}

void set_position(EntityId e, Position p) {
    set_origin(&static_meshes[e].model, p);
}

void translate(EntityId e, Position p) {
    auto t_mat = Transform{
        Position{1, 0, 0},
        Position{0, 1, 0},
        Position{0, 0, 1},
        p
    };
    static_meshes[e].model = transform(&static_meshes[e].model, &t_mat);
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
    static_meshes[e].model = rotate(&static_meshes[e].model, theta);
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

    static_meshes[e].model = rotate(&static_meshes[e].model, theta);
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

    static_meshes[e].model = rotate(&static_meshes[e].model, theta);
}

void set_color(EntityId e, Color color) {
    static_meshes[e].color = color;
}

void add_polygon(EntityId e, Position *vertices, size_t count) {
    static_meshes[e].polygon.vertex_count = count;

    delete[] static_meshes[e].polygon.vertices; // limpa a memória antiga
    static_meshes[e].polygon.vertices = vertices;

    // setter method
    //for (size_t i = 0; i < count; i++)
    //    meshes[e].polygon.vertices[i] = {vertices[i].x, vertices[i].y};
}
