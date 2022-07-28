#include <morse_interpreter.h>
#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    char* str = read_dynamic_string();
    if (str == NULL)
        return -1;
    printf("Entered string is: %s\n", str);
    free(str);
    return 0;
}