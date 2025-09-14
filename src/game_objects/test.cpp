#include "../ecs/entity.h"

#include "../utils/print.h"
#include "test.h"


void create_test_object() {
    EntityId id = create_game_object();

    if (id == INVALID_ENTITY) {
        print_error("Failed to create test object. Out of space.");
        return;
    }
    EntityId mesh_id = add_mesh(id);
    set_mesh_color(mesh_id, color_yellow);
    set_rotation_velocity(mesh_id, Velocity{0.0f, -1.0f, 0.0f});

    //set_position(mesh_id, Position{0, 0, 0});
    const size_t count = 12;
    Position *vertices = new Position[count * 3];
    Position v[8] = {
        {-0.5f, -0.5f, -0.5f}, // 0
        { 0.5f, -0.5f, -0.5f}, // 1
        { 0.5f,  0.5f, -0.5f}, // 2
        {-0.5f,  0.5f, -0.5f}, // 3
        {-0.5f, -0.5f,  0.5f}, // 4
        { 0.5f, -0.5f,  0.5f}, // 5
        { 0.5f,  0.5f,  0.5f}, // 6
        {-0.5f,  0.5f,  0.5f}  // 7
    };
    int faces[12][3] = {
        // fundo (z = -0.5)
        {0, 1, 2}, {0, 2, 3},
        // topo (z = +0.5)
        {4, 6, 5}, {4, 7, 6},
        // frente (y = +0.5)
        {3, 2, 6}, {3, 6, 7},
        // trás (y = -0.5)
        {0, 5, 1}, {0, 4, 5},
        // direita (x = +0.5)
        {1, 5, 6}, {1, 6, 2},
        // esquerda (x = -0.5)
        {0, 3, 7}, {0, 7, 4}
    };
    for (size_t i = 0; i < count; i++) {
        vertices[i*3 + 0] = v[faces[i][0]];
        vertices[i*3 + 1] = v[faces[i][1]];
        vertices[i*3 + 2] = v[faces[i][2]];
    }
    add_polygon(mesh_id, vertices, count * 3);
    //translate(mesh_id, Position{0, 0, -5.0f});
    set_position(mesh_id, Position{0, 0, -5.0f});

    rotate_x(id, deg_to_rad(30));
    rotate_y(id, deg_to_rad(-45));
}
