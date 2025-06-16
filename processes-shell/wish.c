#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

int main() {
    char *path = "/usr/bin"; // likely to be /bin on Mac
    while (1) {
        printf("wish> ");
        char *input = NULL;
        size_t len = 0;
        getline(&input, &len, stdin);

        // Remove the trailing newline character, if present
        if (input[strlen(input) - 1] == '\n') {
            input[strlen(input) - 1] = '\0';
        }

        char *command = strtok(input, " ");
        if (strcmp(command, "exit") == 0) {
            break;
        } else if (strcmp(command, "path") == 0) {
            path = strtok(NULL, " ");
        }else if (strcmp(command, "cd") == 0) {
            char *dir = strtok(NULL, " ");
            if (dir == NULL) {
                printf("Error: No directory provided\n");
            } else {
                if (chdir(dir) != 0) {
                    printf("Error: Directory not found\n");
                }
            }
        } else {
            char delim[] = " \0";
            char *args = strtok(NULL, "\0");
            printf("path: %s\n", path);
            printf("args: %s\n", args);
            printf("command: %s\n", command);
            char *full_path = malloc(strlen(path) + strlen(command) + 2);
            strcpy(full_path, path);
            strcat(full_path, "/");
            strcat(full_path, command);

            // Dynamically build argv array
            int argv_size = 10; // Initial size
            char **argv = malloc(argv_size * sizeof(char *));
            argv[0] = full_path; // First argument is the command itself
            int argc = 1;
            char *output = NULL;

            char *arg = strtok(args, " ");
            while (arg != NULL) {
                if (argc >= argv_size - 1) { // Resize argv if needed
                    argv_size *= 2;
                    argv = realloc(argv, argv_size * sizeof(char *));
                }
                if (strstr(arg, ">") != NULL) {
                    printf("arg: %s\n", arg);
                    arg = arg + 1;
                    output = strtok(NULL, " ");
                    printf("output: %s\n", output);
                    argv[argc] = NULL; // Null-terminate the array
                    break;
                }
                argv[argc++] = arg; // Add argument to argv
                arg = strtok(NULL, " "); // Get next argument
            }
            argv[argc] = NULL; // Null-terminate the array

            // Debug print argv
            for (int i = 0; i < argc; i++) {
                printf("argv[%d]: %s\n", i, argv[i]);
            }
            pid_t pid = fork();
            if (pid == 0) {
                if (output != NULL) {
                    int fd = open(output, O_CREAT | O_WRONLY, 0644);
                    dup2(fd, 1);
                }
                execv(full_path, argv);
                printf("Error: Command not found\n");
                exit(1);
            }
            waitpid(pid, NULL, 0);
            free(full_path);
            free(argv);
        }
    }
    return 0;
}