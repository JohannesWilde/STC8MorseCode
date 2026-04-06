import argparse
import sys

conversion = {
    # numbers
    '0': "morseCodeSymbolIndex_0",
    '1': "morseCodeSymbolIndex_1",
    '2': "morseCodeSymbolIndex_2",
    '3': "morseCodeSymbolIndex_3",
    '4': "morseCodeSymbolIndex_4",
    '5': "morseCodeSymbolIndex_5",
    '6': "morseCodeSymbolIndex_6",
    '7': "morseCodeSymbolIndex_7",
    '8': "morseCodeSymbolIndex_8",
    '9': "morseCodeSymbolIndex_9",
    # letters
    'a': "morseCodeSymbolIndex_a",
    'b': "morseCodeSymbolIndex_b",
    'c': "morseCodeSymbolIndex_c",
    'd': "morseCodeSymbolIndex_d",
    'e': "morseCodeSymbolIndex_e",
    'f': "morseCodeSymbolIndex_f",
    'g': "morseCodeSymbolIndex_g",
    'h': "morseCodeSymbolIndex_h",
    'i': "morseCodeSymbolIndex_i",
    'j': "morseCodeSymbolIndex_j",
    'k': "morseCodeSymbolIndex_k",
    'l': "morseCodeSymbolIndex_l",
    'm': "morseCodeSymbolIndex_m",
    'n': "morseCodeSymbolIndex_n",
    'o': "morseCodeSymbolIndex_o",
    'p': "morseCodeSymbolIndex_p",
    'q': "morseCodeSymbolIndex_q",
    'r': "morseCodeSymbolIndex_r",
    's': "morseCodeSymbolIndex_s",
    't': "morseCodeSymbolIndex_t",
    'u': "morseCodeSymbolIndex_u",
    'v': "morseCodeSymbolIndex_v",
    'w': "morseCodeSymbolIndex_w",
    'x': "morseCodeSymbolIndex_x",
    'y': "morseCodeSymbolIndex_y",
    'z': "morseCodeSymbolIndex_z",
    'A': "morseCodeSymbolIndex_a",
    'B': "morseCodeSymbolIndex_b",
    'C': "morseCodeSymbolIndex_c",
    'D': "morseCodeSymbolIndex_d",
    'E': "morseCodeSymbolIndex_e",
    'F': "morseCodeSymbolIndex_f",
    'G': "morseCodeSymbolIndex_g",
    'H': "morseCodeSymbolIndex_h",
    'I': "morseCodeSymbolIndex_i",
    'J': "morseCodeSymbolIndex_j",
    'K': "morseCodeSymbolIndex_k",
    'L': "morseCodeSymbolIndex_l",
    'M': "morseCodeSymbolIndex_m",
    'N': "morseCodeSymbolIndex_n",
    'O': "morseCodeSymbolIndex_o",
    'P': "morseCodeSymbolIndex_p",
    'Q': "morseCodeSymbolIndex_q",
    'R': "morseCodeSymbolIndex_r",
    'S': "morseCodeSymbolIndex_s",
    'T': "morseCodeSymbolIndex_t",
    'U': "morseCodeSymbolIndex_u",
    'V': "morseCodeSymbolIndex_v",
    'W': "morseCodeSymbolIndex_w",
    'X': "morseCodeSymbolIndex_x",
    'Y': "morseCodeSymbolIndex_y",
    'Z': "morseCodeSymbolIndex_z",
    # punctuation
    '.': "morseCodeSymbolIndex_period",
    ',': "morseCodeSymbolIndex_comma",
    '?': "morseCodeSymbolIndex_questionmark",
    '\'': "morseCodeSymbolIndex_apostrophe",
    '/': "morseCodeSymbolIndex_slash",
    '(': "morseCodeSymbolIndex_parenthesisOpen",
    ')': "morseCodeSymbolIndex_parenthesisClose",
    ':': "morseCodeSymbolIndex_colon",
    '=': "morseCodeSymbolIndex_doubleDash",
    '+': "morseCodeSymbolIndex_plus",
    '-': "morseCodeSymbolIndex_minus",
    '"': "morseCodeSymbolIndex_quotation",
    '@': "morseCodeSymbolIndex_at",
    # non-latin characters
    'ä': "morseCodeSymbolIndex_ae",
    'ö': "morseCodeSymbolIndex_oe",
    'ü': "morseCodeSymbolIndex_ue",
    'Ä': "morseCodeSymbolIndex_ae",
    'Ö': "morseCodeSymbolIndex_oe",
    'Ü': "morseCodeSymbolIndex_ue",
    # inter-word-spacing
    ' ': "morseCodeSymbolIndex_space",
    '\n': "morseCodeSymbolIndex_space",
}

# Source - https://stackoverflow.com/a/15008806
# Posted by mgilson, modified by community. See post 'Timeline' for change history
# Retrieved 2026-04-06, License - CC BY-SA 4.0

def t_or_f(arg):
    ua = str(arg).upper()
    if 'TRUE' == ua:
       return True
    elif 'FALSE' == ua:
       return False
    else:
       raise ValueError(f"Non-boolean string \"{arg}\" - only \"True\" or \"False\" are supported.")


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Parsing text and converting it to a list of MorseCodeSymbolIndex.")
    parser.add_argument("-i", "--input_path", help="Path to the input file.", required=True)
    parser.add_argument("-o", "--output_path", help="Path where to create the output file.", required=True)
    parser.add_argument("-n", "--no-overwrite", default="False", help="Do not overwrite the output file if it already exists [default False].", type=t_or_f)
    args = parser.parse_args()

    input_file_name = args.input_path
    output_file_name = args.output_path
    output_file_creation_mode = 'x' if args.no_overwrite else 'w'

    with open(input_file_name, "r", encoding="utf-8") as input_file:
        input = input_file.read()

    return_value = 0

    output = ""
    try:
        for character in input:
            output += conversion[character] + ", "
    except KeyError as e:
        return_value = -1
        print(f'Error: {e}')

    # print(output)

    if (0 == return_value):
        with open(output_file_name, output_file_creation_mode, encoding="utf-8") as output_file:
            output_file.write(output)

    exit(return_value)
