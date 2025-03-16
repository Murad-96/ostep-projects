#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

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
        } else {
            char *full_path = malloc(strlen(path) + strlen(command) + 2);
            strcpy(full_path, path);
            strcat(full_path, "/");
            strcat(full_path, command);
            char *argv[] = {full_path, NULL};
            pid_t pid = fork();
            if (pid == 0) {
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