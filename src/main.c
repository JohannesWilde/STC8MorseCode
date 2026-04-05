#include "8051_helpers.h"
#include "configuration.h"
#include "pinout.h"
#include "prescaler.h"
#include "static_assert.h"
#include "stc8g.h"
#include "ws2812.h"

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))


#define LED_PIN MAKE_PIN_NAME(LED_PORT_NUMBER, LED_PIN_NUMBER)
#define NEO_PIXEL_PIN MAKE_PIN_NAME(NEO_PIXEL_PORT_NUMBER, NEO_PIXEL_PIN_NUMBER)


#define NEO_PIXEL_DATA_BYTES_PER_PIXEL 4
#define NEO_PIXEL_DATA_OFFSET_RED 1
#define NEO_PIXEL_DATA_OFFSET_GREEN 0
#define NEO_PIXEL_DATA_OFFSET_BLUE 2
#define NEO_PIXEL_DATA_OFFSET_WHITE 3

static uint8_t neoPixelData[1 * NEO_PIXEL_DATA_BYTES_PER_PIXEL];




#define BRIGHTNESS_DELTA_STEP 1

static uint8_t colorBrightness;
static uint8_t colorDelta = BRIGHTNESS_DELTA_STEP;


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


__code ExtendedColor const violet = {
    .red = 189,
    .green = 0,
    .blue = 206,
    .white = 0
};

__code ExtendedColor const yellow = {
    .red = 255,
    .green = 200,
    .blue = 0,
    .white = 0
};


typedef enum
{
    morseCodeSignal_dit,
    morseCodeSignal_dah
}
MorseCodeSignal;

inline uint8_t morseCodeSignalToDuration(MorseCodeSignal const signal)
{
    switch (signal)
    {
    case morseCodeSignal_dit: return 1;
    case morseCodeSignal_dah: return 3;
    }
    return 0xff;
}

typedef struct
{
    uint8_t content;
    uint8_t length;     // in bits
}
MorseCodeSymbol;

typedef enum
{
    morseCodeSymbolIndex_0,
    morseCodeSymbolIndex_1,
    morseCodeSymbolIndex_2,
    morseCodeSymbolIndex_3,
    morseCodeSymbolIndex_4,
    morseCodeSymbolIndex_5,
    morseCodeSymbolIndex_6,
    morseCodeSymbolIndex_7,
    morseCodeSymbolIndex_8,
    morseCodeSymbolIndex_9,
    morseCodeSymbolIndex_space,
}
MorseCodeSymbolIndex;

// MSb transmitted first, each bit as per MorseCodeSignal.
__code MorseCodeSymbol const morseCodeSymbols[] = {
    /* 0 */     {.content = 0b11111000, .length = 5},
    /* 1 */     {.content = 0b01111000, .length = 5},
    /* 2 */     {.content = 0b00111000, .length = 5},
    /* 3 */     {.content = 0b00011000, .length = 5},
    /* 4 */     {.content = 0b00001000, .length = 5},
    /* 5 */     {.content = 0b00000000, .length = 5},
    /* 6 */     {.content = 0b10000000, .length = 5},
    /* 7 */     {.content = 0b11000000, .length = 5},
    /* 8 */     {.content = 0b11100000, .length = 5},
    /* 9 */     {.content = 0b11110000, .length = 5},
    /* space */ {.content = 0b00000000, .length = 0},
};

__code const MorseCodeSymbolIndex text[] = {
    morseCodeSymbolIndex_5, morseCodeSymbolIndex_5, /*morseCodeSymbolIndex_1, morseCodeSymbolIndex_2, morseCodeSymbolIndex_3, */morseCodeSymbolIndex_space,
};


typedef struct
{
    MorseCodeSymbolIndex const * currentSymbol;
    MorseCodeSymbol currentSymbolWorkingCopy;
    uint8_t durationTillNextSignal;
    bool showingSignalAndNotPause;
}
MorseCodeSenderState;

static MorseCodeSenderState morseCodeSenderState;


void main()
{
    LED_PIN = 0;
    NEO_PIXEL_PIN = 0;

    COMPILE_TIME_ASSERT(1 == LED_PORT_NUMBER);
    P1M0 = (0x00 /* DIO_MODE_HIGH_Z_INPUT_M0 */) |
           (DIO_MODE_PUSH_PULL_OUTPUT_M0 << LED_PIN_NUMBER);
    P1M1 = (0xff /* DIO_MODE_HIGH_Z_INPUT_M1 */) &
           ((DIO_MODE_PUSH_PULL_OUTPUT_M1 << LED_PIN_NUMBER) | ~(1 << LED_PIN_NUMBER));

    COMPILE_TIME_ASSERT(5 == NEO_PIXEL_PORT_NUMBER);
    P5M0 = (0x00 /* DIO_MODE_HIGH_Z_INPUT_M0 */) |
           (DIO_MODE_PUSH_PULL_OUTPUT_M0 << NEO_PIXEL_PIN_NUMBER);
    P5M1 = (0xff /* DIO_MODE_HIGH_Z_INPUT_M1 */) &
           ((DIO_MODE_PUSH_PULL_OUTPUT_M1 << NEO_PIXEL_PIN_NUMBER) | ~(1 << NEO_PIXEL_PIN_NUMBER));


    COMPILE_TIME_ASSERT(0 < CLOCK_DIVISOR);
    #if 1 < CLOCK_DIVISOR
        SFRX_ON();
        CLKDIV = CLOCK_DIVISOR;
        while (!(HIRCCR & 0x01));
        SFRX_OFF();
    #endif

    #define WAKEUP_TIMER_COUNT ((F_WAKEUP_TIMER / F_SYS_TICK / 16) - 1)
    COMPILE_TIME_ASSERT((1ull << 15) > WAKEUP_TIMER_COUNT);
    WKTCL = WAKEUP_TIMER_COUNT % 256;
    WKTCH = ((/*enabled*/ 1) << 7) | (0x7f & (WAKEUP_TIMER_COUNT / 256));

    interrupts(); // enable interrupts

    COMPILE_TIME_ASSERT(1 < NELEMS(text));
    morseCodeSenderState.currentSymbol = &text[0];
    morseCodeSenderState.currentSymbolWorkingCopy.content = morseCodeSymbols[*morseCodeSenderState.currentSymbol].content;
    morseCodeSenderState.currentSymbolWorkingCopy.length = morseCodeSymbols[*morseCodeSenderState.currentSymbol].length;
    morseCodeSenderState.durationTillNextSignal = 1; // allow pre-decrement
    // morseCodeSenderState.showingSignalAndNotPause = false;

    neoPixelData[0 * NEO_PIXEL_DATA_BYTES_PER_PIXEL + NEO_PIXEL_DATA_OFFSET_RED]    = yellow.red;
    neoPixelData[0 * NEO_PIXEL_DATA_BYTES_PER_PIXEL + NEO_PIXEL_DATA_OFFSET_GREEN]  = yellow.green;
    neoPixelData[0 * NEO_PIXEL_DATA_BYTES_PER_PIXEL + NEO_PIXEL_DATA_OFFSET_BLUE]   = yellow.blue;
    // neoPixelData[0 * NEO_PIXEL_DATA_BYTES_PER_PIXEL + NEO_PIXEL_DATA_OFFSET_WHITE]  = yellow.white;

    while (true)
    {
        // Note another cycle passed.
        --morseCodeSenderState.durationTillNextSignal;
        if (0 == morseCodeSenderState.durationTillNextSignal)
        {
            // Signal duration over - check for next signal.
            bool const lastSymbolFinished = (0 == morseCodeSenderState.currentSymbolWorkingCopy.length);

            // Load next symbol if so required.
            if (lastSymbolFinished)
            {
                // Load next symbol to show after this pause.
                ++morseCodeSenderState.currentSymbol;
                if ((text + NELEMS(text)) ==  morseCodeSenderState.currentSymbol)
                {
                    // Loop back to front of text.
                    morseCodeSenderState.currentSymbol = &text[0];
                }
                else
                {
                    // intentionally empty
                }

                MorseCodeSymbolIndex const nextSymbolIndex = *morseCodeSenderState.currentSymbol;

                morseCodeSenderState.currentSymbolWorkingCopy.content = morseCodeSymbols[nextSymbolIndex].content;
                morseCodeSenderState.currentSymbolWorkingCopy.length = morseCodeSymbols[nextSymbolIndex].length;
            }
            else
            {
                // intentionally empty
            }

            // Determine next signal.
            if (morseCodeSymbolIndex_space == *morseCodeSenderState.currentSymbol)
            {
                // End of word.
                morseCodeSenderState.durationTillNextSignal = 7;
                morseCodeSenderState.showingSignalAndNotPause = false;
            }
            else if (morseCodeSenderState.showingSignalAndNotPause)
            {
                // Previously content, so determine length of pause.
                if (lastSymbolFinished)
                {
                    // End of symbol.
                    morseCodeSenderState.durationTillNextSignal = 3;
                }
                else
                {
                    // Inside symbol.
                    morseCodeSenderState.durationTillNextSignal = 1;
                }
                morseCodeSenderState.showingSignalAndNotPause = false;
            }
            else // if (!morseCodeSenderState.showingSignalAndNotPause)
            {
                // Previously pause, so look for next content.
#ifndef NDEBUG
                // Next symbol must be loaded before a pause above [as to correctly handle morseCodeSymbolIndex_space].
                while (0 == morseCodeSenderState.currentSymbolWorkingCopy.length);
#endif // NDEBUG

                // Use MSb of currentSymbolWorkingCopy next.
                MorseCodeSignal const nextSignal = (morseCodeSenderState.currentSymbolWorkingCopy.content & 0x80);
                // Advance symbol content.
                morseCodeSenderState.currentSymbolWorkingCopy.content <<= 1;
                morseCodeSenderState.currentSymbolWorkingCopy.length -= 1;
                // Apply nextSignal.
                morseCodeSenderState.durationTillNextSignal = morseCodeSignalToDuration(nextSignal);
                morseCodeSenderState.showingSignalAndNotPause = true;
            }




            LED_PIN = morseCodeSenderState.showingSignalAndNotPause;

            show(neoPixelData,
                 /*bytes*/ 1 * NEO_PIXEL_DATA_BYTES_PER_PIXEL,
                 /*brightness*/ morseCodeSenderState.showingSignalAndNotPause ? 255 : 0);
        }
        else
        {
            // intentionally empty
        }


        // if (BRIGHTNESS_DELTA_STEP > colorBrightness)
        // {
        //     colorDelta = BRIGHTNESS_DELTA_STEP;

        //     neoPixelData[0 * NEO_PIXEL_DATA_BYTES_PER_PIXEL + NEO_PIXEL_DATA_OFFSET_RED]    = violet.red;
        //     neoPixelData[0 * NEO_PIXEL_DATA_BYTES_PER_PIXEL + NEO_PIXEL_DATA_OFFSET_GREEN]  = violet.green;
        //     neoPixelData[0 * NEO_PIXEL_DATA_BYTES_PER_PIXEL + NEO_PIXEL_DATA_OFFSET_BLUE]   = violet.blue;
        //     // neoPixelData[0 * NEO_PIXEL_DATA_BYTES_PER_PIXEL + NEO_PIXEL_DATA_OFFSET_WHITE]  = violet.white;
        // }
        // else if ((255 - BRIGHTNESS_DELTA_STEP) < colorBrightness)
        // {
        //     colorDelta = (uint8_t)(-BRIGHTNESS_DELTA_STEP);

        //     neoPixelData[0 * NEO_PIXEL_DATA_BYTES_PER_PIXEL + NEO_PIXEL_DATA_OFFSET_RED]    = yellow.red;
        //     neoPixelData[0 * NEO_PIXEL_DATA_BYTES_PER_PIXEL + NEO_PIXEL_DATA_OFFSET_GREEN]  = yellow.green;
        //     neoPixelData[0 * NEO_PIXEL_DATA_BYTES_PER_PIXEL + NEO_PIXEL_DATA_OFFSET_BLUE]   = yellow.blue;
        //     // neoPixelData[0 * NEO_PIXEL_DATA_BYTES_PER_PIXEL + NEO_PIXEL_DATA_OFFSET_WHITE]  = yellow.white;
        // }
        // else
        // {
        //     // intentionally empty
        // }


        PCON |= 0x02;  // PCON.PD = 1 - Enter power-down mode
        SFRX_ON();
        while (!(HIRCCR & 0x01));
        SFRX_OFF();
    }
}
