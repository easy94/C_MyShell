#include "myheader.h"

int main(int argc, char const *argv[])
{
    Token **token_ptrs;
    size_t token_len;
    Pipe *pip;
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
                tokenize(&bufptr, &token_ptrs, &token_len);
                pip = assignPipeline(token_ptrs, token_len);
                //find the progname in $PATH, expand and save to cmd->cmd
                for (size_t i = 0; i < pip->cntr; i++){
                    if(expandPathVar(pip->cmd[i]->args[0], &pip->cmd[i]->cmd)){
                        cleanUpTokFail(token_ptrs, token_len); 
                        goto leave;
                    }
                }
                execCmds(pip);
                cleanUpTokSucc(token_ptrs, token_len);
                cleanUpPipe(pip);
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
        leave:
        puts("end of loop successful");
    }
}
