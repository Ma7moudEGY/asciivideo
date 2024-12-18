#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "frame.h"
#include "helpers.h"
#include "pixel.h"
/*
 * Creating a new frame.
 */
frame *frame_new(uint64_t id, uint8_t *buf, int wrap, int width, int height)
{
    if (width <= 0 || height <= 0)
    {
        ERROR("Invalid width or height: width=%d, height=%d", width, height);
        return NULL;
    }
    if (buf == NULL)
    {
        ERROR("buf is null");
        return NULL;
    }
    frame *new_frame = (frame *)malloc(sizeof(frame));
    if (!new_frame)
    {
        perror("malloc failed"); // Use perror for system error messages
        return NULL;
    }
    // Initialize the frame fields
    new_frame->id = id;
    new_frame->width = width;
    new_frame->height = height;
    new_frame->wrap = wrap;

    size_t buf_size = malloc_usable_size(buf);

    new_frame->buf = (uint8_t *)malloc(buf_size);
    // new_frame->pixels = pixels;

    if (!new_frame->buf)
    {
        ERROR("Couldn't allocate memory for frame buffer: frame_id={%i}, size={%lu}", id, buf_size);
        return NULL;
    }

    memcpy(new_frame->buf, buf, buf_size);
    return new_frame;
}

/*
 * Writing the frame that converted into Ascii Art to the path of the
 * destination file.
 */
int frame_ascii_write_to(frame *frame, char *filename)
{
    FILE *file = fopen(filename, "wb");

    for (int y = 0; y < frame->height; y++)
    {
        for (int x = 0; x < frame->width; x++)
        {
            pixel px = pixel_new(
                frame->buf[y * frame->wrap + x],
                frame->buf[y * frame->wrap + x + 1],
                frame->buf[y * frame->wrap + x + 2]);
            char ascii_char = pixel_to_ascii(px);

            fputc(ascii_char, file);
            fputc(ascii_char, file);
            fputc(ascii_char, file);
            // printf("f");
        }
        fputc('\n', file);
    }

    fclose(file);
}

void frame_free(frame *frame)
{
    free(frame->buf);
    free(frame);
}