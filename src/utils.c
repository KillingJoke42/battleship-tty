#include <utils.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

char* get_string(char* prompt)
{
    char c;
    uint8_t namelen = 1;
    char *str = (char *)malloc(sizeof(char));

    printf("%s", prompt);
    while ((c = getchar()) != '\n' && c != EOF)
    {
        str[namelen++ - 1] = c;
        str = realloc(str, sizeof(char) * (namelen));
    }
    str[namelen - 1] = '\0';
    return str;
}