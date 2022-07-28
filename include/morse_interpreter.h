#ifndef __MORSE_INTERPRETER_H__
#define __MORSE_INTERPRETER_H__

#include <stdint.h>

#define MORSE_LETTER_MAXLEN 5
#define MORSE_BIN_MAX_BITS 19
#define CHAR_TO_MORSE_WINDOW_MASK 0b111
#define CHAR_TO_MORSE_WINDOW_BITS 3

#define CHAR_IS_NUM(ch) (ch >= '0' && ch <= '9')
#define CHAR_IS_ALPHA(ch) ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
//#define CHAR_IS_INVALID(ch) !(CHAR_IS_NUM(ch) || CHAR_IS_ALPHA(ch))

char* read_dynamic_string(void);
void check_message_validity(char* msg);
void print_string_ascii(char* msg);
char* char_to_morseletter(char msgch);
char morseletter_to_char(char* morseletter);
#endif