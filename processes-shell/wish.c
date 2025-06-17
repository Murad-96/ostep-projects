#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_ARGS 10
#define MAX_COMMANDS 10

struct command {
    char *string; // The command string to be parsed into arguments
    char *argv[MAX_ARGS];
    int argc;
    char *output_file; // For redirection
};

void parse_commands(char *input, struct command* commands, size_t *command_count) {
    int count = 0; // Initialize command count
    char *token = strtok(input, "&");
    while (token != NULL && count < MAX_COMMANDS - 1) {
        commands[count++].string = token;
        token = strtok(NULL, "&");
    }
    *command_count = count; // Set the count of commands
}

void parse_args(struct command *cmd) {
    char *input = cmd->string; // Use the command string directly
    char **argv = cmd->argv; // Pointer to the argv array in the command struct
    // Remove the trailing newline character, if present
    if (input[strlen(input) - 1] == '\n') {
        input[strlen(input) - 1] = '\0';
    }

    // tokenize the input string
    char *token = strtok(input, " ");

    argv[0] = token; // First argument is the command itself
    int argc = 1;

    while (argc < MAX_ARGS - 1 && (token = strtok(NULL, " ")) != NULL) {
        if (strstr(token, ">") != NULL) {
            // Handle output redirection
            cmd->output_file = strtok(NULL, " "); // Get the output file name
            if (cmd->output_file == NULL) {
                fprintf(stderr, "Error: No output file specified for redirection\n");
                return; // Exit if no output file is specified
            }
            break; // Stop parsing further arguments after redirection
        }
        argv[argc++] = token; // Add argument to argv
    }
    
    argv[argc] = NULL; // Null-terminate the array
    cmd->argc = argc; // Set the argument count in the command struct
}

int main() {
    char error_message[30] = "An error has occurred\n";
    char *path = "/usr/bin"; // likely to be /bin on Mac
    while (1) {
        printf("wish> ");
        char *input = NULL;
        size_t len = 0;
        getline(&input, &len, stdin);

        struct command* commands = malloc(MAX_COMMANDS * sizeof(struct command));
        size_t command_count;
        parse_commands(input, commands, &command_count);

        for (size_t i = 0; i < command_count; i++) {
            struct command *cmd = &commands[i];
            if (cmd->string == NULL || strlen(cmd->string) == 0) {
                continue; // Skip empty commands
            }
            parse_args(cmd); // Parse the command into arguments
        }

        if (command_count == 0) {
            free(commands);
            free(input);
            continue; // No valid commands to execute
        }

        if (strcmp(commands[0].argv[0], "exit") == 0) {
            free(commands);
            free(input);
            exit(0); // Exit the shell
        } else if (strcmp(commands[0].argv[0], "path") == 0) {
            if (commands[0].argv[1] != NULL) {
                path = commands[0].argv[1]; // Update the path, useful for execv
                printf("Path updated to: %s\n", path);
            } else {
                printf("Current path: %s\n", path);
            }
        } else if (strcmp(commands[0].argv[0], "cd") == 0) {
            if (commands[0].argv[1] != NULL) {
                if (chdir(commands[0].argv[1]) != 0) {
                    perror("cd failed");
                    write(STDERR_FILENO, error_message, strlen(error_message));
                }
            } else {
                printf("Error: No directory provided\n");
                write(STDERR_FILENO, error_message, strlen(error_message));
            }
        } else {

            for (int i = 0; i < command_count; i++) {
                struct command *cmd = &commands[i];
                if (cmd->argv[0] == NULL) {
                    continue; // Skip empty commands
                }

                pid_t pid = fork();
                if (pid == 0) { // Child process
                    if (cmd->output_file != NULL) {
                        int fd = open(cmd->output_file, O_CREAT | O_WRONLY | O_TRUNC, 0644);
                        if (fd < 0) {
                            perror("open failed");
                            write(STDERR_FILENO, error_message, strlen(error_message));
                            exit(1); // Exit child process on failure
                        }
                        dup2(fd, STDOUT_FILENO); // Redirect stdout to the file
                        close(fd); // Close the file descriptor
                    }
                    execvp(cmd->argv[0], cmd->argv);
                    perror("execv failed"); // If execv fails
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    exit(1); // Exit child process on failure
                } else if (pid < 0) { // Fork failed
                    perror("fork failed");
                }
                
                //free(full_path); // Free the full path after use
            }

            while (1) {
                pid_t pid = wait(NULL); // Wait for any child process to finish
                if (pid < 0) {
                    if (errno == ECHILD) {
                        break; // No more child processes
                    } else {
                        perror("wait failed");
                        write(STDERR_FILENO, error_message, strlen(error_message));
                    }
                }
            }
        }

        free(commands); // Free the commands array
    }
    return 0;
}