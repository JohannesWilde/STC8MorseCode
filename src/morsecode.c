#include "morsecode.h"

#include "nelems.h"
#include "static_assert.h"


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


void morseCodeSenderStateInit(void)
{
    COMPILE_TIME_ASSERT(1 < NELEMS(morseCodeText));
    morseCodeSenderState.currentSymbol = &morseCodeText[0];
    morseCodeSenderState.currentSymbolWorkingCopy.content = morseCodeSymbols[*morseCodeSenderState.currentSymbol].content;
    morseCodeSenderState.currentSymbolWorkingCopy.length = morseCodeSymbols[*morseCodeSenderState.currentSymbol].length;
    morseCodeSenderState.durationTillNextSignal = 1; // allow pre-decrement
    // morseCodeSenderState.showingSignalAndNotPause = false;
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
            MorseCodeSignal const nextSignal = (MorseCodeSignal)(morseCodeSenderState.currentSymbolWorkingCopy.content & 0x80);
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