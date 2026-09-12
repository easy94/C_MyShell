#include "vars.h"

static struct var *vars;
static size_t var_len = 1;

void varCreate()
{
    vars = malloc(1);
}

char *varGet(char *id)
{
    char *temp;
    for (size_t i = 0; i < var_len; i++)
    {
        if((temp = vars[i].identifier) == id)
        {
            return temp;
        }
    }
    
    return NULL;
}

int isVarDup(char *arg){
    for (size_t i = 0; i < var_len; i++)
    {
        if(vars[i].identifier == arg) return 1; 
    }

    return 0;
}

int varAdd(char* id, char *val)
{
    if(getenv(id) || isVarDup(id)){
        fprintf(stderr, "variable name exists");
        return -1;
    }

    vars = reallocarray(vars, ++var_len, sizeof(struct var));
    strcpy(vars[var_len].identifier, id);
    strcpy(vars[var_len].value, val);
    

    return 0;
}

void varUnset()
{

}