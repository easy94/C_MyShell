#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <dirent.h>
#include <wctype.h>

char *KEYWORDS[2] = 
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
int myInputCheck(char *buf)
{
    if(strlen(buf) == 0){
        return 0;
    }

    size_t len = sizeof(KEYWORDS)/sizeof(char*);
    char keyword[256];
    int index;
    char tok[1000];
    char *loc;
    
    sscanf(buf, " %s %[^\n]", keyword, tok);

    printf("%s\n", keyword);
    printf("%s\n", tok);
    size_t buf_len = strlen(buf);

    int i = 0;
    while (!iswspace(buf[i++])){}

    //after this while: k is index of first tok char
    //and k-1 is first whitespace
    //so compare first k chars for keyword

            if(strncpy_s(keyword, buf, i))
            {
                fprintf(stderr, "something went wrong in strncpy");
                return 1;
            }
            if(memcpy_s(tok, buf[i], buf_len-i)){
                fprintf(stderr, "something went wrong in memcpy_s");
                return 1;
            }

            printf("%s\n", tok);
            prinft("%s\n", keyword);

            for (size_t j = 0; j < len; j++)
            {
                if(!strncmp(keyword, KEYWORDS[j], i-1)){
                    index = j;
                }
            }

    if(index == 0){
        myExecFunc();
        printf("the rest of the string was: %s\n", tok);
        return 0;
    }
    else if(index == 1){
        return 0;
    }
    else{
        fprintf(stderr, "unknown command keyword: %");
        return 1;
    }
    

    return 0;
}

int main(int argc, char const *argv[])
{
    int error;
    char buf[1000];
    scanf(" %[^\n]", buf);
    printf("%s\n", buf);
    if((error = myInputCheck(buf))){
        fprintf(stderr, "myInputCheck returned error");
        return 1;
    }

    /* struct dirent *entry;
    DIR *dir = opendir("./testbin");
    while ((entry = readdir(dir)) != NULL)
    {
        if(entry->d_name != "." && entry->d_name != ".."){
            printf("filename: %s\n", entry->d_name);
        }
    } */
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

