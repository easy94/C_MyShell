#include <stdio.h>
#include <unistd.h>
#include "/home/lucas/VSCProjects/C_MyShell/source/myheader.h"
#include "/home/lucas/VSCProjects/C_MyShell/source/builtins.h"

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

void chdirChange(char *path){

    if(context->path_curr) 
    context->before_one = context->path_curr;
        
    context->path_curr = strdup(path);
}

int changeDir(Token *tok)
{
    if(!validatePath(tok->name)){
        perror("chdir: ");
        errno=0;
        return;
    }
    chdirChange(tok->name);
}