#!/usr/bin/env python3

# This script helps us build a Makefile rule for a C file.
# It reads the C source, finds all #include lines, and then writes a target
# such as:
#
# my_program: my_program.c my_header.h
#     $(CC) $(CFLAGS) -c my_program.c -o my_program.o
#
# The goal is to make the build system know which files the C file depends on.
# This is useful because if an included header changes, Make can rebuild the file.

import sys
import re
import os


# A set is used because we only care about unique include names.
# Example: if the same header is included twice, we only keep it once.
def extract_includes(filepath):
    """Read a C file and return all header files it includes.

    Example:
        #include "my_header.h"
        #include <stdio.h>

    The function returns a Python set like:
        {'my_header.h', 'stdio.h'}
    """
    includes = set()

    try:
        # Open the C file in text mode so we can read it line by line.
        with open(filepath, 'r') as f:
            for line in f:
                # We remove leading/trailing spaces before checking the line.
                cleaned_line = line.strip()

                # This regular expression matches a line like:
                #   #include "header.h"
                #   #include <stdio.h>
                #
                # Pattern explanation:
                # - #include  -> the literal text "#include"
                # - \s+       -> one or more spaces
                # - [<"]     -> either < or "
                # - ([^>"]+) -> capture everything up to > or "
                # - [>"]     -> closing > or "
                match = re.match(r'#include\s+[<"]([^>"]+)[>"]', cleaned_line)

                if match:
                    # match.group(1) is the text inside the quotes or angle brackets.
                    # Example: for '#include "header.h"' it gives 'header.h'.
                    includes.add(match.group(1))

    except FileNotFoundError:
        # If the file is not found, show a friendly error and stop the script.
        print(f"Error: File {filepath} not found", file=sys.stderr)
        sys.exit(1)

    return includes


# This function creates a Make target string.
# A Make target looks like:
#   target_name: dependency1 dependency2
#       command to build it
#
# Example:
#   main: main.c my_header.h
#       $(CC) $(CFLAGS) -c main.c -o main.o

def generate_make_target(c_file, includes):
    """Create the target rule and the command used to build a .c file."""
    # os.path.splitext('main.c') returns ('main', '.c')
    # We remove the last extension to get 'main'.
    base_name = os.path.splitext(c_file)[0]

    # os.path.basename('folder/main') returns 'main'
    # This keeps just the file name without any directory.
    target_name = os.path.basename(base_name)

    # Build the dependency list.
    # Start with the C file itself, then add each included header.
    # sorted(...) makes the list easier to read and predictable.
    deps = [c_file] + sorted(list(includes))

    # Join the list with spaces so it becomes a proper Make dependency list.
    deps_str = ' '.join(deps)

    # Create the target rule text.
    make_target = f"{target_name}: {deps_str}\n"

    # The build command compiles the C file into an object file.
    # The object file will be named target_name.o.
    make_target += f"\t$(CC) $(CFLAGS) -c {c_file} -o {target_name}.o\n"

    return target_name, make_target


# This function updates the Makefile.
# It checks whether a target already exists, and either replaces it or appends it.
def update_makefile(c_file, makefile_path="../makefile"):
    """Update the Makefile with a target for the given C file."""
    # The script expects the C file to live in a folder like ../test/
    # Example: if c_file is 'main.c', the actual source file path is '../test/main.c'.
    filepath = f"../test/{c_file}"

    # Step 1: read the C file and find its header dependencies
    includes = extract_includes(filepath)

    # Step 2: turn that information into a make rule
    target_name, make_target = generate_make_target(c_file, includes)

    # Step 3: read the existing Makefile if it exists.
    makefile_content = ""
    if os.path.exists(makefile_path):
        with open(makefile_path, 'r') as f:
            makefile_content = f.read()

    # We will look for a target that begins with: "some_name: "
    # re.MULTILINE means the '^' and '$' checks each line separately.
    target_pattern = re.compile(rf"^{target_name}:\s", re.MULTILINE)

    # If the target already exists, replace the old block.
    if target_pattern.search(makefile_content):
        # This regex finds the whole existing rule block for this target.
        # It looks for:
        #   target_name: ...
        # followed by lines until the next non-tab-indented line or end of file.
        #
        # Example match:
        #   main: main.c header.h
        #       $(CC) $(CFLAGS) -c main.c -o main.o
        makefile_content = re.sub(
            rf"^{target_name}:.*?(?=\n[^\t]|\Z)",
            make_target.rstrip(),
            makefile_content,
            flags=re.MULTILINE | re.DOTALL
        )
    else:
        # If this target is new, append it to the end of the Makefile.
        # We add a newline before appending if the file doesn't already end with one.
        if makefile_content and not makefile_content.endswith('\n'):
            makefile_content += '\n'

        makefile_content += make_target

    # Save the result back to the Makefile.
    with open(makefile_path, 'w') as f:
        f.write(makefile_content)

    return target_name


# This is the main part of the script.
# It runs only when this file is executed directly.
if __name__ == "__main__":
    # Python scripts receive command-line arguments in sys.argv.
    # sys.argv[0] is the script name itself.
    # sys.argv[1] is the first real argument, like 'main.c'.
    if len(sys.argv) != 2:
        print("Usage: automake.py <file.c>", file=sys.stderr)
        sys.exit(1)

    c_file = sys.argv[1]

    # Update the Makefile and get the target name we created.
    target = update_makefile(c_file)

    # Print the target name to the terminal.
    print(target)
