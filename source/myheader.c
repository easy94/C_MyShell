#include "myheader.h"

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
    int argc = 0;
    char *const argv[10];
    Pipe *pipeline = malloc(sizeof(Pipe));
    Command *current = &pipeline->cmd[cntr]; 

    for (size_t i = 0; i < len; i++)
    {
        switch ((*tokens)[i].type)
        {
        case WORD:
            if(argc == 0){
                current[i].cmd = tokens[i]->name;
            }
            else
            {
                *(argv)[argc++] = tokens[i]->name;
            }
            
            break;
        case PIPE:
            *(argv)[argc] = NULL;
            argc = 0;
            cntr++;
            current = &pipeline->cmd[cntr];
            break;
        case REDIRECT_FROM:
            current[i].file_in = tokens[++i]->name;
            break;
        case REDIRECT_TO:
            current[i].file_out = tokens[++i]->name;
            break;
        case APPEND_TO:
            current[i].file_out = tokens[++i]->name;
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
