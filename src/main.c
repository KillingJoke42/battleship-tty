#include <morse_interpreter.h>
#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    char* str = read_dynamic_string();
    if (str == NULL)
        return -1;

    char* morsestr = string_to_morse(str);
    printf("Entered string is: %s\n", morsestr);
    free(str);
    free(morsestr);
    return 0;
}