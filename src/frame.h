#ifndef _FRAME_H
#define _FRAME_H

#include <stdint.h>
#include "pixel.h"

typedef struct
{
  uint64_t id; // the ID of the frame, its order in the video
  uint32_t height;
  uint32_t width;
  pixel pixels[]; // Array of pixels for the frame.
} frame;

/*
 * Creating a new frame.
 */
frame *frame_new(uint64_t id, pixel pixels[], unsigned int width, unsigned int height);

/*
 * Converting the frame into Ascii Art.
 */
char *frame_to_ascii(frame *frame);

/*
 * Writing the frame that converted into Ascii Art to the path of the
 * destination file.
 */
int frame_ascii_write_to(frame *frame, char* filename);

#endif
