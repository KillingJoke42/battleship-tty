#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define clrscr() printf("\e[1;1H\e[2J")

char* get_string(char* prompt);
#endif