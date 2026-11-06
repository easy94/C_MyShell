#include <stdio.h>
#include <unistd.h>
#include "myheader.h"

struct chdir_data
{
    char *path_curr;
    char *before_one;
};

struct chdir_data *context;

void chdirCreate()
{
    context = calloc(0, sizeof(*context));
}

int chdirChange(char *path)
{
    if(!validatePath(path)){
        perror("chdir: ");
        errno=0;
        return;
    }
    if(context->path_curr) 
    context->before_one = context->path_curr;
        
    context->path_curr = strdup(path);
}