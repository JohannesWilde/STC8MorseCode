#include "../color.h"


#define HUE_PERIOD_UINT16 (((uint16_t)(65535)) / 15)


uint16_t nextMajorHue(uint16_t const hue)
{
    return ((hue / HUE_PERIOD_UINT16) + 1) * HUE_PERIOD_UINT16;
}


ExtendedColor hueToRgbw(uint16_t hue)
{
    ExtendedColor newColor;

    uint16_t const hueBase = hue / HUE_PERIOD_UINT16;
    uint16_t const hueDiff = (hue - hueBase * HUE_PERIOD_UINT16);
    uint8_t const integerDiff = (uint8_t)(hueDiff * 15 / 257); // 255 / (65535 / 15) = 15 / 257
    uint8_t const inverseIntegerDiff = 255 - integerDiff;

    switch ((uint8_t)hueBase)
    {
    case 15:
        // fall through
    case 0:
    {
        newColor.red = 255;
        newColor.green = inverseIntegerDiff;
        newColor.blue = 255;
        newColor.white = 0;
        break;
    }
    case 1:
    {
        newColor.red = inverseIntegerDiff;
        newColor.green = 0;
        newColor.blue = 255;
        newColor.white = 0;
        break;
    }
    case 2:
    {
        newColor.red = 0;
        newColor.green = integerDiff;
        newColor.blue = 255;
        newColor.white = 0;
        break;
    }
    case 3:
    {
        newColor.red = 0;
        newColor.green = 255;
        newColor.blue = inverseIntegerDiff;
        newColor.white = 0;
        break;
    }
    case 4:
    {
        newColor.red = integerDiff;
        newColor.green = 255;
        newColor.blue = 0;
        newColor.white = 0;
        break;
    }
    case 5:
    {
        newColor.red = 255;
        newColor.green = inverseIntegerDiff;
        newColor.blue = 0;
        newColor.white = 0;
        break;
    }
    case 6:
    {
        newColor.red = inverseIntegerDiff;
        newColor.green = 0;
        newColor.blue = 0;
        newColor.white = integerDiff;
        break;
    }
    case 7:
    {
        newColor.red = integerDiff;
        newColor.green = integerDiff;
        newColor.blue = integerDiff;
        newColor.white = 255;
        break;
    }
    case 8:
    {
        newColor.red = 255;
        newColor.green = inverseIntegerDiff;
        newColor.blue = 255;
        newColor.white = 255;
        break;
    }
    case 9:
    {
        newColor.red = inverseIntegerDiff;
        newColor.green = 0;
        newColor.blue = 255;
        newColor.white = 255;
        break;
    }
    case 10:
    {
        newColor.red = 0;
        newColor.green = integerDiff;
        newColor.blue = 255;
        newColor.white = 255;
        break;
    }
    case 11:
    {
        newColor.red = 0;
        newColor.green = 255;
        newColor.blue = inverseIntegerDiff;
        newColor.white = 255;
        break;
    }
    case 12:
    {
        newColor.red = integerDiff;
        newColor.green = 255;
        newColor.blue = 0;
        newColor.white = 255;
        break;
    }
    case 13:
    {
        newColor.red = 255;
        newColor.green = inverseIntegerDiff;
        newColor.blue = 0;
        newColor.white = 255;
        break;
    }
    case 14:
    {
        newColor.red = 255;
        newColor.green = integerDiff;
        newColor.blue = integerDiff;
        newColor.white = inverseIntegerDiff;
        break;
    }
    default:
    {
        newColor.red = 0;
        newColor.green = 0;
        newColor.blue = 0;
        newColor.white = 0;
        break;
    }
    }

    return newColor;
}
