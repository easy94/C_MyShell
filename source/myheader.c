#include "myheader.h"

const int MAXSIZE = 15;
const size_t BUILTINS_LEN = 3;
const size_t OPERATORS_LEN = 5;

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

int checkToken(char *tok)
{
    for (size_t i = 0; i < OPERATORS_LEN; i++)
    {
        if (strcmp(tok, OPERATORS[i]) == 0)
        {
            return i;
        }
    }
    return 4;
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

int validatePath(const char *path)
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

void checkArgCapacity(Command *curr)
{
    if(curr->args_count == curr->args_capacity)
    {
        curr->args = realloc(curr->args, sizeof(char*)*2*curr->args_capacity);
        curr->args_capacity *= 2;
    }
}

void addArgCmd(Command *curr, int *cntr, char *arg)
{
    checkArgCapacity(curr);
    curr->args[(*cntr)++] = arg;
    curr->args_count++;
}

Pipe *assignPipeline(Token *tokens[], int len)
{
    int cntr = 0;
    int argc = 0;
    Pipe *pipeline = malloc(sizeof(Pipe));  // pipe struct has fixed cmdarray size
    for (size_t i = 0; i < 15; i++)
    {
        Command *c = malloc(sizeof(Command));
        pipeline->cmd[i] = c;                             //these must be all freed afterwards.//So: pipeline->cmd->args->allelementsonebyone 
        pipeline->cmd[i]->args = malloc(sizeof(char*));
        pipeline->cmd[i]->args_capacity = 1;
        pipeline->cmd[i]->file_in = NULL;
        pipeline->cmd[i]->file_out = NULL;
        pipeline->cmd[i]->args_count = 0;
        pipeline->cmd[i]->append_flag = 0;
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
                char *progname_ptr = strrchr(tokens[i]->name, '/') == NULL ? tokens[i]->name : strrchr(tokens[i]->name, '/') + 1;
                char *progname = strdup(progname_ptr);
                addArgCmd(current, &argc, progname);
            }
            else
                addArgCmd(current, &argc, tokens[i]->name);

            if(!(i < len -1)){
                addArgCmd(current, &argc, NULL);
            }
            //make individual ptrs for pathofprogram and programname and truncate if user specified fullpath
            break;
        case PIPE:
            addArgCmd(current, &argc, NULL);
            argc = 0;
            current = pipeline->cmd[++cntr];
            pipeline->cntr++; 
            break;
        case REDIRECT_FROM:
            current->file_in = tokens[++i]->name;
            break;
        case REDIRECT_TO:
            current->file_out = tokens[++i]->name;
            checkArgCapacity(current);
            current->args[argc++] = NULL;
            current->args_count++;
            break;
        case APPEND_TO:
            current->file_out = tokens[++i]->name;
            current->append_flag = 1;
            break;
        default:
            break;
        }
    }
    return pipeline;
}

int tokenize(char **buf, Token **token_list_out[], size_t *len_out)
{
    *token_list_out = malloc(sizeof(Token*)*25);
    Token *token;
    char bufcpy[1000];
    strcpy(bufcpy, *buf);

    int key, k = 0;
    char *tok = strtok(bufcpy, " ");
    while (tok != NULL)
    {
        token = malloc(sizeof(*token));
        if ((key = checkToken(tok)) == WORD)
        {
            token->name = strdup(tok);
            token->type = WORD;
        }
        else
        {
            token->type = key;
        }
        (*token_list_out)[k] = token;
        tok = strtok(NULL, " ");
        k++;
    }

    *token_list_out = realloc(*token_list_out, sizeof(Token*)*(k));
    *len_out = k;

    return 0;
}

void cleanUpTokSucc(Token **tok, size_t tok_len)
{
    for (size_t i = 0; i < tok_len; i++)
    {
        free(tok[i]);
    }
    free(tok);
}


void cleanUpTokFail(Token **tok, size_t tok_len)
{
    for (size_t i = 0; i < tok_len; i++)
    {
        if(tok[i]->type == WORD)
            free(tok[i]->name);

        free(tok[i]);
    }
    free(tok);
}

void cleanUpPipe(Pipe *pipeline)
{
    int l = pipeline->cntr;
    for (size_t i = 0; i < l; i++)
    {
        free(pipeline->cmd[i]->args[0]);
    }
    free(pipeline);
}

int execCmds(Pipe *cmds)
{
    size_t len = cmds->cntr;
    pid_t cid, errid;
    int wstatus;
    int fd[2];
    char *temp;
    size_t i;
    int old_fd0 = -1;

    
    for (i = 0; i < len; i++)
    {
        if(pipe(fd) != 0){
            fprintf(stderr, "pipe failed");
            abort();
        }

        cid = fork();
        switch (cid)
        {
        case 0:
            int in = -1;    //
            int out = 0;
            //check if cmd takes input from file and write it to stdin
            if((temp = cmds->cmd[i]->file_in) != NULL){
                if((in = open(temp, O_RDONLY)) < 0){
                    perror("child_in open");
                    errno = 0;
                    abort();
                }
                dup2(in, STDIN_FILENO);
            }
            temp = NULL;    //for reuse

            //check if cmd outputs to file instead of piping
            if ((temp = cmds->cmd[i]->file_out) != NULL){
                out = cmds->cmd[i]->append_flag? 
                    open(temp, O_WRONLY | O_APPEND | O_CREAT, 0722) : open(temp, O_WRONLY | O_TRUNC | O_CREAT, 0722)
                ;
                if(out < 0){
                    perror("child_out open");
                    errno = 0;
                    abort();
                }
                dup2(out, STDOUT_FILENO);
                temp = NULL;
            }
            else // pipe
            {
                if(in < 0)
                { //if no inputfile was specified, take the pipe as input
                    if(old_fd0 > -1){
                        dup2(old_fd0, STDIN_FILENO);
                    }
                    else{
                        dup2(fd[0], STDIN_FILENO);
                    }
                    
                }
                if(i < len-1)   //last iteration: dont write to pipe
                {
                    dup2(fd[1], STDOUT_FILENO);
                }
            }
            if(old_fd0 > -1)
                close(old_fd0)
            ;
            close(fd[1]);
            close(fd[0]);
            execv(cmds->cmd[i]->cmd, cmds->cmd[i]->args);
            perror("execv child");
            errno = 0;
            abort();
        break;

        case -1:
            fprintf(stderr, "fork failed");
            _exit(EXIT_FAILURE);
            break;

        default:
            close(fd[1]);
            old_fd0 = fd[0];
            if ((errid = wait(&wstatus)))
            {
                if (!WIFEXITED(wstatus))
                {
                    abort();
                }
            }

            break;
        }
    }

    return 0;
}
