#ifndef _PLAYER_H
#define _PLAYER_H

#include "./specs.h"

typedef struct
{
    const char *src;
    specs *specs;
    pid_t audio_pid;
} player;

/**
 * Creatting a new video player struct.
 */
player *player_new(const char *src, specs *specs);

/**
 * Running the video player.
 */
int player_video_run(player *player);

/**
 * Running the audio player.
 */
int player_audio_run(player *player);

#endif