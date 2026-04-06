#include "8051_helpers.h"
#include "color.h"
#include "configuration.h"
#include "morsecode.h"
#include "nelems.h"
#include "pinout.h"
#include "prescaler.h"
#include "statemachine.h"
#include "static_assert.h"
#include "stc8g.h"
#include "ws2812.h"

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


#define xstr(s) str(s)
#define str(s) #s

#define LED_PIN MAKE_PIN_NAME(LED_PORT_NUMBER, LED_PIN_NUMBER)
#define NEO_PIXEL_PIN MAKE_PIN_NAME(NEO_PIXEL_PORT_NUMBER, NEO_PIXEL_PIN_NUMBER)


#define NEO_PIXEL_DATA_BYTES_PER_PIXEL 4
#define NEO_PIXEL_DATA_OFFSET_RED 1
#define NEO_PIXEL_DATA_OFFSET_GREEN 0
#define NEO_PIXEL_DATA_OFFSET_BLUE 2
#define NEO_PIXEL_DATA_OFFSET_WHITE 3

static uint8_t neoPixelData[1 * NEO_PIXEL_DATA_BYTES_PER_PIXEL];





__code const MorseCodeSymbolIndex morseCodeText[] = {
    #include MORSE_CODE_TEXT_SOURCE_FILE
};
MorseCodeSenderState morseCodeSenderState;

#include "morsecode.c"





// Statemachine

typedef struct
{
    uint8_t brightness;
    uint16_t hue;
}
StatemachineData;

static StatemachineData statemachineData;
static Statemachine statemachine;


FunctionPointerPrototype statemachineHandlerLoopColors(StatemachineStage stage, void * data);
FunctionPointerPrototype statemachineHandlerMorse(StatemachineStage stage, void * data);


FunctionPointerPrototype statemachineHandlerLoopColors(StatemachineStage stage, void * const untypedData)
{
    StatemachineData * const data = (StatemachineData *)untypedData;
    StatemachineHandler nextHandler = &statemachineHandlerLoopColors;
    switch (stage)
    {
    case StatemachineStageInit:
    {

        neoPixelData[0 * NEO_PIXEL_DATA_BYTES_PER_PIXEL + NEO_PIXEL_DATA_OFFSET_RED]    = yellow.red;
        neoPixelData[0 * NEO_PIXEL_DATA_BYTES_PER_PIXEL + NEO_PIXEL_DATA_OFFSET_GREEN]  = yellow.green;
        neoPixelData[0 * NEO_PIXEL_DATA_BYTES_PER_PIXEL + NEO_PIXEL_DATA_OFFSET_BLUE]   = yellow.blue;
        // neoPixelData[0 * NEO_PIXEL_DATA_BYTES_PER_PIXEL + NEO_PIXEL_DATA_OFFSET_WHITE]  = yellow.white;
        show(neoPixelData,
             /*bytes*/ 1 * NEO_PIXEL_DATA_BYTES_PER_PIXEL,
             /*brightness*/ 128);

        break;
    }
    case StatemachineStageProcess:
    {
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

        StatemachineHandler nextHandler = &statemachineHandlerMorse;

        break;
    }
    case StatemachineStageDeinit:
    {
        // intentionally empty
        break;
    }
    }
    return (FunctionPointerPrototype)nextHandler;
}

FunctionPointerPrototype statemachineHandlerMorse(StatemachineStage stage, void * const untypedData)
{
    StatemachineData * const data = (StatemachineData *)untypedData;
    StatemachineHandler nextHandler = &statemachineHandlerMorse;
    switch (stage)
    {
    case StatemachineStageInit:
    {
        morseCodeSenderStateInit();
        break;
    }
    case StatemachineStageProcess:
    {
        if (morseCodeSenderStateUpdate())
        {
            LED_PIN = morseCodeSenderState.showingSignalAndNotPause;

            show(neoPixelData,
                 /*bytes*/ 1 * NEO_PIXEL_DATA_BYTES_PER_PIXEL,
                 /*brightness*/ morseCodeSenderState.showingSignalAndNotPause ? 128 : 0);
        }
        else
        {
            // intentionally empty
        }
        break;
    }
    case StatemachineStageDeinit:
    {
        // intentionally empty
        break;
    }
    }
    return (FunctionPointerPrototype)nextHandler;
}



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

    statemachineInit(&statemachine, &statemachineHandlerLoopColors);

    while (true)
    {
        statemachineProcess(&statemachine, &statemachineData);

        PCON |= 0x02;  // PCON.PD = 1 - Enter power-down mode
        SFRX_ON();
        while (!(HIRCCR & 0x01));
        SFRX_OFF();
    }
}
