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
    
}

char morseletter_to_char(char* morseletter)
{

}