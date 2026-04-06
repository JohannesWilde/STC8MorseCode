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

static inline uint8_t morseCodeSignalToDuration(MorseCodeSignal const signal)
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
    // numbers
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
    // letters
    morseCodeSymbolIndex_a,
    morseCodeSymbolIndex_b,
    morseCodeSymbolIndex_c,
    morseCodeSymbolIndex_d,
    morseCodeSymbolIndex_e,
    morseCodeSymbolIndex_f,
    morseCodeSymbolIndex_g,
    morseCodeSymbolIndex_h,
    morseCodeSymbolIndex_i,
    morseCodeSymbolIndex_j,
    morseCodeSymbolIndex_k,
    morseCodeSymbolIndex_l,
    morseCodeSymbolIndex_m,
    morseCodeSymbolIndex_n,
    morseCodeSymbolIndex_o,
    morseCodeSymbolIndex_p,
    morseCodeSymbolIndex_q,
    morseCodeSymbolIndex_r,
    morseCodeSymbolIndex_s,
    morseCodeSymbolIndex_t,
    morseCodeSymbolIndex_u,
    morseCodeSymbolIndex_v,
    morseCodeSymbolIndex_w,
    morseCodeSymbolIndex_x,
    morseCodeSymbolIndex_y,
    morseCodeSymbolIndex_z,
    // punctuation
    morseCodeSymbolIndex_period,
    morseCodeSymbolIndex_comma,
    morseCodeSymbolIndex_questionmark,
    morseCodeSymbolIndex_apostrophe,
    morseCodeSymbolIndex_slash,
    morseCodeSymbolIndex_parenthesisOpen,
    morseCodeSymbolIndex_parenthesisClose,
    morseCodeSymbolIndex_colon,
    morseCodeSymbolIndex_doubleDash,
    morseCodeSymbolIndex_plus,
    morseCodeSymbolIndex_minus,
    morseCodeSymbolIndex_quotation,
    morseCodeSymbolIndex_at,
    // non-latin characters
    morseCodeSymbolIndex_ae,
    morseCodeSymbolIndex_oe,
    morseCodeSymbolIndex_ue,
    // inter-word-spacing
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


typedef enum
{
    morseCodeSenderUpdateResult_noUpdate,
    morseCodeSenderUpdateResult_update,
    morseCodeSenderUpdateResult_end
}
MorseCodeSenderUpdateResult;


void morseCodeSenderStateInit(void);
MorseCodeSenderUpdateResult morseCodeSenderStateUpdate(void);

#endif // MORSECODE_H
