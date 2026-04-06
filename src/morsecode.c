#include "morsecode.h"

#include "nelems.h"
#include "static_assert.h"


// MSb transmitted first, each bit as per MorseCodeSignal.
// https://en.wikipedia.org/wiki/Morse_code
__code MorseCodeSymbol const morseCodeSymbols[] = {
    // numbers
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
    // letters
    /* a */     {.content = 0b01000000, .length = 2},
    /* b */     {.content = 0b10000000, .length = 4},
    /* c */     {.content = 0b10100000, .length = 4},
    /* d */     {.content = 0b10000000, .length = 3},
    /* e */     {.content = 0b00000000, .length = 1},
    /* f */     {.content = 0b00100000, .length = 4},
    /* g */     {.content = 0b11000000, .length = 3},
    /* h */     {.content = 0b00000000, .length = 4},
    /* i */     {.content = 0b00000000, .length = 2},
    /* j */     {.content = 0b01110000, .length = 4},
    /* k */     {.content = 0b10100000, .length = 3},
    /* l */     {.content = 0b01000000, .length = 4},
    /* m */     {.content = 0b11000000, .length = 2},
    /* n */     {.content = 0b10000000, .length = 2},
    /* o */     {.content = 0b11100000, .length = 3},
    /* p */     {.content = 0b01100000, .length = 4},
    /* q */     {.content = 0b11010000, .length = 4},
    /* r */     {.content = 0b01000000, .length = 3},
    /* s */     {.content = 0b00000000, .length = 3},
    /* t */     {.content = 0b10000000, .length = 1},
    /* u */     {.content = 0b00100000, .length = 3},
    /* v */     {.content = 0b00010000, .length = 4},
    /* w */     {.content = 0b01100000, .length = 3},
    /* x */     {.content = 0b10010000, .length = 4},
    /* y */     {.content = 0b10110000, .length = 4},
    /* z */     {.content = 0b11000000, .length = 4},
    // punctuation
    /* . */     {.content = 0b01010100, .length = 6},
    /* , */     {.content = 0b11001100, .length = 6},
    /* ? */     {.content = 0b00110000, .length = 6},
    /* ' */     {.content = 0b01111000, .length = 6},
    /* / */     {.content = 0b10010000, .length = 5},
    /* ( */     {.content = 0b10110000, .length = 5},
    /* ) */     {.content = 0b10110100, .length = 6},
    /* : */     {.content = 0b11100000, .length = 6},
    /* = */     {.content = 0b10001000, .length = 5},
    /* + */     {.content = 0b01010000, .length = 5},
    /* - */     {.content = 0b10000100, .length = 6},
    /* " */     {.content = 0b01001000, .length = 6},
    /* @ */     {.content = 0b01101000, .length = 6},
    // non-latin characters
    /* ae */    {.content = 0b01010000, .length = 4},
    /* oe */    {.content = 0b11100000, .length = 4},
    /* ue */    {.content = 0b00110000, .length = 4},
    // inter-word-spacing
    /* space */ {.content = 0b00000000, .length = 0},
};


void morseCodeSenderStateInit(void)
{
    COMPILE_TIME_ASSERT(1 < NELEMS(morseCodeText));
    morseCodeSenderState.currentSymbol = &morseCodeText[0];
    morseCodeSenderState.currentSymbolWorkingCopy.content = morseCodeSymbols[*morseCodeSenderState.currentSymbol].content;
    morseCodeSenderState.currentSymbolWorkingCopy.length = morseCodeSymbols[*morseCodeSenderState.currentSymbol].length;
    morseCodeSenderState.durationTillNextSignal = 1; // allow pre-decrement
    morseCodeSenderState.showingSignalAndNotPause = false;
}


bool morseCodeSenderStateUpdate(void)
{
    bool const previousSignalAndNotPulse = morseCodeSenderState.showingSignalAndNotPause;

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
            if ((morseCodeText + NELEMS(morseCodeText)) ==  morseCodeSenderState.currentSymbol)
            {
                // Loop back to front of text.
                morseCodeSenderState.currentSymbol = &morseCodeText[0];
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
            MorseCodeSignal const nextSignal =
                    (0 == (morseCodeSenderState.currentSymbolWorkingCopy.content & 0x80)) ? morseCodeSignal_dit: morseCodeSignal_dah;
            // Advance symbol content.
            morseCodeSenderState.currentSymbolWorkingCopy.content <<= 1;
            morseCodeSenderState.currentSymbolWorkingCopy.length -= 1;
            // Apply nextSignal.
            morseCodeSenderState.durationTillNextSignal = morseCodeSignalToDuration(nextSignal);
            morseCodeSenderState.showingSignalAndNotPause = true;
        }
    }

    return (previousSignalAndNotPulse != morseCodeSenderState.showingSignalAndNotPause);
}