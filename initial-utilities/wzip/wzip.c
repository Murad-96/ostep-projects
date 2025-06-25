#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

// Function to count the number of consecutive same characters in a buffer
// its the caller's responsibility to move the pointer to the next character
int count_same_char (char *buffer) {
    int count = 1;
    while (buffer[count] == buffer[count - 1]) { // while buffer[count] != '\0') {
        count++;
    }
    return count;
}

int main (int argc, char *argv[]) {
    // if (argc < 2) {
    //     fprintf(stderr, "Usage: %s <file> > <compressed>\n", argv[0]);
    //     return 1;
    // }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("Error opening file");
        return 1;
    }

    char buffer[BUFFER_SIZE];
    char *ptr;
    ssize_t bytesRead;
    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
        // Process the data read from the file
        int count = 1;
        ptr = buffer;
        while (*ptr) {
            count = count_same_char(ptr);
            //printf("This character: %c\n", *ptr);
            //printf("%d\n", count); // Print count to stdout
            fwrite(ptr, 1, 1, stdout); // Print count to stdout
            fwrite(&count, sizeof(int), 1, stdout); // Write count to stdout
            ptr += count; // Move pointer to the next character
        }
    }

    if (bytesRead < 0) {
        perror("Error reading file");
    }
    
    return 0;
}