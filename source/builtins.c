#include "builtins.h"

const size_t BUILTINS_LEN = 4;

const char *BUILTINS[4] =
{
    "exec",  // execute bin/script    0
    "chdir", // change directory      1
    "exit",   // exit program          2
    "var",   // create var             3
};