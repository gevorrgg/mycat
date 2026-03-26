## mycat

C implementation of the Unix cat command with safe I/O and error handling using only syscalls.

## Description

mycat reads the content of a file and writes it to standard output (terminal).
It handles interrupted system calls (EINTR) and other read/write errors safely.

## Usage
``` bash
./mycat <filename>
```

## Example:

``` bash
./mycat example.txt
````

## Error Handling

mycat prints error messages to standard error in case of:

- Improper argument count
- File open errors
- Read errors
- Write errors

## Exit codes:

- 1 — Improper argument count
- 2 — File open error
- 3 — Read error
- 4 — Write error
- 5 — Unexpected write result
- 6 — Error closing file

## Compilation
``` bash
gcc -o mycat main.c
```
