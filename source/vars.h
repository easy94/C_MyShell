#if !defined(VARS_H)
#define VARS_H
#include "myheader.h"

struct var
{
    char *identifier;
    char *value;
};

void varCreate();
char *varGet(char *id);
int varAdd(char* id, char *val);
int isVarDup(char *s);

#endif // VARS_H
