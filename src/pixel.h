#ifndef _PIXEL_H
#define _PIXEL_H
#include <stdint.h>

// There are 10 levels of brightness. It depends on the intensity (brightness) of the pixel.

typedef struct
{
    uint16_t r;
    uint16_t g;
    uint16_t b;
} pixel;

/**
 * Creating a new pixel.
 */

pixel pixel_new(uint16_t red, uint16_t green, uint16_t blue);

/**
 * Getting the intensity level of a specific pixel.
 *
 * Formula:
 *   I = 0.299*R + 0.587*G + 0.114*B
 */

uint16_t pixel_intensity(pixel px);

/**
 * Converting the pixel to ascii character depends on its intensity (Grayscale value).
 *
 * You can find the index of the suitable ascii code using this formula:
 *  Index = I/255 * strlen(ASCII_CHARS)
 *
 * where I is the intensity of the pixel
 */

char pixel_to_ascii(pixel px);

#endif