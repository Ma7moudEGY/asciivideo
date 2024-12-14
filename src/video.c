#include <string.h>
#include "./video.h"
#include "helpers.h"

/**
 * Initialize new video
 */
video *video_new(const char *src, const char *out)
{
    video *vid = malloc(sizeof(video));

    vid->src = src;
    vid->out = out;
    vid->fmt_ctx = NULL;
    vid->codec = NULL;
    vid->codec_ctx = NULL;
    vid->video_stream = NULL;
    vid->audio_stream = NULL;
    vid->video_stream_index = -1;
    vid->audio_stream_index = -1;
    vid->pkt = NULL;
    vid->frame = NULL;

    if (avformat_open_input(&vid->fmt_ctx, vid->src, NULL, NULL) < 0)
    {
        ERROR("Could not open input file\n");
        goto fail;
    }

    if (avformat_find_stream_info(vid->fmt_ctx, NULL) < 0)
    {
        ERROR("Could not find stream information\n");
        goto fail;
    }

    int streams_status = video_set_streams(vid);

    switch (streams_status)
    {
    case -1:
        ERROR("Couldn't find video stream.");
        goto fail;
        break;
    case -2:
        WARN("Couldn't find audio stream.");
        break;
    default:
        break;
    }

    int decoder_status = video_find_decoder(vid);

    if (decoder_status == -1)
    {
        ERROR("Could not find the decoder");
        goto fail;
    }

    INFO("Decoder was found, dec={%s}, type={%u}", vid->codec->long_name, vid->codec_ctx->codec_type);
    vid->frame = av_frame_alloc();
    vid->pkt = av_packet_alloc();
    if (!vid->frame || !vid->pkt)
    {
        ERROR("Could not allocate frame or packet\n");
        goto fail;
    }

    return vid;

fail:
    ERROR("Couldn't create the video struct");
    return NULL;
}

/**
 * Find the stream index of the video.
 */

static int video_set_streams(video *vid)
{
    for (unsigned int i = 0; i < vid->fmt_ctx->nb_streams; i++)
    {
        if (vid->fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            vid->video_stream_index = i;
            vid->video_stream = vid->fmt_ctx->streams[i];
            INFO("video stream was found, i={%i}", vid->video_stream_index);
        }
        if (vid->fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        {
            vid->audio_stream_index = i;
            vid->audio_stream = vid->fmt_ctx->streams[i];
            INFO("audio stream was found, i={%i}", vid->audio_stream_index);
        }
    }

    if (vid->video_stream_index == -1)
    {
        return -1;
    }
    if (vid->audio_stream_index == -1)
    {
        return -2;
    }
    return 0;
}

/**
 * Find the decoder of the video
 */
static int video_find_decoder(video *vid)
{
    vid->codec = avcodec_find_decoder(vid->video_stream->codecpar->codec_id);
    if (vid->codec == NULL)
    {
        ERROR("Codec not found");
        return -1;
    }

    vid->codec_ctx = avcodec_alloc_context3(vid->codec);
    if (vid->codec_ctx == NULL)
    {
        ERROR("Could not allocate codec context");
        return -1;
    }

    // Copy codec parameters from the input stream to the codec context
    if (avcodec_parameters_to_context(vid->codec_ctx, vid->video_stream->codecpar) < 0)
    {
        ERROR("Could not copy codec parameters to context");
        return -1;
    }

    // Open codec
    if (avcodec_open2(vid->codec_ctx, vid->codec, NULL) < 0)
    {
        ERROR("Could not open codec");
        return -1;
    }
    else
    {
        INFO("Codec was opened");
    }

    return 0;
}

/**
 * Decoding video frames into struct frames (frames.h)
 */

int video_decode_frames(video *vid, int (*handler)(AVCodecContext *codec_ctx, AVFrame *frame, AVPacket *pkt, const char *out))
{

    // Read frames from the file
    while (av_read_frame(vid->fmt_ctx, vid->pkt) >= 0)
    {
        if (vid->pkt->stream_index == vid->video_stream_index)
        {
            if (handler(vid->codec_ctx, vid->frame, vid->pkt, vid->out) == -1)
            {
                FATAL("Couldn't handle the frame #%i", vid->codec_ctx->frame_num);
                goto fail;
            }
        }
        av_packet_unref(vid->pkt);
    }
    return 0;

fail:
    return -1;
}
/**
 * Clean up the video from the memory
 */
void video_clean_up(video *vid)
{
    av_frame_free(&vid->frame);
    av_packet_free(&vid->pkt);
    avcodec_free_context(&vid->codec_ctx);
    avformat_close_input(&vid->fmt_ctx);
    free(vid);
}