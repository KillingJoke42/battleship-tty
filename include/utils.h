#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define clrscr() printf("\e[H\e[2J\e[3J")

#ifndef NUM_ROWS
#define NUM_ROWS    10
#endif

#ifndef NUM_COLS
#define NUM_COLS    10
#endif

#ifndef NUM_SHIPS
#define NUM_SHIPS   5
#endif

typedef struct rng_mem{
    uint8_t val;
    struct rng_mem* next;
} rng_mem_t;

void echo_on(void);
void echo_off(void);
char* get_string(char* prompt);
char get_character(char *info_prompt, char *err_prompt, char start, char end);
uint8_t** alloc_dynamic_2d_arr(char num_rows, char num_cols);
void clear_2d_arr(uint8_t **arr, uint8_t val, uint8_t num_rows, uint8_t num_cols);
void print_2d_dynamic_arr(uint8_t **arr, uint8_t num_rows, uint8_t num_cols);
void enter_wait_prompt(char* prompt);
uint8_t* alloc_rng_arr(uint8_t sz);
uint8_t get_rng_val(uint8_t* arr, uint8_t* sz);
void scalar_multiplication(char (*dest)[NUM_ROWS][NUM_COLS], char (*src)[NUM_ROWS][NUM_COLS], uint8_t binarize);
void scalar_swap(char dest[NUM_ROWS][NUM_COLS], char src[NUM_ROWS][NUM_COLS]);
void swapchars(char *x, char *y);
void swapuint8(uint8_t *x, uint8_t *y);
void delay(int number_of_seconds);
#endif