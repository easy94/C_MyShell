#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <dirent.h>
#include <wctype.h>

enum KEYS{
    EXEC = 0,
    CHDIR = 1,
};

static const char *KEYWORDS[2] = 
{
    "exec",             //execute bin/script    0
    "chdir"             //change directory      1
};

int myExecFunc(){
    printf("keyword found: exec!\n");
    return 0;
}

//check which keyword is in first part of prompt.
//user prompt should look like: 
//  [SHELL KEYWORD] [KEYWORD ARG aka. file/dir] [arguments for keyword arg aka. almost irrelevant for me]
int myInputCheck(char *buf, char *key, char *key_arg)
{
    if(strlen(buf) == 0){
        return 0;
    }

    size_t len = sizeof(KEYWORDS)/sizeof(char*);
    char keyword[256];
    char tok[1000];
    int index = 0;
    int error = 0;
    
    sscanf(buf, " %s %[^\n]", keyword, tok);

    printf("%s\n", keyword);
    printf("%s\n", tok);

    error = 1;
    for (size_t j = 0; j < len; j++)
    {
        if (strcmp(keyword, KEYWORDS[j]) == 0)
        {
            error = 0;
        }
    }

    if(error != 0)
        return 1
    ;

    if(index == EXEC){

        myExecFunc();
        printf("the rest of the string was: %s\n", tok);
        return 0;
    }
    else if(index == CHDIR){
        
        return 0;
    }
    else{
        fprintf(stderr, "unknown command keyword:");
        return 1;
    }
    
    return 0;
}

int main(int argc, char const *argv[])
{
/*     //imitate the PATH variable processing with testbin
    struct dirent *entry;
    DIR *dir = opendir("./testbin");
    while ((entry = readdir(dir)) != NULL)
    {
        if(entry->d_name != "." && entry->d_name != ".."){
            printf("filename: %s\n", entry->d_name);

        }
    } */

    while (1)
    {
        
        char *key = malloc(0);
        char *key_arg = malloc(0);
        
        int error;
        char buf[1000];
        scanf(" %[^\n]", buf);
        printf("%s\n", buf);
        if((error = myInputCheck(buf, key, key_arg))){
            fprintf(stderr, "myInputCheck returned error");
            return 1;
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

