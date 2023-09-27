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

## Code Overview

- `main()` - The main loop reading input and executing commands using `run()`

- `run()` - Parses pipelines, sequences and executes simple commands

- `command()` - Parses and executes a single command with I/O redirection

- `wc()` - Built-in `wc` command implementation
 
- `sequence()` - Executes commands sequentially separated by `;`

- `pipeline()` - Sets up pipe and executes pipeline with two commands

## Usage 

To build:

```
gcc main.c -o shell
```

To run:

```
./shell
```

When running, it provides an interactive prompt `ssh361>` where you can enter commands.

## Examples

Here are some example runs and outputs:

```
ssh361> ls
file1.txt file2.txt

ssh361> echo "Hello World"
Hello World

ssh361> wc < file1.txt
13 25 152

ssh361> grep "hello" file2.txt > output.txt

ssh361> sort file1.txt | uniq
...

ssh361> uname; uptime; pwd
Linux
12:00:00 up 2 days
/home/user
```

## Implementation 

- The shell was developed on Ubuntu 20.04 in a Docker container.

- It is implemented in C using libraries like `unistd`, `stdlib`, `sys/wait` etc.

## References

This project was implemented as an assignment for a university operating systems course. The goal was to understand UNIX shell internals like forking processes, executing programs, redirecting I/O and piping.
