#include "./frame.h"
#include "./helpers.h"
#include "./pixel.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string.h>

/*
 * Creating a new frame.
 */
frame *frame_new(uint64_t id, pixel pixels[], unsigned int width, unsigned int height)
{
    // Allocate memory for the frame structure and its pixel array
    frame *new_frame = (frame *)malloc(sizeof(frame) + sizeof(pixel) * width * height);
    if (!new_frame)
    {
        return NULL;
    }

    // Initialize the frame fields
    new_frame->id = id;
    new_frame->width = width;
    new_frame->height = height;

    memcpy(new_frame->pixels, pixels, sizeof(pixel) * width * height);

    return new_frame;
}

/*
 * Converting the frame into Ascii Art.
 */
char *frame_to_ascii(frame *frame)
{
    unsigned int width = frame->width;
    unsigned int height = frame->height;

    char *ascii_art = (char *)malloc(width * height + 1); // Extra space for newlines and '\0'
    if (!ascii_art)
    {
        return NULL; // Return NULL if memory allocation fails
    }

    for (unsigned int y = 0; y < height; y++)
    {
        for (unsigned int x = 0; x < width; x++)
        {
            pixel p = frame->pixels[y * width + x];
            char ascii_char = pixel_to_ascii(p);
            ascii_art[y + x] = ascii_char;
        }
        ascii_art[y + width] = '\n';
    }
    ascii_art[width + height] = '\0';

    return ascii_art;
}

/*
 * Writing the frame that converted into Ascii Art to the path of the
 * destination file.
 */
int frame_ascii_write_to(frame *frame, char *filename)
{
    char *ascii_art = frame_to_ascii(frame);
    if (!ascii_art)
    {
        return -1; // Return -1 if ASCII conversion fails
    }

    FILE *file = fopen(filename, "w");
    if (!file)
    {
        free(ascii_art);
        return -1;
    }

    fprintf(file, "%s", ascii_art);
    fclose(file);
    free(ascii_art);

    return 0; // Return 0 on success

}