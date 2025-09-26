#pragma once

#include "ecs/systems/menu.h"
#include "ecs/systems/hud.h"

// Diferença de tempo em segundos
typedef float DeltaTime;

void system_init();

void system_render();

void system_process(DeltaTime);

void system_input(InputEvent);
