#ifndef _VIDEO_H
#define _VIDEO_H

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
typedef struct
{
    const char *src, *out;
    AVFormatContext *fmt_ctx;
    const AVCodec *codec;
    AVCodecContext *codec_ctx;
    AVStream *video_stream;
    AVStream *audio_stream;
    int video_stream_index;
    int audio_stream_index;
    AVPacket *pkt;
    AVFrame *frame;
} video;

/**
 * Initialize new video
 */
video *video_new(const char *src, const char *out);

/**
 * Find the stream index of the video.
 */

static int video_set_streams(video *vid);

/**
 * Find the decoder of the video
 */
static int video_find_decoder(video *vid);

/**
 * Decoding video frames into struct frames (frames.h)
 */

int video_decode_frames(video *vid, int (*handler)(AVCodecContext *codec_ctx, AVFrame *frame, AVPacket *pkt, const char *out));

/**
 * Clean up the video from the memory
 */
void video_clean_up(video *vid);

#endif
