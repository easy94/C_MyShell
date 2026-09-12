#include <stdio.h>
#include "/home/lucas/VSCProjects/C_MyShell/source/myheader.h"
#include "/home/lucas/VSCProjects/C_MyShell/source/vars.h"
#include "/home/lucas/VSCProjects/C_MyShell/source/builtins.h"

int main(int argc, char const *argv[])
{
    varCreate();
    assignVar("pimmel=berger");
    puts(varGet("pimmel"));
    return 0;
}
