#pragma once
#include "component.h"

// === Entity Management ===
extern size_t meshes_count; // Se a ordem for importante, e houver remoção, usar uma técnica de memory pool.

EntityId create_entity();
void remove_entity(EntityId e);

void set_position(EntityId e, Position position);

void set_color(EntityId e, Color color);

void translate(EntityId e, Position p);
void rotate_x(EntityId e, float theta);
void rotate_y(EntityId e, float theta);
void rotate_z(EntityId e, float theta);

void add_polygon(EntityId e, Position *vertices, size_t count);
