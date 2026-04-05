#include "morsecode.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>


// hacky way to remove storage specifier from STC source code.
#define __code

const MorseCodeSymbolIndex morseCodeText[] = {
    morseCodeSymbolIndex_5, /*morseCodeSymbolIndex_1, morseCodeSymbolIndex_2, morseCodeSymbolIndex_3, */morseCodeSymbolIndex_space,morseCodeSymbolIndex_space,
};
MorseCodeSenderState morseCodeSenderState;

#include "morsecode.c"


int main()
{
    morseCodeSenderStateInit();


    for (size_t index = 0; 50 > index; ++index)
    {
        if (morseCodeSenderStateUpdate())
        {
            // intentionally empty
        }
        else
        {
            // intentionally empty
        }

        if (morseCodeSenderState.showingSignalAndNotPause)
        {
            printf("^");
        }
        else
        {
            printf("_");
        }
    }

    printf("\n");

    return 0;
}
