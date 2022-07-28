#include <morse_interpreter.h>
#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    // char* str = read_dynamic_string();
    // if (str == NULL)
    //     return -1;
    char* test;
    // for (int i = '0'; i <= '9'; i++)
    // {
    // test = char_to_morseletter(i);
    // printf("Entered string is: %s\n", test);
    // }
    for (int i = '0'; i <= '9'; i++)
    {
    test = char_to_morseletter(i);
    printf("Entered string %c is: %s\n", i, test);
    }
    for (int i = 'A'; i <= 'Z'; i++)
    {
    test = char_to_morseletter(i);
    printf("Entered string %c is: %s\n", i, test);
    }
    free(test);
    return 0;
}