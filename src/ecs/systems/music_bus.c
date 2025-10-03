#include "utils/print.h"

#include "music_bus.h"
#include <stdio.h>


Mix_Music *music;
//Mix_Chunk *effect;


void audio_bus_init()
{
    const char music_path[] = "0-drive.mp3";

    music = Mix_LoadMUS(music_path);
    if (!music)
        print_error("Music [%s] loading error: %s\n", music_path, Mix_GetError());

    //effect = Mix_LoadWAV("efeito.wav");

}

void audio_bus_play_music()
{
    if (!music)
        return;

    Mix_PlayMusic(music, -1); // -1 = loop infinito
    //Mix_PlayChannel(-1, effect, 0); // toca uma vez
}

void audio_bus_stop_music()
{
    Mix_HaltMusic();
}


void audio_bus_close()
{
    //Mix_FreeChunk(effect);
    Mix_FreeMusic(music);
    Mix_CloseAudio();
}
