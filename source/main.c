#include "myheader.h"

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
