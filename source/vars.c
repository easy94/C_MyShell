#include "vars.h"

static struct var **vars;
static size_t var_len = 0;

void varCleanUp()
{
    for (size_t i = 0; i < var_len; i++)
    {
        free(vars[i]->identifier);        
        free(vars[i]->value);
        free(vars[i]);
    }
    free(vars);
}

void varCreate()
{
    vars = malloc(0);
}

char *varGet(char *id)
{
    char *temp;
    for (size_t i = 0; i < var_len; i++)
    {
        temp = (*vars)[i].identifier;
        if(strcmp(temp, id))
        {
            return (*vars)[i].value;
        }
    }
    
    return NULL;
}

int isVarDup(char *arg){
    for (size_t i = 0; i < var_len; i++)
    {
        if((*vars)[i].identifier == arg) return 1; 
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
    struct var *new = calloc(1, sizeof(new));
    
    new->value = strdup(val);
    new->identifier = strdup(id);
    vars[var_len-1] = new;
    return 0;
}

void varUnset()
{

}