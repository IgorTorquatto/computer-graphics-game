/*#pragma once

// SDL2
#ifdef _WIN32 // Setup horrível do code::blocks com mingw2-32bits
    #include <SDL.h>
    #include <SDL_mixer.h>
#else
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_mixer.h>
#endif

extern Mix_Music *music;

void audio_bus_init();
void audio_bus_play_music();
void audio_bus_stop_music();
void audio_bus_close();*/
