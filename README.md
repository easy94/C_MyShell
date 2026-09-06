# notes

Interpret commandline input via predefined keywords such as "|", "exec", etc.
The user input string is traversed and tokenized as long as there is a | argument found.
This is handled via constant globals like "pipeflag" to set as while condition.
After each loop the variables must be freed and reassigned with the new program and arguments strings.

## Read input

Parse commands found in Keyvalidation and tokenize into the Path and Pathargs variables.
This is mostly done using strtok() and requiring the user to follow standard rules on commandline usage.

REWRITE THE TOKENIZER: do the whole string at once and parse each cmd+args sequence in a struct cmd.

## Execute input

CallChild first forks, then pipes stdout of child to stdin of its parent as long as the pipeflag is set.
Meanwhile the output of the child buffered in stdin.
If the pipeflag is not set, the child executes the program and uses the inputbuffer as its input without
piping the output, printing the result to screen.


### TODO

- arithmetic expression interpreting
- variable setting
- redirecting input >> << > < 2>
- aliases
- prompt customization?
- (.) expanding doesnt exist yet.(expanding in general) -> aliasing first.