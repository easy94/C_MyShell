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

enum BUILTIN
{
    EXEC = 0,
    CHDIR = 1,
    EXIT_APP = 2,
};
static const int MAXSIZE = 15;

static const char *BUILTINS[3] =
    {
        "exec",  // execute bin/script    0
        "chdir", // change directory      1
        "exit"   // exit program          2
};
static const size_t BUILTINS_LEN = 3;

typedef struct
{
    char *file_in;  // NULL equals stdin
    char *file_out; // NULL equals stdout
    char *cmd;      // the program to execute
    char *const *args;    // the argv arr
    int append_flag;
} Command;

typedef struct
{
    Command cmd[MAXSIZE];
    int cntr;
} Pipe;

typedef struct
{
    int type;          // see OPERATOR enum
    const char *name;  // program name path
} Token;

static const char *OPERATORS[5] =
{
    "|", ">", "<", ">>", ""
};

static const size_t OPERATORS_LEN = 5;

int checkToken(char *tok)
{
    for (size_t i = 0; i < OPERATORS_LEN; i++)
    {
        if (strcmp(tok, OPERATORS[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

int expandPathVar(char *target, const char **out)
{
    if(validatePath(*out))  //if the path already points to a correct program
    {
        free((char*)*out);
        *out = realpath(*out, NULL);
        return 0;
    }
    else
    {
        DIR *dir;
        struct dirent *entry;
        char *path = strdup(getenv("PATH"));
        char *str = strtok(path, ":");
        char *fullpath;
        
        while (str != NULL)
        {
            if ((dir = opendir(str)) == NULL){
                perror("opendir");
                return 1;
            }

           errno = 0;
        while ((entry = readdir(dir)) != NULL)
        {
            if (entry->d_type == DT_REG && strcmp(entry->d_name, target)==0){
                fullpath = malloc(strlen(str)+strlen(target)+1);
                strcpy(fullpath, strcat(strcat(str, "/"), target));
                free((char*)*out);
                *out = fullpath;
                free (path);
                return 0;
            }
        }
        
        if (errno != 0){
            perror("readdir");
            errno = 0;
        }
        
        closedir(dir);
        str = strtok(NULL, ":");
    }
    free(path);
    fprintf(stderr, "specified file does'nt exist\n");
    return 1;
}
}

int validatePath(char *path)
{
    int fd;
    if((fd = open(path, O_RDONLY)) < 0)
    {
        errno = 0;
        return 0;
    }
    close(fd);
    return 1;
}

int validateKeyAndAdvbufptr(char *key_out, char **buf)
{
    char arg[16];
    int k = 0;
    while ((*buf)[k] != ' ' && (*buf)[k] != '\0'){
        arg[k] = (*buf)[k];
        k++;
    }

    arg[k] = '\0';

    if(k > 15 || k == 0)
        return 1
    ;

    for (size_t i = 0; i < BUILTINS_LEN; i++)
    {
        if (strcmp(arg, BUILTINS[i]) == 0)
        {
            strcpy(key_out, arg);
            *buf = (*buf+(k+1));
            return 0;
        }
    }

    fprintf(stderr, "no keyword matching\n");
    return 1;
}

// NEEDED LATER
/*
char *progname_ptr = strrchr(progpath, '/') == NULL ? progpath : strrchr(progpath, '/') + 1;
char *progname = strdup(progname_ptr);
*/
Pipe *assignPipeline(Token *tokens[], int len)
{
    int cntr = 0;
    Pipe *pipeline = malloc(sizeof(Pipe));
    Command *current = &pipeline->cmd[cntr]; 

    for (size_t i = 0; i < len; i++)
    {
        switch ((*tokens)[i].type)
        {
        case WORD:

            break;
        case PIPE:

            break;
        case REDIRECT_FROM:

            break;
        case REDIRECT_TO:

            break;
        case APPEND_TO:

            break;
        default:
            break;
        }
    }
    return pipeline;
}

int tokenize(char **buf, Token **token_list_out[], size_t *len_out)
{
    Token *tokens = malloc(sizeof(Token)*25);
    char bufcpy[1000];
    strcpy(bufcpy, *buf);

    int key, k = 0;
    char *tok = strtok(bufcpy, " ");
    while (tok != NULL)
    {

        if ((key = checkToken(tok)) == WORD)
        {
            tokens[k].name = strdup(tok);
            tokens[k].type = WORD;
        }
        else
        {
            tokens[k].type = key;
        }

        tok = strtok(NULL, " ");
        k++;
    }

    *token_list_out = realloc(tokens, sizeof(Token)*(k+1));
    *len_out = k+1;

    return 0;
}

void cleanUp(char *const **pathargs, const char **path)
{
    int index = 0;
    
    while((*pathargs)[index] != NULL)
    {
        free((*pathargs)[index++]);
    }
    free((char *)*pathargs);
    free((char *)*path);

}

int ExecCmds(Token *tokens[], size_t *index)
{
   
    return 0;
}

int main(int argc, char const *argv[])
{
    Token *token_ptrs[1];
    size_t token_len;
    char *const *pathargs;
    const char *path;
    char buf[1000], key[16] = "\0";
    char *bufptr;
    int keyswitch;

    while (1)
    {
        buf[0] = '\0';
        key[0] = '\0';
        scanf(" %[^\n]", buf);
        bufptr = buf;
        if ((keyswitch = validateKeyAndAdvbufptr(key, &bufptr)) != 0)
        {
            continue;
        }

        switch (keyswitch)
        {
        case EXEC:
            do
            {
                tokenize(&bufptr, &token_ptrs, &token_len);
                expandPathVar(pathargs[0], &path);
                
                for (size_t i = 0; i < token_len; i++)
                {
                    
                }
                
                cleanUp(&pathargs, &path);

            } while (1);
            break;
        case CHDIR:

            break;
        case EXIT_APP:
            abort();
            break;
        default:
            fprintf(stderr, "keyswitch reached default case");
            return 1;
            break;
        }

        puts("end of loop successful");
    }
}
