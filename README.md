# Simple Shell

This is a simple UNIX shell program written in C. It demonstrates core shell functionality like:

- Executing commands
- Redirecting input/output
- Piping between commands 
- Command sequences
- Path searching

## Features

The shell supports the following features:

- **Executing commands**: Commands entered by the user are parsed into arguments and executed using `fork()` and `exec()`.

- **I/O redirection**: The shell interprets redirection operators `>`, `<` and `2>` for stdout, stdin and stderr respectively.

- **Pipelines**: The `|` operator pipes the stdout of one command to the stdin of the next command.

- **Command sequences**: Commands separated by `;` are executed sequentially.

- **Path searching**: Executables are searched for in the PATH environment variable.

- **Built-in commands**: `wc` is implemented as a built-in command.

## Usage

To build:

```
make
``` 

To run:

```
./shell
```

Type commands and press enter to execute. Use `ctrl-c` or `ctrl-d` to exit.

## Code Overview

- `main()` - The main loop reading input and executing commands using `run()`

- `run()` - Parses pipelines, sequences and executes simple commands

- `command()` - Parses and executes a single command with I/O redirection

- `wc()` - Built-in `wc` command implementation
 
- `sequence()` - Executes commands sequentially separated by `;`

- `pipeline()` - Sets up pipe and executes pipeline with two commands

## References

This project was implemented as an assignment for a university operating systems course. The goal was to understand UNIX shell internals like forking processes, executing programs, redirecting I/O and piping.
