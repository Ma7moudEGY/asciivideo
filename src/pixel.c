#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "pixel.h"
#include "helpers.h"

// create a new pixel
pixel pixel_new(uint16_t red, uint16_t green, uint16_t blue)
{
    pixel newpixel;

    newpixel.r = red;   // set red colour
    newpixel.g = green; // set green colour
    newpixel.b = blue;  // set blue colour

    return newpixel;
}

// getting intensity of a specific pixel
uint16_t pixel_intensity(pixel px)
{
    uint16_t intensity;

    intensity = (0.299 * px.r + 0.587 * px.g + 0.114 * px.b);

    return intensity; // return intensity as a value
}

char pixel_to_ascii(pixel px)
{
    static char ASCII_CHARS[] = " .:-=+*%#@";

    uint16_t intensity, index;

    intensity = pixel_intensity(px); // calculating intensity of the pixel

    uint16_t ascii_length = strlen(ASCII_CHARS) - 1; // defining Ascii length as the string length of ASCII characters minus 1

    index = intensity / 255.0 * (ascii_length); // calculating index using the formula

    return ASCII_CHARS[index]; // return ASCII characters of the index
}
