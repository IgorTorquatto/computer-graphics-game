#pragma once

#define IMG_PATH_FLOOR "textures/floor/stone_tiles_diff.jpg"

extern float z_far; // defined in main.c


void init_floor();

void update_floor(float delta);

void draw_floor();
