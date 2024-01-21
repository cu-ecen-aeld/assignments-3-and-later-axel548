#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <syslog.h>
#include <signal.h>

#define PORT 9000
#define FILE_PATH "/var/tmp/aesdsocketdata"

volatile sig_atomic_t stop_flag = 0;

void handle_signal(int signum) {
    stop_flag = 1;
    syslog(LOG_INFO, "Caught signal, exiting");
}

int main() {
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    int sockfd, client_sockfd;
    char buffer[1024];

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    openlog("aesdsocket", LOG_PID | LOG_NDELAY, LOG_USER);

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        return -1;
    }

    // Set up server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Socket binding failed");
        return -1;
    }

    // Listen for connections
    if (listen(sockfd, 5) == -1) {
        perror("Socket listening failed");
        return -1;
    }

    while (!stop_flag) {
        // Accept connection
        client_sockfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
        if (client_sockfd == -1) {
            perror("Error accepting connection");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        syslog(LOG_INFO, "Accepted connection from %s", client_ip);

        // Receive data
        ssize_t bytes_received = recv(client_sockfd, buffer, sizeof(buffer), 0);
        if (bytes_received > 0) {
            // Append to file
            FILE* file = fopen(FILE_PATH, "a");
            if (file != NULL) {
                fwrite(buffer, 1, bytes_received, file);
                fclose(file);
            }
        }

        // Return data to client
        FILE* file = fopen(FILE_PATH, "r");
        if (file != NULL) {
            fseek(file, 0, SEEK_END);
            long file_size = ftell(file);
            rewind(file);

            fread(buffer, 1, file_size, file);
            send(client_sockfd, buffer, file_size, 0);

            fclose(file);
        }

        syslog(LOG_INFO, "Closed connection from %s", client_ip);
        close(client_sockfd);
    }

    // Cleanup
    closelog();
    close(sockfd);
    remove(FILE_PATH);

    return 0;
}
