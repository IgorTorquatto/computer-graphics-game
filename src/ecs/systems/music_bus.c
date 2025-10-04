#include <stdio.h>

#include "utils/print.h"

#include "music_bus.h"


// Acessados externamente com os ids de MusicTrack / SfxTrack
// São todos NULL por padrão (static)
static Mix_Music *musics[MUSIC_N];
static Mix_Chunk *effects[SFX_N];


void audio_bus_init()
{
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
        print_error("SDL Failed to load Audio device: %s\n", Mix_GetError());
        exit(EXIT_FAILURE);
    }

    Mix_AllocateChannels(32);

    struct music_bindings {
        MusicTrack track;
        const char* path;
    } music_bindings[] = {
        {MUSIC_MENU, "audio/ambient/Suburban Neighborhood_morning.ogg"},
        {MUSIC_GAME, "audio/music/1-epic-chase.mp3"},
        {MUSIC_OPENING, "audio/sfxs/opening/Spooky Ambience.ogg"},
        {MUSIC_GAME_OVER, "audio/sfxs/game_over/Slide guitar_decreasing pitch.ogg"},
    };

    for (struct music_bindings *b = music_bindings; b < music_bindings + MUSIC_N; ++b) {
        musics[b->track] = Mix_LoadMUS(b->path);
        if (!musics[b->track])
            print_error("Music [%s] loading error: %s", b->path, Mix_GetError());
    }

    struct sfx_bindings {
        SfxTrack track;
        const char* path;
    } sfx_bindings[] = {
        {SFX_UI_1, "audio/sfxs/ui/African2.wav"},
        {SFX_UI_2, "audio/sfxs/ui/African3.wav"},
        {SFX_UI_SELECT, "audio/sfxs/ui/African4.wav"},
        {SFX_UI_CANCEL, "audio/sfxs/ui/African1.wav"},
        {SFX_JUMP_1, "audio/sfxs/jump/Swipe-1.wav"},
        {SFX_JUMP_2, "audio/sfxs/jump/Swipe-2.wav"},
        {SFX_DASH, "audio/sfxs/dash/Whoosh Short Light 03.wav"},
        {SFX_WALK, "audio/sfxs/footsteps/Footsteps_walking.wav"}, // loop this
        {SFX_RUN, "audio/sfxs/footsteps/Footsteps_running.wav"}, // loop this
        {SFX_COIN_PICK, "audio/sfxs/coins/Coins in Sack Dropped on Soft Surface.wav"},
        {SFX_COIN_DROP, "audio/sfxs/coins/Coins In Sack Dropped on Wood.wav"},
        {SFX_DAMAGE, "audio/sfxs/damage/Injured.wav"},
        {SFX_ACHIEVEMENT, "audio/sfxs/achievement/Harmonized Tone_Pleasant but Spooky.wav"},
    };

    for (struct sfx_bindings *b = sfx_bindings; b < sfx_bindings + SFX_N; ++b) {
        effects[b->track] = Mix_LoadWAV(b->path);
        if (!effects[b->track])
            print_error("SFX [%s] loading error: %s", b->path, Mix_GetError());
    }

}

void audio_bus_play_music(MusicTrack track, bool loop)
{
    if (!musics[track]) {
        print_error("Music [%d] not loaded", track);
        return;
    }
    // -1 = loop infinito
    if (Mix_PlayMusic(musics[track], -1 * (int)loop) == -1)
        print_error("Music [%d] playing error: %s", track, Mix_GetError());
}

void audio_bus_stop_music()
{
    Mix_HaltMusic();
}


/*void audio_bus_play_sfx(SfxTrack track)
{
    if (!effects[track]) {
        print_error("SFX [%d] not loaded", track);
        return;
    }
    if (Mix_PlayChannel(-1, effects[track], 0) == -1)
        print_error("SFX [%d] playing error: %s", track, Mix_GetError());
}*/

void audio_bus_play_sfx(SfxTrack track)
{
    if (!effects[track]) {
        print_error("SFX [%d] not loaded", track);
        return;
    }
    int channel = Mix_PlayChannel(-1, effects[track], 0);
    if (channel == -1) {
        print_error("SFX [%d] playing error: %s", track, Mix_GetError());
        // Pode-se optar por parar o som menos importante ou ignorar, dependendo da lógica
    }
}

static bool active_channels[8]; // all channels inactive

void audio_bus_play_channel(SfxTrack track, int channel)
{
    active_channels[channel] = true;
    if (!effects[track]) {
        print_error("SFX [%d] not loaded", track);
        return;
    }
    //if (Mix_PlayChannel(channel, effects[track], 0) == -1)
    //    print_error("SFX [%d] playing error on channel %d: %s", track, Mix_GetError(), channel);
    if (Mix_PlayChannel(channel, effects[track], 0) == -1)
    print_error("SFX [%d] playing error on channel %d: %s", track, channel, Mix_GetError());

}

void audio_bus_stop_all_channels()
{
    for (bool* c = active_channels; c < active_channels + sizeof(active_channels) / sizeof(bool); ++c)
        if (*c) {
            audio_bus_stop_channel(c - active_channels);
            *c = false;
        }
}

void audio_bus_stop_channel(int channel)
{
    Mix_HaltChannel(channel);
}

void audio_bus_close()
{
    for (Mix_Music** track_ptr = musics; track_ptr < musics + MUSIC_N; ++track_ptr)
        Mix_FreeMusic(*track_ptr);
    for (Mix_Chunk** track_ptr = effects; track_ptr < effects + SFX_N; ++track_ptr)
        Mix_FreeChunk(*track_ptr);
    Mix_CloseAudio();
}
