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

#define WAKEUP_TIMER_COUNT_LOOP ((F_WAKEUP_TIMER / /*F_SYS_TICK*/ 2000 / 16) - 1)
#define WAKEUP_TIMER_COUNT_MORSE ((F_WAKEUP_TIMER / F_SYS_TICK / 16) - 1)
#define LOOP_BRIGHTNESS_RAMP_UP_PRE_SCALER (2 - 1)
#define LOOP_BRIGHTNESS_COMPLET_PRE_SCALER (2 - 1)
#define MORSE_PRE_START_DELAY 7

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
    // use custom as per mode
    uint8_t counter;
    uint8_t counter2;
    // retain over all modes
    uint8_t brightness;
    uint16_t hue;
    uint16_t targetHue;
}
StatemachineData;

static __code uint16_t const hueYellow = 22676;
static __code uint16_t const hueViolet = 5482;

#define NEO_PIXEL_BRIGHTNESS_MAX 128

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
        COMPILE_TIME_ASSERT((1ull << 15) > WAKEUP_TIMER_COUNT_LOOP);
        WKTCL = WAKEUP_TIMER_COUNT_LOOP % 256;
        WKTCH = ((/*enabled*/ 1) << 7) | (0x7f & (WAKEUP_TIMER_COUNT_LOOP / 256));

        data->counter = 0;
        data->counter2 = LOOP_BRIGHTNESS_COMPLET_PRE_SCALER;

        if (10 > (data->targetHue - hueYellow))
        {
            data->targetHue = hueViolet;
        }
        else
        {
            data->targetHue = hueYellow;
        }

        break;
    }
    case StatemachineStageProcess:
    {
        // ramp up brightness
        if ((NEO_PIXEL_BRIGHTNESS_MAX > data->brightness))
        {
            if (updatePrescaler(&data->counter, LOOP_BRIGHTNESS_RAMP_UP_PRE_SCALER))
            {
                ++data->brightness;
            }
            else
            {
                // intentionally empty
            }
        }
        else
        {
            if (10 > (data->targetHue - data->hue))
            {
                if (updatePrescaler(&data->counter2, LOOP_BRIGHTNESS_COMPLET_PRE_SCALER))
                {
                    nextHandler = &statemachineHandlerMorse;
                }
                else
                {
                    // intentionally empty
                }
            }
            else
            {
                // intentionally empty
            }
        }

        data->hue += 10;

        ExtendedColor const color = hueToRgbw(data->hue);

        neoPixelData[0 * NEO_PIXEL_DATA_BYTES_PER_PIXEL + NEO_PIXEL_DATA_OFFSET_RED]    = color.red;
        neoPixelData[0 * NEO_PIXEL_DATA_BYTES_PER_PIXEL + NEO_PIXEL_DATA_OFFSET_GREEN]  = color.green;
        neoPixelData[0 * NEO_PIXEL_DATA_BYTES_PER_PIXEL + NEO_PIXEL_DATA_OFFSET_BLUE]   = color.blue;
        neoPixelData[0 * NEO_PIXEL_DATA_BYTES_PER_PIXEL + NEO_PIXEL_DATA_OFFSET_WHITE]  = color.white;
        show(neoPixelData,
             /*bytes*/ 1 * NEO_PIXEL_DATA_BYTES_PER_PIXEL,
             /*brightness*/ data->brightness);

        break;
    }
    case StatemachineStageDeinit:
    {
        COMPILE_TIME_ASSERT((1ull << 15) > WAKEUP_TIMER_COUNT_MORSE);
        WKTCL = WAKEUP_TIMER_COUNT_MORSE % 256;
        WKTCH = ((/*enabled*/ 1) << 7) | (0x7f & (WAKEUP_TIMER_COUNT_MORSE / 256));
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

        data->counter = MORSE_PRE_START_DELAY;
        LED_PIN = morseCodeSenderState.showingSignalAndNotPause;
        show(neoPixelData,
             /*bytes*/ 1 * NEO_PIXEL_DATA_BYTES_PER_PIXEL,
             /*brightness*/ morseCodeSenderState.showingSignalAndNotPause ? 128 : 0);
        break;
    }
    case StatemachineStageProcess:
    {
        if (0 < data->counter)
        {
            --data->counter;
        }
        else
        {
            MorseCodeSenderUpdateResult const result = morseCodeSenderStateUpdate();

            switch (result)
            {
            case morseCodeSenderUpdateResult_update:
            {
                LED_PIN = morseCodeSenderState.showingSignalAndNotPause;

                show(neoPixelData,
                     /*bytes*/ 1 * NEO_PIXEL_DATA_BYTES_PER_PIXEL,
                     /*brightness*/ morseCodeSenderState.showingSignalAndNotPause ? 128 : 0);

                break;
            }
            case morseCodeSenderUpdateResult_noUpdate:
            {
                // intentionally empty
                break;
            }
            case morseCodeSenderUpdateResult_end:
            {
                nextHandler = &statemachineHandlerLoopColors;
                break;
            }
            }
        }
        break;
    }
    case StatemachineStageDeinit:
    {
        data->brightness = 0; // I do end on a pause - so LED off.
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

    COMPILE_TIME_ASSERT((1ull << 15) > WAKEUP_TIMER_COUNT_MORSE);
    WKTCL = WAKEUP_TIMER_COUNT_MORSE % 256;
    WKTCH = ((/*enabled*/ 1) << 7) | (0x7f & (WAKEUP_TIMER_COUNT_MORSE / 256));

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
