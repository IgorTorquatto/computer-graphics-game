#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

extern Mix_Music *music;

void audio_bus_init();
void audio_bus_play_music();
void audio_bus_stop_music();
void audio_bus_close();
