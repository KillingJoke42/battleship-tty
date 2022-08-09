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

uint8_t** alloc_dynamic_2d_arr(char num_rows, char num_cols)
{
    uint8_t **arr = (uint8_t **)malloc(sizeof(uint8_t *) * num_rows);

    for (int i = 0; i < num_rows; i++)
    {
        arr[i] = (uint8_t *)malloc(sizeof(uint8_t) * num_cols);
    }

    return arr;
}

void clear_2d_arr(uint8_t **arr, uint8_t val, uint8_t num_rows, uint8_t num_cols)
{
    for (int i = 0; i < num_rows; i++)
        for (int j = 0; j < num_cols; j++)
            arr[i][j] = val;
            
    return;
}

void enter_wait_prompt(char* prompt)
{
    char c;
    printf("%s\n", prompt);
    while((c = getchar()) != '\n' && c != '\r');
}