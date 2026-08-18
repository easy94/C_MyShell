#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <wctype.h>

enum KEYS{
    EXEC = 0,
    CHDIR = 1,
    EXIT_APP = 2,
};

static const char *PATHS[2] = 
{
    "/home/lucas/VSCProjects/C_MyShell/anothertestbin/",
    "/home/lucas/VSCProjects/C_MyShell/testbin/"
};

static const char *KEYWORDS[3] = 
{
    "exec",             //execute bin/script    0
    "chdir",             //change directory      1
    "exit"              //exit program          2
};

int searchInPathVar(char *path)
{
    // imitate the PATH variable processing with testbin
    struct dirent *entry;
    printf("%s\n", path);
    for (size_t i = 0; i < sizeof(KEYWORDS)/sizeof(char*); i++)
    {
        DIR *dir;
        if((dir = opendir(PATHS[i])) == NULL)
        {
            perror("opendir:");
            return 1;
        }

        while((entry = readdir(dir)) != NULL)
        {
            if(entry->d_name == path){
                return 0;
            }
        }  
    }

    fprintf(stderr, "specified file does'nt exist");
    return 1;

    
}

//check which keyword is in first part of prompt.
//user prompt should look like: 
//  [SHELL KEYWORD] [KEYWORD ARG aka. file/dir] [arguments for keyword arg aka. almost irrelevant for me]
int myInputCheck(char *buf, char **key, char **path, char **path_arg)
{
    if(strlen(buf) == 0){
        return 0;
    }

    size_t len = sizeof(KEYWORDS)/sizeof(char*);
    char cmd[256];
    char prog[1000];
    char prog_arg[1000];
    int error = 0;
    
    sscanf(buf, " %s %s %[^\n]", cmd, prog, prog_arg);


    error = 1;
    for (size_t j = 0; j < len; j++)
    {
        if (strcmp(cmd, KEYWORDS[j]) == 0)
        {
            error = 0;
        }
    }

    if(error != 0){
        switch (error)
        {
            case EXEC:
            fprintf(stderr, "exec some went wrong");
            break;
            case EXIT_APP:
            fprintf(stderr, "exit some went wrong");
            break;
            case CHDIR:
            fprintf(stderr, "chdir some went wrong");
            break;
            default:
            break;
        } 
        return 1;
    }

    *key = realloc(*key, strlen(cmd)+1);
    strcpy(*key, cmd);
    *path = realloc(*path, strlen(prog)+1);
    strcpy(*path, prog);
    *path_arg = realloc(*path_arg, strlen(prog_arg)+1);
    strcpy(*path_arg, prog_arg);

    printf("%s\n", *key);
    printf("%s\n", *path);
    printf("%s\n", *path_arg);

    return 0;
}

int main(int argc, char const *argv[])
{
    char *key = malloc(0), *path = malloc(0), *path_arg = malloc(0);
    char buf[1000];
    int error = 0;
    char const *ptrpath = path;
    char *const ptrpath_arg = path_arg;

    while (1)
    {
        scanf(" %[^\n]", buf);
        if((error = myInputCheck(buf, &key, &path, &path_arg))){
            fprintf(stderr, "myInputCheck returned error");
            error = 0;
        }

        searchInPathVar(path);  //see static variable PATH for testing

        //only then start forking
        pid_t cpid;

        if ((cpid=fork())==-1)
        {
            perror("fork");
            return 1;
        }
        
        if (cpid == 1)
        {
            execv(ptrpath, &ptrpath_arg);
        }
        else
        {
            
        }
        
        

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

