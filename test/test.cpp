#include "morsecode.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <iostream>
#include <vector>


// hacky way to remove storage specifier from STC source code.
#define __code

const MorseCodeSymbolIndex morseCodeText[] = {
    morseCodeSymbolIndex_5,
    morseCodeSymbolIndex_1,
    morseCodeSymbolIndex_space,
    morseCodeSymbolIndex_space,
    morseCodeSymbolIndex_2,
    morseCodeSymbolIndex_3,
};
MorseCodeSenderState morseCodeSenderState;

#include "morsecode.c"


static void printVector(std::vector<bool> const & values)
{
    for (bool const value : values)
    {
        if (value)
        {
            std::cout << "^";
        }
        else
        {
            std::cout << "_";
        }
    }
}


#define DIT true
#define DAH true, true, true
#define PAUSE_1 false
#define PAUSE_3 false, false, false
#define PAUSE_7 false, false, false, false, false, false, false

int main()
{
    std::vector<bool> const expectations{
        /*morseCodeSymbolIndex_5*/ DIT, PAUSE_1, DIT, PAUSE_1, DIT, PAUSE_1, DIT, PAUSE_1, DIT,
                                   PAUSE_3,
        /*morseCodeSymbolIndex_1*/ DIT, PAUSE_1, DAH, PAUSE_1, DAH, PAUSE_1, DAH, PAUSE_1, DAH,
        /*morseCodeSymbolIndex_space*/ PAUSE_7,
        /*morseCodeSymbolIndex_space*/ PAUSE_7,
        /*morseCodeSymbolIndex_2*/ DIT, PAUSE_1, DIT, PAUSE_1, DAH, PAUSE_1, DAH, PAUSE_1, DAH,
        /*morseCodeSymbolIndex_3*/ DIT, PAUSE_1, DIT, PAUSE_1, DIT, PAUSE_1, DAH, PAUSE_1, DAH,
    };
    std::vector<bool> observations;
    observations.reserve(expectations.size());



    morseCodeSenderStateInit();

    for (size_t index = 0; expectations.size() > index; ++index)
    {
        if (morseCodeSenderStateUpdate())
        {
            // intentionally empty
        }
        else
        {
            // intentionally empty
        }

        observations.push_back(morseCodeSenderState.showingSignalAndNotPause);
    }


    bool success = false;

    if (expectations.size() != observations.size())
    {
        // success = false;
    }
    else
    {
        bool equal = true;
        for (size_t index = 0; expectations.size() > index; ++index)
        {
            if (expectations[index] != observations[index])
            {
                equal = false;
                break;
            }
            else
            {
                // intentionally empty
            }
        }
        success = equal;
    }


    if (not success)
    {
        std::cout << "observations: ";
        printVector(observations);
        std::cout << std::endl;

        std::cout << "expectations: ";
        printVector(expectations);
        std::cout << std::endl;

        std::cout << "Failed." << std::endl;
    }
    else
    {
        std::cout << "Success." << std::endl;
    }

    return success ? 0 : -1;
}
