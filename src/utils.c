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

uint8_t* alloc_rng_arr(uint8_t sz)
{
    uint8_t *arr = (uint8_t *)malloc(sizeof(uint8_t) * sz);
    for (int i = 0; i < sz; i++)
        *(arr + i) = i;
    return arr;
}

uint8_t get_rng_val(uint8_t *arr, uint8_t *sz)
{
    if (*sz == 1)
    {
        *sz = 0;
        return *(arr);
    }
    uint8_t rng_out = (rand() % *sz);
    uint8_t ret = *(arr + rng_out);

    // printf("arr: ");
    // for (int i = 0; i < *sz; i++)
    //     printf("%d ", *(arr + i));
    // printf("\n");

    uint8_t temp = *(arr + rng_out);
    *(arr + rng_out) = *(arr + *sz - 1);
    *(arr + *sz - 1) = temp;

    *(sz) = *(sz) - 1;
    arr = realloc(arr, sizeof(uint8_t) * *(sz));

    // printf("arrnew: ");
    // for (int i = 0; i < *sz; i++)
    //     printf("%d ", *(arr + i));
    // printf("\n");
    return ret;
}