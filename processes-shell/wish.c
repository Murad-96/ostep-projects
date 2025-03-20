#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

int main() {
    char *path = "/usr/bin";
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
            char *args = strtok(NULL, delim);
            char *output = strtok(NULL, ">");
            if (output != NULL) {
                printf("output: %s\n", output);
            }
            printf("path: %s\n", path);
            printf("args: %s\n", args);
            printf("command: %s\n", command);
            char *full_path = malloc(strlen(path) + strlen(command) + 2);
            strcpy(full_path, path);
            strcat(full_path, "/");
            strcat(full_path, command);
            char *argv[] = {full_path, args, NULL};
            printf("full_path: %s\n", full_path);
            printf("args: %s\n", args);
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
        }
    }
    return 0;
}