#include <utils.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>

void echo_on(void)
{
    struct termios t;
    printf("\n");
    tcgetattr(0, &t);
    t.c_lflag |= ECHO;
    tcsetattr(0, TCSANOW, &t);  
}

void echo_off(void)
{
    struct termios t;
    tcgetattr(0, &t);
    t.c_lflag &= ~ECHO;
    tcsetattr(0, TCSANOW, &t);
}

char* get_string(char* prompt)
{
    char c;
    uint8_t namelen = 1;
    char *str = (char *)malloc(sizeof(char));

    while (namelen == 1)
    {
        printf("%s", prompt);
        while ((c = getchar()) != '\n' && c != EOF)
        {
            str[namelen++ - 1] = c;
            str = realloc(str, sizeof(char) * (namelen));
        }
        str[namelen - 1] = '\0';
    }
    return str;
}

char get_character(char *info_prompt, char *err_prompt, char start, char end)
{
    char c;
    char ret = '\0';
    do {
        printf("%s", info_prompt);
        while((c = getchar()) != '\n' && c != EOF)
            ret = c;
        if (ret < start || ret > end)
            printf("%s", err_prompt);                
    } while (ret < start || ret > end);
    return ret;
}

uint8_t* alloc_dynamic_2d_arr(char num_rows, char num_cols)
{
    uint8_t *arr = (uint8_t *)malloc(sizeof(uint8_t) * num_rows * num_cols);

    return arr;
}

char* alloc_dynamic_2d_char_arr(char num_rows, char num_cols)
{
    char *arr = (char *)malloc(sizeof(char) * num_rows * num_cols);

    return arr;
}

uint8_t arr_2d_get_val(uint8_t *arr, char num_rows, char row, char col)
{
    return arr[(num_rows * row) + col];
}

void arr_2d_set_val(uint8_t *arr, char num_rows, char row, char col, uint8_t val)
{
    arr[(num_rows * row) + col] = val;
    return;
}

char arr_2d_get_char_val(char *arr, char num_rows, char row, char col)
{
    return arr[(num_rows * row) + col];
}

void arr_2d_set_char_val(char *arr, char num_rows, char row, char col, uint8_t val)
{
    arr[(num_rows * row) + col] = val;
    return;
}

void clear_2d_arr(uint8_t *arr, uint8_t val, uint8_t num_rows, uint8_t num_cols)
{
    for (int i = 0; i < num_rows; i++)
        for (int j = 0; j < num_cols; j++)
            arr[(num_rows * i) + j] = val;
            
    return;
}

void clear_2d_char_arr(char *arr, uint8_t val, uint8_t num_rows, uint8_t num_cols)
{
    for (int i = 0; i < num_rows; i++)
        for (int j = 0; j < num_cols; j++)
            arr[(num_rows * i) + j] = val;
            
    return;
}

void print_2d_dynamic_arr(uint8_t *arr, uint8_t num_rows, uint8_t num_cols)
{
    printf("  ");
    for (int format = 0; format < num_cols; format++)
        printf("%d ", format);
    printf("\n");
    for (int row = 0; row < num_rows; row++)
    {
        printf("%c ", row + 'A');
        for (int col = 0; col < num_cols; col++)
        {
            printf("%c ", arr[(num_rows * row) + col]);
        }
        printf("\n");
    }
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

    uint8_t temp = *(arr + rng_out);
    *(arr + rng_out) = *(arr + *sz - 1);
    *(arr + *sz - 1) = temp;

    *(sz) = *(sz) - 1;
    //`arr = realloc(arr, sizeof(uint8_t) * *(sz));

    return ret;
}

void scalar_multiplication(char *dest, char *src, uint8_t binarize)
{
    for (int i = 0; i < NUM_ROWS; i++)
    {
        for (int j = 0; j < NUM_COLS; j++)
        {
            char idx = (NUM_ROWS*i)+j;
            *(dest+idx) = (binarize == 0) ? (*(dest+idx) * *(src+idx)) : ((*(dest+idx) * *(src+idx)) ? '#' : '0');
        }
    }
}

void scalar_swap(char *dest, char *src, char num_rows, char num_cols)
{
    for (int i = 0; i < num_rows; i++)
    {
        for (int j = 0; j < num_cols; j++)
        {
            char idx = (num_rows*i)+j;
            dest[idx] = dest[idx] + src[idx];
            src[idx] = dest[idx] - src[idx];
            dest[idx] = dest[idx] - src[idx];
        }
    }
}

void swapchars(char *x, char *y)
{
    *x = *x + *y;
    *y = *x - *y;
    *x = *x - *y;
    return;
}

void swapuint8(uint8_t *x, uint8_t *y)
{
    *x = *x + *y;
    *y = *x - *y;
    *x = *x - *y;
    return;
}

void delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;
  
    // Storing start time
    clock_t start_time = clock();
  
    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds);
}