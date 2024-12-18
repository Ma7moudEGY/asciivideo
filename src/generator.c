#include <string.h>

#include "generator.h"
#include "video.h"
#include "frame.h"
#include "pixel.h"
#include "helpers.h"
#include "generator.h"

int decode_handler(AVCodecContext *dec_ctx, AVFrame *av_frame, AVPacket *av_pkt, AVFormatContext *av_fmt_ctx, AVStream *av_video_stream, const char *out_dir)
{
    char buf[1024];
    int ret;

    ret = avcodec_send_packet(dec_ctx, av_pkt);
    if (ret < 0)
    {
        ERROR("Error sending a packet for decoding, ret={%i}", ret);
        return -1;
    }

    AVRational framerate =
        av_guess_frame_rate(av_fmt_ctx, av_video_stream, av_frame);

    double fps = framerate.num / framerate.den;
    double duration = av_fmt_ctx->duration / (double)AV_TIME_BASE;

    int frames_count = fps * duration;

    while (ret >= 0)
    {
        ret = avcodec_receive_frame(dec_ctx, av_frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            goto done;
        else if (ret < 0)
        {
            ERROR("Error during decoding");
            return -1;
        }

        int frame_n = dec_ctx->frame_num;
        int w = av_frame->width, h = av_frame->height;
        snprintf(buf, sizeof(buf), "%s/%ld.ascii", out_dir, frame_n);

        INFO("(%.1f\%) Saving frame %ld/%ld: path={\"%s\"}, res={%ix%i}", frame_n / (float)frames_count * 100, frame_n, frames_count, buf, av_frame->width, av_frame->height);

        frame *fm = frame_new(frame_n, av_frame->data[0], av_frame->linesize[0], w, h);

        if (!fm)
        {
            ERROR("Creating frame#%i was failed", dec_ctx->frame_num);
        }

        int writing_status = frame_ascii_write_to(fm, buf);
        if (writing_status == -1)
        {
            ERROR("Couldn't write frame <%i> into %s", dec_ctx->frame_num, buf);
            return -1;
        }
        frame_free(fm);
    }

done:
    return 0;
}

/**
 * Creating a new video generator struct.
 */
ascii_video_gen *ascii_viden_gen_new(const char *src, const char *dest, video *vid)
{
    ascii_video_gen *gen = (ascii_video_gen *)malloc(sizeof(ascii_video_gen));

    gen->src = src;
    gen->dest = dest;
    gen->vid = vid;
    return gen;
}

/**
 * Running the generator.
 */
int ascii_video_gen_run(ascii_video_gen *gen)
{
    video_decode_frames(gen->vid, decode_handler);

    // Flush the decoder
    decode_handler(gen->vid->codec_ctx, gen->vid->frame, NULL, gen->vid->fmt_ctx, gen->vid->video_stream, gen->dest);
    return 0;
}
