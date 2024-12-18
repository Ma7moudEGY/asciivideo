#define __USE_MISC
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <ncurses.h>
#include <pthread.h>
#include <math.h>

#include "generator.h"
#include "player.h"
#include "video.h"
#include "pixel.h"
#include "helpers.h"
#include "specs.h"
#include "frame.h"

void *audio_routine(void *p)
{
    return (void *)player_audio_run((player *)p);
}
void *video_routine(void *p)
{
    return (void *)player_video_run((player *)p);
}

int main(int argc, char **argv)
{
    const char *src, *out;

    if (argc <= 2)
    {
        fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
        exit(0);
    }
    src = argv[1];
    out = argv[2];

    video *vid = video_new(src, out);

    ascii_video_gen *gen = ascii_viden_gen_new(src, out, vid);
    ascii_video_gen_run(gen);

    char buf[1024];
    snprintf(buf, sizeof(buf), "%s/audio.wav", out);
    int audio_ext_ret = video_extract_audio(vid, buf);
    if (audio_ext_ret != 0)
    {
        ERROR("Couldn't extract the audio and play it.");
    }
    AVRational framerate =
        av_guess_frame_rate(vid->fmt_ctx, vid->video_stream, vid->frame);

    double fps = framerate.num / framerate.den;
    double duration = vid->fmt_ctx->duration / (double)AV_TIME_BASE;

    int frames_count = fps * duration;
    specs *specs = specs_new(frames_count, fps, duration, vid->video_stream->codecpar->width, vid->video_stream->codecpar->height, audio_ext_ret == 0 ? true : false);

    INFO("Creating specs file, content={%s}", specs_serialize(specs));
    initscr();
    noecho();
    curs_set(0);
    INFO("w=%i, h=%i", vid->codec_ctx->width, vid->codec_ctx->height);

    player *player = player_new(out, specs);

    pthread_t th_video;
    pthread_t th_audio;

    void *ret_audio, *ret_video;

    pthread_create(&th_video, NULL, video_routine, (void *)player);
    audio_ext_ret == 0 &&
        pthread_create(&th_audio, NULL, audio_routine, (void *)player);

    pthread_join(th_video, &ret_video);

    audio_ext_ret == 0 &&
        pthread_join(th_audio, &ret_audio);

    if ((int)ret_video == -1)
    {
        ERROR("player failed to play the video");
    }

    endwin();
    video_clean_up(vid);

    return 0;
}