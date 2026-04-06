#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>


typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
}
Color;

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t white;
}
ExtendedColor;


ExtendedColor hueToRgbw(uint16_t hue);


extern __code Color const violet;
extern __code Color const yellow;


#endif // COLOR_H
