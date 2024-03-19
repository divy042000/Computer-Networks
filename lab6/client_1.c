#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define SERVER_PORT 5432
#define MAX_LINE 256
#define SERVER_HOST "localhost"

int main() {
    struct hostent *hp;         // Host entry pointer
    struct sockaddr_in sin;     // Socket address structure
    char buf[MAX_LINE];        // Buffer for data
    int len;                    // Length of received data
    int s;                      // Socket descriptor

    // Get host information
    hp = gethostbyname(SERVER_HOST);
    if (!hp) {
        fprintf(stderr, "Error: unknown host\n");
        exit(1);
    }

    // Initialize socket address structure
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
    sin.sin_port = htons(SERVER_PORT);

    // Create a socket
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error: socket");
        exit(1);
    }

    // Connect to the server
    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("Error: connect");
        close(s);
        exit(1);
    }

    /* send GET command to server */
    send(s, "GET\n", strlen("GET\n"), 0);

    /* receive and print the file from the server */
    while ((len = recv(s, buf, sizeof(buf), 0))) {
        fwrite(buf, 1, len, stdout);
    }

    // Close the socket
    close(s);

    return 0;
}

