#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <unistd.h>
#define FRAME_DELAY 33333 // Microseconds (1/30 second)

static void ascii_save(unsigned char *buf, int wrap, int xsize, int ysize, char *filename)
{
    FILE *file = fopen(filename, "wb");
    const char *ascii_map = " .:-=+*%#@";
    int ascii_len = strlen(ascii_map);

    for (int y = 0; y < ysize; y++)
    {
        for (int x = 0; x < xsize; x++)
        {
            // Map pixel intensity (0-255) to an ASCII character
            unsigned char pixel_value = buf[y * wrap + x];
            char ascii_char = ascii_map[(pixel_value * (ascii_len - 1)) / 255];
            fputc(ascii_char, file);
            fputc(ascii_char, file);
            fputc(ascii_char, file);
            // printf("f");
        }
        fputc('\n', file); // New line for each row
    }

    // fwrite(file_buf, xsize * ysize, xsize * ysize, file);
    fclose(file);
}

static void player(const char *prefix)
{
    for (int i = 1; i <= 450; i++)
    {
        // chr buf[100];
        char filename[128];
        snprintf(filename, sizeof(filename), "%s-%i.ascii", prefix, i);
        FILE *file = fopen(filename, "r");
        printf("\t\t\t");
        char ch;
        while ((ch = fgetc(file)) != EOF)
        {
            printf("%c", ch);
        }
        usleep(FRAME_DELAY);
        fflush(stdout);
    }
}

static void decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt, const char *filename)
{
    char buf[1024];
    int ret;

    ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0)
    {
        fprintf(stderr, "Error sending a packet for decoding\n");
        exit(1);
    }

    while (ret >= 0)
    {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0)
        {
            fprintf(stderr, "Error during decoding\n");
            exit(1);
        }

        printf("Saving frame %ld: %ix%i\n", dec_ctx->frame_num, frame->width, frame->height);
        snprintf(buf, sizeof(buf), "%s-%ld.ascii", filename, dec_ctx->frame_num);

        ascii_save(frame->data[0], frame->linesize[0], frame->width, frame->height, buf);
        // usleep(FRAME_DELAY);
    }
    printf("Decoding is done\n");
}

int main(int argc, char **argv)
{
    const char *filename, *outfilename;
    AVFormatContext *fmt_ctx = NULL;
    const AVCodec *codec;
    AVCodecContext *codec_ctx = NULL;
    AVStream *video_stream = NULL;
    int video_stream_index = -1;
    AVPacket *pkt = NULL;
    AVFrame *frame = NULL;

    if (argc <= 2)
    {
        fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
        exit(0);
    }
    filename = argv[1];
    outfilename = argv[2];

    // Open the input file
    if (avformat_open_input(&fmt_ctx, filename, NULL, NULL) < 0)
    {
        fprintf(stderr, "Could not open input file\n");
        exit(1);
    }

    // Find stream information
    if (avformat_find_stream_info(fmt_ctx, NULL) < 0)
    {
        fprintf(stderr, "Could not find stream information\n");
        exit(1);
    }

    // Find the video stream
    for (unsigned int i = 0; i < fmt_ctx->nb_streams; i++)
    {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            video_stream_index = i;
            video_stream = fmt_ctx->streams[i];
            break;
        }
    }
    if (video_stream_index == -1)
    {
        fprintf(stderr, "Could not find a video stream\n");
        exit(1);
    }

    // Find the decoder for the video stream
    codec = avcodec_find_decoder(video_stream->codecpar->codec_id);
    if (!codec)
    {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }

    // Allocate codec context
    codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx)
    {
        fprintf(stderr, "Could not allocate codec context\n");
        exit(1);
    }

    // Copy codec parameters from the input stream to the codec context
    if (avcodec_parameters_to_context(codec_ctx, video_stream->codecpar) < 0)
    {
        fprintf(stderr, "Could not copy codec parameters to context\n");
        exit(1);
    }

    // Open codec
    if (avcodec_open2(codec_ctx, codec, NULL) < 0)
    {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    // Allocate frame and packet
    frame = av_frame_alloc();
    pkt = av_packet_alloc();
    if (!frame || !pkt)
    {
        fprintf(stderr, "Could not allocate frame or packet\n");
        exit(1);
    }

    // Read frames from the file
    while (av_read_frame(fmt_ctx, pkt) >= 0)
    {
        if (pkt->stream_index == video_stream_index)
        {
            decode(codec_ctx, frame, pkt, outfilename);
        }
        av_packet_unref(pkt);
    }

    // Flush the decoder
    decode(codec_ctx, frame, NULL, outfilename);

    player(outfilename);
    // Clean up
    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&fmt_ctx);

    return 0;
}
