#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>

#define exit(N)         \
    {                   \
        fflush(stdout); \
        fflush(stderr); \
        _exit(N);       \
    }

/**
 * @brief Prints the word, line and byte counts
 *
 * Prints the number of bytes, lines and words based on the given counts
 *
 * @param bsize Number of bytes
 * @param lsize Number of lines
 * @param wsize Number of words
 */
static void print_result(int bsize, int lsize, int wsize)
{
    printf("Num Bytes: %d\n", bsize);
    printf("Num Lines: %d\n", lsize);
    printf("Num Words: %d\n", wsize);
    fflush(stdout);
}

/**
 * @brief Implements the wc command
 *
 * Reads input from stdin and counts number of bytes, lines and words.
 * Prints the counts using print_result().
 */
void wc()
{

    int bsize = 0;  // byte size
    int lsize = 0;  // line size
    int wsize = 0;  // word size
    int inWord = 0; // flag to check if we are in a word
    char c;         // character to read in

    while (read(STDIN_FILENO, &c, 1) > 0)
    {            // read in one character at a time
        bsize++; // increment byte size

        if (c == '\n')
        {
            lsize++; // increment line size if we encounter a newline
        }
        if (c == ' ' || c == '\n' || c == '\t')
        {
            inWord = 0; // if we encounter a space, newline, or tab, we are not in a word
        }
        else if (!inWord)
        {               // if we are not in a word and we encounter a character, we are in a word
            inWord = 1; // set inWord to 1
            wsize++;    // increment word size
        }
    }

    print_result(bsize, lsize, wsize);
}

/**
 * @brief Parses and executes commands
 *
 * Parses the given command string into arguments and executes using execv after
 * applying any redirections.
 *
 * @param c The command string
 */
void command(char *c)
{

    // return without doing anything if only a newline is received
    if (c[0] == '\n')
    {
        return;
    } 
    
    // pointer to the first occurrence of a redirection operator in c. NULL if no redirection operator is found.
    char *sep = NULL;

    // if a redirection operator is found, the pointer sep will point to the first occurrence of the redirection operator. 
    if ((sep = strstr(c, "2>")))
    {
        *sep = 0; // truncate the string 
        char *fileout = sep + 2; // fileout will point to the file name after the redirection operator. 

        // Removing leading spaces or newline characters
        while (*fileout == ' ' || *fileout == '\n')
            fileout++;

        // strtok will return the first token in fileout, which is the file name.
        fileout = strtok(fileout, " \n");

        int fd_err = open(fileout, O_WRONLY | O_CREAT | O_TRUNC, 0644); // open the file for writing. 
        if (fd_err < 0) // if the file cannot be opened, print an error message. 
        {
            fflush(stderr); // for error 
            return; 
        }
        dup2(fd_err, STDERR_FILENO); // redirect stderr to the file descriptor fd_err.
        close(fd_err);               // close the file descriptor fd_err.
    }

    if ((sep = strstr(c, ">")))
    {
        *sep = 0; // truncate the string 
        char *fileout = sep + 1; // fileout will point to the file name after the redirection operator. 

        // Removing leading spaces or newline characters    
        while (*fileout == ' ' || *fileout == '\n')
            fileout++;

        // strtok will return the first token in fileout, which is the file name.
        fileout = strtok(fileout, " \n");

        int fd_out = open(fileout, O_WRONLY | O_CREAT | O_TRUNC, 0644); // open the file for writing. 
        if (fd_out < 0) // if the file cannot be opened, print an error message. 
        {
            fflush(stderr); // for error 
            return;
        }
        dup2(fd_out, STDOUT_FILENO); // redirect stdout to the file descriptor fd_out. 
        close(fd_out);               // close the file descriptor fd_out. 
    }

    if ((sep = strstr(c, "<")))
    {
        *sep = 0; // truncate the string 
        char *filein = sep + 1;

        // Removing leading spaces or newline characters
        while (*filein == ' ' || *filein == '\n')
            filein++;

        // strtok will return the first token in fileout, which is the file name.
        filein = strtok(filein, " \n");

        int fd_in = open(filein, O_RDONLY); // open the file for reading. 
        if (fd_in < 0) // if the file cannot be opened, print an error message. 
        {
            fflush(stderr);
            return;
        }
        dup2(fd_in, STDIN_FILENO); // redirect stdin to the file descriptor fd_in. 
        close(fd_in);              // close the file descriptor fd_in. 
    }

    char *cmd = strtok(c, " \n"); // strtok will return the first token in c, which is the command.
    char *args[50];               // Assuming the command will have a maximum of 50 arguments.
    int i = 0;                    // i will be used to iterate through the arguments array.
    args[i] = cmd;                // assign the first argument to the command.
    while ((args[++i] = strtok(NULL, " \n")) != NULL)
        ;           // assign the rest of the arguments to the arguments array.
    args[i] = NULL; // the arguments array must be NULL-terminated.

    if (!strcmp("wc", cmd)) // if the command is wc, call wc(). 
    {
        wc();
    }
    else 
    {

        if (strchr(cmd, '/')) 
        { // if the command contains a slash, it is an absolute path to the executable.
            if (access(cmd, X_OK) == 0)
            { // check if the file is executable.
                if (execv(cmd, args) == -1)
                {                       // execute the command.
                    fflush(stderr);     // for error
                    exit(EXIT_FAILURE); // exit if the command fails to execute.
                }
            }
            else
            {
                fprintf(stderr, "cs361sh: command not found: %s\n", cmd); // if the file is not executable, print an error message.
            }
        }
        else
        {
            char *path_var = getenv("PATH");        // get the PATH environment variable.
            char *path_var_copy = strdup(path_var); // make a copy of the PATH environment variable.
            char *dir = strtok(path_var_copy, ":"); // get the first directory in the PATH environment variable.

            while (dir)
            {                                                              // iterate through the directories in the PATH environment variable.
                char full_path[256];                                       // the full path to the executable assuming the path is less than 128 characters.
                snprintf(full_path, sizeof(full_path), "%s/%s", dir, cmd); // concatenate the directory and the command to get the full path to the executable.

                if (access(full_path, X_OK) == 0)
                {                           // check if the file is executable.
                    free(path_var_copy);    // free the copy of the PATH environment variable.
                    if (execv(full_path, args) == -1)
                    {                       // execute the command.
                        fflush(stderr);     // for error
                        exit(EXIT_FAILURE); // exit if the command fails to execute.
                    }
                }

                dir = strtok(NULL, ":"); // get the next directory in the PATH environment variable.
            }

            free(path_var_copy);                                      // free the copy of the PATH environment variable.
            fprintf(stderr, "cs361sh: command not found: %s\n", cmd); // if the file is not executable, print an error message.
        }
        return;
    }
}

void run(char *);

/**
 * @brief Executes pipelines
 *
 * Implements pipelines by setting up pipes between processes.
 *
 * @param head First command in pipeline
 * @param tail Remaining commands in pipeline
 */
void pipeline(char *head, char *tail)
{

    int pipefd[2];
    pipe(pipefd);

    pid_t pid1 = fork();
    if (pid1 == 0)
    {
        close(pipefd[0]); // Close read end
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        run(head);
        exit(0);
    }

    pid_t pid2 = fork();
    if (pid2 == 0)
    {
        close(pipefd[1]); // Close write end
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        run(tail);
        exit(0);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

/**
 * @brief Executes command sequences
 *
 * Implements command sequences by executing commands in order separated by ';'.
 * Calls run() on each command.
 *
 * @param head First command
 * @param tail Remaining commands
 */
void sequence(char *head, char *tail)
{

    // Remove leading spaces from tail
    while (*tail == ' ')
        tail++;

    // Execute head
    run(head);

    // If there is another sequence command in tail, call sequence recursively
    char *sep = strstr(tail, ";");
    if (sep)
    {
        *sep = 0; // truncate the string
        char *new_tail = sep + 1;
        sequence(tail, new_tail); // recursive call with the truncated tail and new tail
    }
    else
    {
        run(tail); // If no more sequence command, just run the tail
    }
}

/**
 * @brief Executes a command line
 *
 * Parses command line and identifies pipelines, sequences and single commands.
 * Calls pipeline(), sequence() or run() as appropriate.
 *
 * @param line The command line to execute
 */
void run(char *line)
{
    char *sep = 0;

    if ((sep = strstr(line, ";")))
    {
        *sep = 0;
        char *tail = sep + 1;
        sequence(line, tail);
    }
    else if ((sep = strstr(line, "|")))
    {
        *sep = 0;
        char *tail = sep + 1;
        pipeline(line, tail);
    }
    else
    {
        pid_t cpid = fork();
        if (cpid == 0)
        {
            // Child Process
            command(line);
            exit(0); // Ensure child process exits after running the command.
        }
        else if (cpid > 0)
        {
            // Parent Process
            waitpid(cpid, NULL, 0); // Parent waits for the child process to finish
        }
        else
        {
            // Handle fork failure
            fflush(stderr);
            return;
        }
    }
}

// reads a line in to buf, one char at a time.
// returns the number of bytes read
int read_line(int fd, char *buf, int max)
{
    if (max <= 0)
        return 0;

    int i = 0;
    while (i < max - 1)
    {
        char c;
        size_t amt = read(fd, &c, 1);
        if (amt != 1)
            break;
        buf[i++] = c;
        if (c == '\n')
            break;
    }
    buf[i] = '\0';

    return i;
}

/**
 * @brief Main function
 *
 * Reads command lines from stdin and executes them by calling run().
 * Exits on EOF.
 */
int main(int argc, char **argv)
{
    printf("cs361sh> ");
    fflush(stdout);

    // handy copies of original file descriptors

    int orig_in = dup(0);
    int orig_out = dup(1);
    int orig_err = dup(2);

    const int BSIZE = 1024;
    char line[BSIZE];

    while (read_line(STDIN_FILENO, line, BSIZE) > 0)
    {
        run(line);
        printf("cs361sh> ");
        fflush(stdout);
    }

    printf("\n");
    fflush(stdout);
    fflush(stderr);
    return 0;
}