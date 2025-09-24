#include "ecs/entity.h"
#include "utils/print.h"

#include "test.h"

static EntityId test_object_id = INVALID_ENTITY;


void test_input(InputEvent event) {
    if (event.type != KEYBOARD)
        return;
    switch (event.key) {
        case 'a':
            translate(test_object_id, vector_right);
            break;

        case 'd':
            translate(test_object_id, vector_left);
            break;

        case 'w':
            translate(test_object_id, vector_up);
            break;

        case 's':
            translate(test_object_id, vector_down);
            break;

        default:
            break;
    }
}


void create_test_object() {
    test_object_id = create_game_object();

    if (test_object_id == INVALID_ENTITY) {
        print_error("Failed to create test object. Out of space.");
        return;
    }
    EntityId mesh_id = add_mesh(test_object_id);
    set_color(test_object_id, color_yellow);
    set_rotation_velocity(test_object_id, (Velocity){0.0f, 1.0f, 0.0f});

    //set_position(mesh_id, Position{0, 0, 0});
    const size_t count = 12;
    Position *vertices = (Position *)malloc(sizeof(Position) * count * 3);
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
    translate(test_object_id, (Position){0, 0, -5.0f});
    //set_position(test_object_id, Position{0, 0, -5.0f});

    rotate_x(test_object_id, deg_to_rad(-30));
    rotate_y(test_object_id, deg_to_rad(45));

    bind_input_function(test_object_id, test_input);
}
