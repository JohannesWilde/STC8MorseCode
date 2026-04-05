#include "ws2812.h"

#include "configuration.h"
#include "static_assert.h"
#include "stc8g.h"
#include "unused.h"


#define MAKE_ASM_PIN_NAME_(port, pin) "_P" #port "_" #pin
#define MAKE_ASM_PIN_NAME(port, pin) MAKE_ASM_PIN_NAME_(port, pin)

#define NEO_PIXEL_ASM_PIN_STRING MAKE_ASM_PIN_NAME(NEO_PIXEL_PORT_NUMBER, NEO_PIXEL_PIN_NUMBER)


void show(uint8_t const * data, uint8_t const length, uint8_t const brightness) __reentrant
{
    for (uint8_t byteIndex = length; 0 < byteIndex; --byteIndex)
    {
        uint8_t datum = (((uint16_t)(*data)) * brightness) >> 8;
        ++data;

        for (uint8_t bitIndex = 8; 0 < bitIndex; --bitIndex)
        {
            uint8_t const bit = 0x80 & datum;
            datum <<= 1;
            if (bit)
            {
                __asm__ (
                // bit "1" transmission
                "    push ar0\n"
                // HIGH
                "    setb    " NEO_PIXEL_ASM_PIN_STRING "\n"    //                      2 [1]             1
                "    mov r0,#6\n"        // 6 = 5 [jump] + 1 [not jump]                 2 [1]             2
                "001$:\n"
                "    djnz r0,001$\n"     // Decrement register and jump if not Zero     2 [2/3]          19
                // LOW
                "    clr    " NEO_PIXEL_ASM_PIN_STRING "\n"     //                      2 [1]             _1
                "    mov r0,#3\n"      // 3 = 2 [jump] + 1 [not jump]                   2 [1]             _2
                "002$:\n"
                "    djnz r0,002$\n"  // Decrement register and jump if not Zero        2 [2/3]           _10
                // "    nop    \n"       //                                                                  _11
                "    pop ar0\n"
                );
            }
            else
            {
                __asm__ (
                // bit "0" transmission
                "    push ar0\n"
                // HIGH
                "    setb    " NEO_PIXEL_ASM_PIN_STRING "\n"    //                      2 [1]             1
                "    mov r0,#3\n"        // 3 = 2 [jump] + 1 [not jump]                 2 [1]             2
                "003$:\n"
                "    djnz r0,003$\n"     // Decrement register and jump if not Zero     2 [2/3]          10
                // LOW
                "    clr    " NEO_PIXEL_ASM_PIN_STRING "\n"     //                      2 [1]             _1
                "    mov r0,#6\n"      // 6 = 5 [jump] + 1 [not jump]                   2 [1]             _2
                "004$:\n"
                "    djnz r0,004$\n"  // Decrement register and jump if not Zero        2 [2/3]           _19
                // "    nop    \n"       //                                                                  _20
                "    pop ar0\n"
                );
            }
        }
    }
}
