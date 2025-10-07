#pragma once

// SDL2
#ifdef _WIN32 // Setup horrível do code::blocks com mingw2-32bits
    #include <SDL.h>
    #include <SDL_mixer.h>
#else
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_mixer.h>
#endif
#include "utils/basics.h" // bool

extern Mix_Music *music;

typedef enum {
    MUSIC_GAME,
    MUSIC_MENU,
    MUSIC_OPENING,
    MUSIC_GAME_OVER,
    MUSIC_N // number of audios
} MusicTrack;

typedef enum {
    SFX_UI_1,      SFX_UI_2,
    SFX_UI_SELECT, SFX_UI_CANCEL,
    SFX_JUMP_1,    SFX_JUMP_2,     SFX_DASH,
    SFX_WALK,      SFX_RUN,
    SFX_COIN_PICK, SFX_COIN_DROP,
    SFX_DAMAGE,    SFX_ACHIEVEMENT,
    SFX_N // number of sounds
} SfxTrack;

void audio_bus_init();

void audio_bus_play_music(MusicTrack, bool once);
void audio_bus_stop_music();

void audio_bus_unpause_music();
void audio_bus_pause_music();

void audio_bus_play_sfx(SfxTrack);
void audio_bus_play_channel(SfxTrack track, int channel);
void audio_bus_stop_channel(int channel);
void audio_bus_stop_all_channels();

void audio_bus_close();
