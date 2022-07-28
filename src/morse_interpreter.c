#include <morse_interpreter.h>
#include <morse_data.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

char* read_dynamic_string(void)
{
    char *str;
    char ch;
    uint8_t next_char = 0;
    uint8_t strlen = 1;
    
    str = (char *)malloc(sizeof(char));
    if (str == NULL)
    {
        printf("Error: Failed to allocate memory for var \"str\" in function \"read_dynamic_string\".\n");
        return NULL;
    }

    printf("Enter message: ");
    
    while((ch = getc(stdin)) != '\n')
    {
        if (ch != ' ' && (!CHAR_IS_ALPHA(ch) && !CHAR_IS_NUM(ch)))
            continue;

        str = (char *)realloc(str, strlen++ * sizeof(char));
        *(str + next_char++) = ch;
    }

    *(str + next_char) = '\0';
    return str;
}

void check_message_validity(char* msg)
{
    int i = 0;
    while(msg[i] != '\0')
    {
        if (msg[i] == ' ')
        {
            i++;
            continue;
        }
        // if (CHAR_IS_INVALID(msg[i]))
        //     printf("Invalid char is %d\n", msg[i]);
        msg[i] = CHAR_IS_ALPHA(msg[i]) ? 'A' : (CHAR_IS_NUM(msg[i]) ? 'N' : 'I');
        i++;
    }
}

void print_string_ascii(char* msg)
{
    for (int i = 0; msg[i] != '\0'; i++)
        printf("%d ", msg[i]);
        
    printf("\n");
    return;
}

char* char_to_morseletter(char msgch)
{
    char* morselstr = (char *)malloc((sizeof(char)) * (MORSE_LETTER_MAXLEN + 1));
    int striter = 0;
    if (CHAR_IS_ALPHA(msgch))
    {
        msgch = 10 + ((msgch >= 'A' && msgch <= 'Z') ? (msgch - 'A') : (msgch - 'a'));
    }
    else
    {
        msgch = msgch - '0';
    }
    int shift = 0;
    letter_t symb = morse_map[msgch];
    if (symb == 0b1)
    {
        morselstr[0] = '.';
        morselstr[1] = '\0';
        return morselstr;
    }
    while(!(symb & (1 << (MORSE_BIN_MAX_BITS - 1 - shift))))
        shift++;
    shift = (MORSE_BIN_MAX_BITS - shift) - 3;
    while(shift >= 0)
    {
        uint8_t binfr = (symb & (CHAR_TO_MORSE_WINDOW_MASK << shift)) >> shift;
        switch (binfr)
        {
            case 0b111:
                morselstr[striter++] = '-';
                shift -= (CHAR_TO_MORSE_WINDOW_BITS + 1);
                break;
            case 0b101:
                morselstr[striter++] = '.';
                shift -= (CHAR_TO_MORSE_WINDOW_BITS - 1);
                break;
            default:
                printf("window: %x\n", binfr);
                break;
        }
    }
    if (shift == -2)
        morselstr[striter++] = '.';
    morselstr[striter] = '\0';
    return morselstr;
}

char morseletter_to_char(char* morseletter)
{

}