#if !defined(BUILTINS_H)
#define BUILTINS_H
#include <stdio.h>
enum BUILTIN    //whats added here must be added in main.c and maybe more
{
    EXEC = 0,
    CHDIR = 1,
    EXIT_APP = 2,
    VAR = 3,

};

extern const size_t BUILTINS_LEN;
extern const char *BUILTINS[];

#endif // BUILTINS_H
