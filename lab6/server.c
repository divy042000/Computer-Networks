#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

#define SERVER_PORT   5432
#define MAX_LINE   256
#define BACKLOG  10

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main() {
    int server_sock, client_sock, portno;
    socklen_t clilen;
    char buffer[MAX_LINE];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    // Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        error("ERROR opening socket");
    }

    // Initialize socket structure
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = SERVER_PORT;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Bind the host address
    if (bind(server_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR on binding");
    }

    // Start listening for the clients
    listen(server_sock, BACKLOG);
    clilen = sizeof(cli_addr);

    // Accept connections from clients
    while (1) {
        client_sock = accept(server_sock, (struct sockaddr *) &cli_addr, &clilen);
        if (client_sock < 0) {
            error("ERROR on accept");
        }

        // Handle client communication
        bzero(buffer, MAX_LINE);
        n = recv(client_sock, buffer, MAX_LINE, 0);
        if (n < 0) {
            error("ERROR reading from socket");
        }

        // If the command is "GET", process it
        if (strncmp(buffer, "GET", 3) == 0) {
            printf("GET command received\n");

            // Receive the filename from the client
            bzero(buffer, MAX_LINE);
            n = recv(client_sock, buffer, MAX_LINE, 0);
            if (n < 0) {
                error("ERROR reading filename from socket");
            }

            printf("Filename received: %s\n", buffer);

            // Open the requested file
            FILE *file = fopen(buffer, "r");
            if (file != NULL) {
                // Send file size to client
                fseek(file, 0, SEEK_END);
                long file_size = ftell(file);
                rewind(file);
                printf("File size: %ld bytes\n", file_size);
                send(client_sock, &file_size, sizeof(file_size), 0);

                // Send file content to client
                char send_buf[MAX_LINE];
                while (fgets(send_buf, MAX_LINE, file) != NULL) {
                    send(client_sock, send_buf, strlen(send_buf), 0);
                }

                fclose(file);
            } else {
                // If file not found, send "File not found" message to client
                long file_not_found = -1;
                send(client_sock, &file_not_found, sizeof(file_not_found), 0);
                printf("File not found\n");
            }
        }

        // Close the client socket
        close(client_sock);
    }

    // Close the server socket (this part is unreachable in the previous code)
    close(server_sock);
    return 0;
}

