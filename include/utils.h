#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define clrscr() printf("\e[1;1H\e[2J")

char* get_string(char* prompt);
uint8_t** alloc_dynamic_2d_arr(char num_rows, char num_cols);
void clear_2d_arr(uint8_t **arr, uint8_t val, uint8_t num_rows, uint8_t num_cols);
#endif