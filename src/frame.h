#ifndef _FRAME_H
#define _FRAME_H

#include <stdint.h>
#include "pixel.h"

typedef struct
{
  uint64_t id;
  int wrap;
  int width;
  int height;
  unsigned char *buf;
} frame;

/*
 * Creating a new frame.
 */
frame *frame_new(uint64_t id, uint8_t *buf, int wrap, int width, int height);

/*
 * Writing the frame that converted into Ascii Art to the path of the
 * destination file.
 */
int frame_ascii_write_to(frame *frame, char *filename);

/**
 * Clean up the frame 
 */
void frame_free(frame *f);

#endif
