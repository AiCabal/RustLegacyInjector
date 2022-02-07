#pragma once
#include <stdint.h>

#ifdef _WINDOWS
#define NEWLINE "\r\n"
#else
#define NEWLINE "\n"
#endif

#define Length(array) (sizeof((array)) / sizeof((array[0])))