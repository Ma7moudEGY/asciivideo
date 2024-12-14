#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "pixel.h"
#include "helpers.h"

// create a new pixel
pixel *pixel_new(uint16_t red, uint16_t green, uint16_t blue)
{
    pixel *newpixel = (pixel *)malloc(sizeof(pixel)); // allocate memory using malloc function for new pixel with using red and green and blue colours

    if (newpixel == NULL) // if memory allocation fails then return to NULL
    {
        return NULL;
    }

    newpixel->rgb[0] = red;   // set red colour
    newpixel->rgb[1] = green; // set green colour
    newpixel->rgb[2] = blue;  // set blue colour

    return newpixel; // return pixel
}

// getting intensity of a specific pixel
uint16_t pixel_intensity(pixel px)
{
    uint16_t intensity; // defining intensity in  this function as local

    intensity = (0.299 * px.rgb[0] + 0.587 * px.rgb[1] + 0.114 * px.rgb[2]); // getting intensity using red, green and blue colours

    return intensity; // return intensity as a value
}

// converting each pixel into ascii characters
char pixel_to_ascii(pixel px)
{
    const char ASCII_CHARS[] = "@\%#*+=-:. \0";

    uint16_t intensity, index; // defining intensity and index in this function as locals

    intensity = pixel_intensity(px); // calculating intensity of the pixel

    uint16_t ascii_length = strlen(ASCII_CHARS) - 1; // defining Ascii length as the string length of ASCII characters minus 1

    index = (ascii_length)-intensity / 255.0 * (ascii_length); // calculating index using the formula

    return ASCII_CHARS[index]; // return ASCII characters of the index
}
