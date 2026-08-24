#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/wait.h>

enum KEYS
{
    EXEC = 0,
    CHDIR = 1,
    EXIT_APP = 2,
};

unsigned char pipeflag = 0;

static const char *KEYWORDS[3] =
    {
        "exec",  // execute bin/script    0
        "chdir", // change directory      1
        "exit"   // exit program          2
};
static const size_t KEYWORDLEN = 3;

int getFullPath(char *target, const char **out)
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
                puts("found");
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

int validateKeyAndAdvbufptr(char **key_out, char **buf)
{
    size_t len = strlen(key_out);
    if(len > 15 || len == 0)
        return 1
    ;
    char arg[16];
    int k = 0;
    while (*buf[k] != " " && *buf[k] != '\0')
        arg[k] = *buf[k]
    ;

    for (size_t i = 0; i < KEYWORDLEN; i++)
    {
        if (strcmp(arg, KEYWORDS[i]) == 0)
        {
            strcpy(*key_out, arg);
            *buf = *(buf+(len+1));
            return 0;
        }
    }

    fprintf(stderr, "no keyword matching");
    return 1;
}

int tokenize(char **buf, const char **path_out, char *const **args_out)
{
    const char **arrargs = malloc(sizeof(char *) * 32);
    char *strpath, *str;
    int i = 1;
    size_t len = 0;
    char *tok;
    unsigned int advbufsum=0; 
    //path
    char *bufcpy;           //dont touch buf cause we need it to traverse through all pipes
    strcpy(bufcpy, *buf);
    tok = strtok(bufcpy, " ");
    len = strlen(tok)+1;
    strpath = malloc(len);          //*******
    strcpy(strpath, tok);

    *path_out = strpath;
    
    // pathargs + filename
    tok = strtok(NULL, " ");            //advance the strtok and keep going with it 0|0str
    while (tok != NULL && tok != "|")
    {
        len = strlen(tok) + 1;
        advbufsum += len;
        str = malloc(len);
        strcpy(str, tok);
        arrargs[i++] = str;
        tok = strtok(NULL, " ");
    }

    if(tok == "|"){
        pipeflag = 1;
    }
    else{
        pipeflag = 0;
    }
    arrargs[0] = strrchr(strpath, '/') == NULL? strpath:strrchr(strpath, '/')+1;
    arrargs[i] = NULL;
    *args_out = realloc(arrargs, (i + 1) * sizeof(char *));
    *buf = *(buf+(advbufsum));

    return 0;
}

void cleanUp(char *const **pathargs, const char **path)
{
    int index = 1;

    while(*pathargs[index] != NULL)
    {
        free(*pathargs[index++]);
    }
    free((char *)*pathargs);
    free((char *)*path);

}

void callChild(char *const **pathargs, const char **path)
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
        if(pipeflag)
        {
            dup2(pfd[1], STDOUT_FILENO);
        }
        close(pfd[0]);
        close(pfd[1]);
        if (execv(*path, *pathargs))
            perror("execv");
        _exit(0);
    }
    else
    {
        if(pipeflag)
        {
            dup2(pfd[0], STDIN_FILENO);
        }
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
}

int main(int argc, char const *argv[])
{
    char *const *pathargs;
    const char *path;
    char buf[1000], key[16] = "\0";
    int index = 1; // index = 1 because pointer shennanigans in tokenize()
    char *bufptr;
    int keyswitch;

    while (1)
    {
        buf[0] = '\0';
        key[0] = '\0';
        scanf(" %[^\n]", buf);
        bufptr = buf;
        if (validateKeyAndAdvbufptr(&key, &bufptr) != 0)
        {
            continue;
        }

        switch (keyswitch)
        {
        case EXEC:
            do
            {
                tokenize(&bufptr, &path, &pathargs);
                getFullPath(pathargs[0], &path);
                callChild(&path, &pathargs);
                cleanUp(&pathargs, &path);

            } while (pipeflag);
            break;
        case CHDIR:
            
            break;
        case EXIT_APP:
            abort();
            break;
        default:
            break;
        }

        puts("end of loop successful");
    }
}
