#pragma once
#include "component.h"

// === Entity Management ===
extern size_t game_objects_count;
extern size_t meshes_count;
extern size_t spheres_count;
extern size_t cuboid_count;
extern size_t cube_count;
extern size_t torus_count;
extern size_t polygons_count;
extern size_t labels_count;

EntityId create_game_object();

EntityId create_label(char *text, Position p);

void remove_entity(EntityId);

void set_label_text(EntityId, char*);
void set_label_color(EntityId, Color);

void set_position(EntityId, Position);
void set_mesh_color(EntityId, Color);
void set_velocity(EntityId, Velocity);
void set_rotation_velocity(EntityId, Velocity);

void translate(EntityId, Position);
void rotate_x(EntityId, Angle theta);
void rotate_y(EntityId, Angle theta);
void rotate_z(EntityId, Angle theta);

typedef Position* Vertices;

EntityId add_mesh(EntityId);
EntityId add_sphere(EntityId);
EntityId add_cuboid(EntityId);
EntityId add_cube(EntityId);
EntityId add_torus(EntityId);
void add_polygon(EntityId, Vertices, size_t);

void bind_process_function(EntityId, ProcessFunction);
void bind_input_function(EntityId, InputFunction);
