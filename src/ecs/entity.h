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

typedef Position* Vertices;

EntityId create_game_object();

EntityId create_label(char *text, Position p);

void remove_game_object(EntityId);

void set_label_text(EntityId, char*);
void set_label_color(EntityId, Color);

void set_position(EntityId, Position);
void set_color(EntityId, Color);
void set_velocity(EntityId, Velocity);
void set_rotation_velocity(EntityId, Velocity);

void translate(EntityId, Position);
void rotate_x(EntityId, Angle theta);
void rotate_y(EntityId, Angle theta);
void rotate_z(EntityId, Angle theta);

GeometryId add_mesh(EntityId);
GeometryId add_sphere(EntityId, float, Position);
GeometryId add_cuboid(EntityId, Position);
GeometryId add_cube(EntityId);
GeometryId add_torus(EntityId, float, float, Position);

inline GeometryType get_geometry_type(GeometryId id) {
    return game_objects[id].geometry.type;
}

// References the data directly.
inline Sphere& get_sphere_ref(GeometryId id) {
    return spheres[id];
}

// References the data directly.
inline Torus& get_torus_ref(GeometryId id) {
    return toruses[id];
}

// Adds an array of polygons to a mesh.
void add_polygon(GeometryId, Vertices, size_t);

void bind_process_function(EntityId, ProcessFunction);
void bind_input_function(EntityId, InputFunction);
