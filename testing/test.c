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
        pipeline->cmd[i]=c;                             //these must be all freed afterwards.//So: pipeline->cmd->args->allelementsonebyone 
        pipeline->cmd[i]->args = malloc(sizeof(char*));
        pipeline->cmd[i]->args_capacity = 1;                                               //then: pipeline->cmd->name
    }
    pipeline->cntr = 1;
    Command *current = pipeline->cmd[cntr];

    for (size_t i = 0; i < len; i++)
    {
        switch (tokens[i]->type)
        {
        case WORD:
            if(argc == 0){
                current->cmd = tokens[i]->name;
            }
            if(current->args_count == current->args_capacity)
            {
                current->args = realloc(current->args, sizeof(char*)*2*current->args_capacity);
                current->args_capacity *= 2;
            }
            current->args[argc++] = tokens[i]->name;
            current->args_count++;

            break;
        case PIPE:
            if(current->args_count == current->args_capacity)
            {
                current->args = realloc(current->args, sizeof(char*)*2*current->args_capacity);
                current->args_capacity *= 2; 
            }
            current->args[argc] = NULL;
            argc = 0;
            current = pipeline->cmd[++cntr];
            pipeline->cntr++;
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

int ExecCmds(Pipe *cmds)
{
    size_t len = cmds->cntr;
    pid_t cid;
    int wstatus;
    int fd[2];
    char *temp;

    for (size_t i = 0; i < len; i++)
    {
        cid = fork();

        if (!(cmds->cmd[i]->file_in && cmds->cmd[i]->file_out))
        {
            pipe(fd);
        }

        if (cid == -1)
        {
            fprintf(stderr, "-1fork");
            abort();
        }
        
        if (cid == 0)
        {
            if((temp = cmds->cmd[i]->file_in) != NULL)
            {
                fd[0] = open(temp, O_RDONLY);
                dup2(fd[0], STDIN_FILENO);
            }
            if (temp = cmds->cmd[i]->file_out != NULL)
            {
                fd[1] = open(temp, O_WRONLY);
                dup2(fd[1], STDOUT_FILENO);
            }
            if (!(cmds->cmd[i]->file_in && cmds->cmd[i]->file_out))
            {
                dup2(fd[1],STDOUT_FILENO);

            }
            

            
        }
        else
        {
            if(!(cmds->cmd[i]->file_in && cmds->cmd[i]->file_out))  //pipe
            {
                dup2(fd[0], STDIN_FILENO);      //read from pipes' readend

            }
        }
        
    }

    return 0;
}

int main(int argc, char const *argv[])
{
    int len = 6;
    Token a = {.name="ls",.type=WORD};
    Token b = {.name="-a",.type=WORD};
    Token e = {.type = REDIRECT_FROM};
    Token f = {.name="/", .type=WORD};
    Token c = {.name="",.type=PIPE};
    Token d = {.name="wc",.type=WORD};

    Token *testdata[6] = {&a,&b,&e,&f,&c,&d};

    assignPipeline(testdata, len);

    return 0;
}
