#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Error: Both file path and text string must be specified.\n");
        exit(EXIT_FAILURE);
    }

    const char *writefile = argv[1];
    const char *writestr = argv[2];

    if (writefile == NULL || writestr == NULL) {
        fprintf(stderr, "Error: File path or text string not specified.\n");
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen(writefile, "w");
    if (file == NULL) {
        perror("Error: Failed to open the file for writing");
        syslog(LOG_ERR, "Failed to open the file for writing: %s", writefile);
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%s\n", writestr);

    fclose(file);

    openlog("writer", LOG_PID | LOG_NDELAY, LOG_USER);

    syslog(LOG_DEBUG, "Writing \"%s\" to \"%s\" successful", writestr, writefile);

    closelog();

    printf("File created successfully at: %s\n", writefile);

    exit(EXIT_SUCCESS);
}
