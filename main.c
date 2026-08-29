#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/wait.h>

enum OPERATOR
{
    PIPE = 0,
    REDIRECT_TO = 1,
    REDIRECT_FROM = 2,
    APPEND_TO = 3,
    REGULAR = 4,
};

enum BUILTIN
{
    EXEC = 0,
    CHDIR = 1,
    EXIT_APP = 2,
};

static const char *BUILTINS[3] =
    {
        "exec",  // execute bin/script    0
        "chdir", // change directory      1
        "exit"   // exit program          2
};
static const size_t BUILTINS_LEN = 3;

static const char *OPERATORS[5] =
{
    "|", ">", "<", ">>", ""
};

static const size_t OPERATORS_LEN = 5;

int expandPathVar(char *target, const char **out)
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


        /*
            SOMEWHERE HERE HAS TO BE CHECKED IF THE SPECIFIED PATH IS ALREADY A FULL PATH THAT POINTS TO A
            PROGRAM OUTSIDE OF PATH VAR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        */

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

int getFullPath()
{

    return 0;
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

//make a struct here. first im reading typedef
typedef struct {
    char *file_in;      // NULL equals stdin
    char *file_out;     // NULL equals stdout
    char *cmd;          //the program to execute
    char **args;        //the argv arr
    int append_flag;
}Command;

typedef struct {
    Command cmd;
}Pipe;

typedef struct {
    int type;               //see OPERATOR enum
    const char *path;       //program name path
    char *const *args;      //program arguments and options
}Token;

int isRedirectOp(char *tok)
{
    for (size_t i = 0; i < OPERATORS_LEN; i++)
    {
        if (strcmp(tok, OPERATORS[i]) == 0)
        {
            return i;
        }
    }
    return 0;
}

int tokenize(char **buf, Token ***token_list)
{
    int k = 0;                  //loop counter and cmd counter = i+1
    size_t len = 0;             //utility variable. '\0' is included

    Token tokens[25];

    char bufcpy[1000];
    strcpy(bufcpy, *buf);
    
    char *tok;
    tok = strtok(bufcpy, " ");
    len = strlen(tok)+1;
    char *progpath = malloc(len);
    strcpy(progpath, tok);
    tokens[0].path = progpath;
    
    const char **arguments;
    char *args_element;
    
    
    int loop_conditional = 0;
    while (loop_conditional == 0)
    {
        if(progpath == NULL) //set it null after each iteration
        {
            len = strlen(tok)+1;
            progpath = malloc(len);
            strcpy(progpath, tok);
            tokens[k].path = progpath;
        }
        
        if((tok = strtok(NULL, " ")) == NULL)
        {
            tokens[k].type = REGULAR;
            break;
        }
        
        int key = REGULAR;
        arguments = malloc(sizeof(char *) * 32);
        int i = 1;
        while (tok != NULL)
        {
            if ((key = isRedirectOp(tok)))
            {
                tokens[k].type = key;
                tok = strtok(NULL, " ");
                len = strlen(tok) + 1;
                args_element = malloc(len);
                strcpy(args_element, tok);
                arguments[++i] = args_element;
                break;
            }
            else if((key = strcmp(tok, OPERATORS[PIPE])) == 0)
            {
                tokens[k].type = PIPE;
                tok = strtok(NULL, " ");
                break;
            }
            else
            {
                len = strlen(tok)+1;
                args_element = malloc(len);
                strcpy(args_element, tok);
                arguments[i] = args_element;
                tok = strtok(NULL, " ");
                i++;
            }
        }

        char *progname_ptr = strrchr(progpath, '/') == NULL ? progpath : strrchr(progpath, '/') + 1;
        char *progname = strdup(progname_ptr);

        arguments[0] = progname;
        arguments[i] = NULL;
        tokens[k].args = realloc(arguments, (i + 1) * sizeof(char *));

        switch (key)
        {
        case PIPE:
            loop_conditional = 0;
            break;
        default:
            loop_conditional = 1;
            break;
        }

        progpath = NULL;
        k++;
    }

    Token *tokken = malloc(sizeof(Token)*(k+1));
    for (size_t i = 0; i < k; i++)
    {
        tokken[i].args = tokens[i].args;
        tokken[i].path = tokens[i].path;
        tokken[i].type = tokens[i].type;
    }
    

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

int pipeExec(char *const **pathargs, const char **path)
{
    int pfd[2];
    pid_t cpid;
    int wstatus;
    pipe(pfd);

    if ((cpid = fork()) == -1)
    {
        perror("fork child");
        return 1;
    }

    if (cpid == 0)
    {
        dup2(pfd[1], STDOUT_FILENO);
        close(pfd[0]);
        close(pfd[1]);
        execv(*path, *pathargs);

        perror("execv");
        _exit(0);
    }
    else
    {

        dup2(pfd[0], STDIN_FILENO);
        close(pfd[0]);
        close(pfd[1]);
        if (wait(&wstatus) < 0)
        {
            if (WIFEXITED(wstatus))
            {
                puts("wait returned -1");
                printf("%d\n", WEXITSTATUS(wstatus));
            }
        }
    }
    return 0;
}

int main(int argc, char const *argv[])
{
    Token **token_ptrs;
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
                tokenize(&bufptr, &token_ptrs);
                expandPathVar(pathargs[0], &path);
                pipeExec(&pathargs, &path);
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
