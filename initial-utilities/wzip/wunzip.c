#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

// function to unzip a file
// it reads the file and writes the uncompressed data to stdout
void unzip (char* buffer, ssize_t bytesRead) {
    char *ptr;
    ptr = buffer; // Pointer to the buffer
    int num;
    while (ptr != buffer + bytesRead) {
        memcpy(&num, ptr + 1, sizeof(int)); // Read the integer from the buffer
        char character = ptr[0];
        for (int i = num; i > 0; i--) {
            // The character to be repeated
            putchar(character); // Write the character to stdout
        }
        ptr += 1 + sizeof(int); // Move the pointer to the next character
    }
}

int main (int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("Error opening file");
        return 1;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
        // Unzip the data
        unzip(buffer, bytesRead);
    }

    if (bytesRead < 0) {
        perror("Error reading file");
    }

    close(fd);
    return 0;
}