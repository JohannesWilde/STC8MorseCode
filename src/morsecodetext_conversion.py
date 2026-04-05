
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
    'b': "morseCodeSymbolIndex_v",
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
}

if __name__ == "__main__":

    input_file_name = "morsecodetext_raw.txt"
    output_file_name = "morsecodetext.c"

    with open(input_file_name, "r") as input_file:
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
        with open(output_file_name, "w") as output_file:
            output_file.write(output)

    exit(return_value)
