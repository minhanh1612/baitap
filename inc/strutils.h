#ifndef STRUTILS_H
#define STRUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


char* str_reverse(char* str);

char* str_trim(char* str);

int str_to_int(const char* str, int* result);

#endif // STRUTILS_H