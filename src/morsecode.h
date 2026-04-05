#ifndef MORSECODE_H
#define MORSECODE_H

#include <stdbool.h>
#include <stdint.h>


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


typedef struct
{
    MorseCodeSymbolIndex const * currentSymbol;
    MorseCodeSymbol currentSymbolWorkingCopy;
    uint8_t durationTillNextSignal;
    bool showingSignalAndNotPause;
}
MorseCodeSenderState;


void morseCodeSenderStateInit(void);
bool morseCodeSenderStateUpdate(void);

#endif // MORSECODE_H
