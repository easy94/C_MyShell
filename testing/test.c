#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/wait.h>
#include <fcntl.h>


enum OPERATOR
{
    PIPE = 0,
    REDIRECT_TO = 1,
    REDIRECT_FROM = 2,
    APPEND_TO = 3,
    WORD = 4,
};
typedef struct
{
    char *file_in;  // NULL equals stdin
    char *file_out; // NULL equals stdout
    const char *cmd;      // the program to execute
    char **args;    // the argv arr
    size_t args_count;
    size_t args_capacity;
    int append_flag;
} Command;

typedef struct
{
    Command *cmd[15];
    int cntr;
} Pipe;

typedef struct
{
    int type;       // see OPERATOR enum
    char *name;     // program name path
} Token;


Pipe *assignPipeline(Token *tokens[], int len)
{
    int cntr = 0;
    int argc = 0;
    Pipe *pipeline = malloc(sizeof(Pipe));
    for (size_t i = 0; i < 15; i++)
    {
        Command *c = malloc(sizeof(Command));
        pipeline->cmd[i]=c;                             //these must be all freed afterwards. So: pipeline->cmd->args->allelementsonebyone then: pipeline->cmd->name
    }
    
    Command *current = pipeline->cmd[cntr];

    for (size_t i = 0; i < len; i++)
    {
        switch (tokens[i]->type)
        {
        case WORD:
            if(argc == 0){
                current->cmd = tokens[i]->name;
            }
            else
            {
                if(current->args_count == current->args_capacity)
                {
                    current->args = realloc(current->args, sizeof(char*)*2*current->args_capacity);
                }
                current->args[argc++] = tokens[i]->name;
            }
            break;
        case PIPE:
            current->args[argc] = NULL;
            argc = 0;
            current = pipeline->cmd[++cntr];
            break;
        case REDIRECT_FROM:
            current->file_in = tokens[++i]->name;
            break;
        case REDIRECT_TO:
            current->file_out = tokens[++i]->name;
            break;
        case APPEND_TO:
            current->file_out = tokens[++i]->name;
            break;
        default:
            break;
        }
    }

    return pipeline;
}

int main(int argc, char const *argv[])
{
    size_t len = 4;

    Token a = {.name="ls",.type=WORD};
    Token b = {.name="-a",.type=WORD};
    Token c = {.name="",.type=PIPE};
    Token d = {.name="wc",.type=WORD};

    Token *testdata[4] = {&a,&b,&c,&d};

    assignPipeline(testdata, len);

    return 0;
}
