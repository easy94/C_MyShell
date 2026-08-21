#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
//#include <wctype.h>
#include <sys/wait.h>

enum KEYS
{
    EXEC = 0,
    CHDIR = 1,
    EXIT_APP = 2,
};

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

int tokenize(char *buf, char *key, const char **path, char *const **args)
{
    const char **arrargs = malloc(sizeof(char *) * 32);
    char *strpath, *str;
    char *tok;
    tok = strtok(buf, " ");
    int i = 0;
    size_t len = 0;

    // key + path
    while (tok != NULL && i < 2)
    {
        len = strlen(tok) + 1;
        if (i++ == 0)
        {
            if (len > 16)
            {
                fprintf(stderr, "argument 1: cmd exceeds length");
                return 1;
            }

            for (size_t i = 0; i < KEYWORDLEN; i++)
            {
                if(strcmp(tok,KEYWORDS[i])==0){
                    strcpy(key,tok);
                }
            }
            if(strlen(key)==0){
                fprintf(stderr, "no key matching");
                return 1;
            }
        }
        else
        {
            strpath = malloc(len);
            strcpy(strpath, tok);
            *path = strpath;
        }
        tok = strtok(NULL, " ");
    }

    // pathargs + filename
    i = 1;
    while (tok != NULL)
    {
        len = strlen(tok) + 1;
        str = malloc(len);
        strcpy(str, tok);
        arrargs[i++] = str;
        tok = strtok(NULL, " ");
    }
    arrargs[0] = strrchr(strpath, '/') == NULL? strpath:strrchr(strpath, '/')+1;
    arrargs[i] = NULL;
    *args = realloc(arrargs, (i + 1) * sizeof(char *));

    return 0;
}

int main(int argc, char const *argv[])
{
    char *const *pathargs;
    const char *path;
    char buf[1000], key[16] = "\0";
    int index = 1; // index = 1 because pointer shennanigans in tokenize()

    while (1)
    {
        scanf(" %[^\n]", buf);
        if (tokenize(buf, key, &path, &pathargs) == 0)
        {
            if(getFullPath(pathargs[0], &path) == 0)
            {

                
                // only then start forking
                
                pid_t cpid;
                int wstatus;
                
                if ((cpid = fork()) == -1)
                {
                    perror("fork child");
                    return 1;
                }
                
                if (cpid == 0)
                {
                    if(execv(path, pathargs)) perror("execv");
                    _exit(0);
                }
                else
                {
                    if(wait(&wstatus) < 0){
                        if(WIFEXITED(wstatus)){
                            puts("wait returned -1");
                            printf("%d\n",WEXITSTATUS(wstatus));
                        }
                    }
                }
                
                while (pathargs[index] != NULL)
                {
                    free(pathargs[index++]);
                }
                
                free((char *)pathargs);
                free((char *)path);
            }
        }
        puts("end of loop successful");
    }
}

/*
int opt;
int verbose = 0;
int flag_f = 0;
char *outfile = NULL;
long number = 0;

while ((opt = getopt(argc, (char * const *)argv, "hvfo:n:")) != -1) {
        switch (opt) {
            case 'h':
                printf("Usage: %s [-h] [-v] [-f] [-o outfile] [-n number] [--] [args...]\n", argv[0]);
                return 0;
                case 'v':
                verbose++;
                break;
                case 'f':
                flag_f = 1;
                break;
                case 'o':
                outfile = optarg;
                break;
                case 'n': {
                char *end = NULL;
                errno = 0;
                long v = strtol(optarg, &end, 10);
                if (errno || end == optarg || *end != '\0') {
                    fprintf(stderr, "invalid number for -n: %s\n", optarg);
                    return 2;
                }
                number = v;
                break;
            }
            default:
            fprintf(stderr, "Unknown option\n");
            return 1;
        }
    }

    printf("Options:\n");
    printf("  verbose = %d\n", verbose);
    printf("  flag_f  = %d\n", flag_f);
    printf("  outfile = %s\n", outfile ? outfile : "(none)");
    printf("  number  = %ld\n", number);

    if (optind < argc) {
        printf("Arguments:\n");
        for (int i = optind; i < argc; ++i)
        printf("  %s\n", argv[i]);
    }
    */
