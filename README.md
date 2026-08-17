# notes

- FIRST OF ALL, there must be builtin command KEYWORDS that only exists for my shell:
The first ones included should be: chdir(changedirectory), exec(execute a bin/script or open a txtfile)

## Read input

- first i need to seperate the execution program from the argument part of the cmd prompted by user.
For this reason i chose the keyword "exec" as the option for taking a mandatory file argument.
So ill have to first make sure its a file that can be executed aka. binary, sript.
The 2nd problem is the arguments must be somehow piped to the bin/script being exectuted.
To check if exec *argument* is indeed a file, ill use dirent to find it in my experimental testbin directory.

## Execute input

## Print result
