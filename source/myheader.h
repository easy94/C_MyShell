#if !defined(MYHEADER_H)
#define MYHEADER_H

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

const int MAXSIZE = 15;
const size_t BUILTINS_LEN = 3;
const size_t OPERATORS_LEN = 5;

enum OPERATOR
{
    PIPE = 0,
    REDIRECT_TO = 1,
    REDIRECT_FROM = 2,
    APPEND_TO = 3,
    WORD = 4,
};

enum BUILTIN
{
    EXEC = 0,
    CHDIR = 1,
    EXIT_APP = 2,
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



static const char *BUILTINS[3] =
    {
        "exec",  // execute bin/script    0
        "chdir", // change directory      1
        "exit"   // exit program          2
};


static const char *OPERATORS[5] =
{
    "|", ">", "<", ">>", ""
};


int checkToken(char *tok);
int expandPathVar(char *target, const char **out);
int validatePath(char *path);
int validateKeyAndAdvbufptr(char *key_out, char **buf);
Pipe *assignPipeline(Token *tokens[], int len);
int tokenize(char **buf, Token **token_list_out[], size_t *len_out);
void cleanUp(char *const **pathargs, const char **path);
int ExecCmds(Token *tokens[], size_t *index);


#endif // MYHEADER_H
