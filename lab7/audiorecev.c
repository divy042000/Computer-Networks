#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#define SERVER_PORT 5432
#define BUF_SIZE 4096


int main(int argc, char *argv[]) {
    struct sockaddr_in sin;
    struct sockaddr_storage client_addr;
    char clientIP[INET_ADDRSTRLEN];
    socklen_t client_addr_len;
    char buffer[BUF_SIZE];
    int len;
    int s;
    char *host;
    struct hostent *hp;
    char fileBuf[BUF_SIZE];

    /* Create a socket */
    if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("server: socket error\n");
        exit(1);
    }

    /* Build address data structure and bind to all local addresses */
    memset((char *)&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    if (argc == 2) {
        host = argv[1];
        hp = gethostbyname(host);
        if (!hp) {
            printf("server: unknown host %s\n", host);
            exit(1);
        }
        memcpy((char *)&sin.sin_addr, hp->h_addr, hp->h_length);
    } else {
        sin.sin_addr.s_addr = INADDR_ANY;
    }
    sin.sin_port = htons(SERVER_PORT);

    if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
        printf("server: bind error\n");
        exit(1);
    } else {
        inet_ntop(AF_INET, &(sin.sin_addr), clientIP, INET_ADDRSTRLEN);
        printf("Server is listening at address %s:%d\n", clientIP, SERVER_PORT);
    }

    printf("Client needs to send \"GET\" to receive the file %s\n", argv[1]);

    client_addr_len = sizeof(client_addr);

    
    while (len = recvfrom(s, buffer, sizeof(buffer), 0,
                          (struct sockaddr *)&client_addr, &client_addr_len)) {

        inet_ntop(client_addr.ss_family,
                 &(((struct sockaddr_in *)&client_addr)->sin_addr),
                 clientIP, INET_ADDRSTRLEN);

        printf("Server got message from %s: %s [%d bytes]\n", clientIP, buffer, len);
        int fileFD;
        if (strncmp(buffer, "GET", 3) == 0) {
            printf("GET Received from the server");
            ssize_t bytesReading;
            while ((bytesReading = read(fileFD, fileBuf,sizeof(fileBuf))) > 0) {

                if (sendto(s, fileBuf, bytesReading, 0,
                           (struct sockaddr *)&client_addr,
                           client_addr_len) == -1) {
                    printf("Sending File Error !");
                    exit(1);
                }
                usleep(100);
            }
            close(fileFD);
            memset(buffer, 0, sizeof(buffer));
        } else {
            fileFD = open(buffer, O_RDONLY);
            if (fileFD < 0) {
                printf("Server Open Error !");
                exit(1);
            }
        }
    }

    /* Send BYE to signal termination */
    strcpy(buf, "BYE");
    sendto(s, buf, sizeof(buf), 0,
           (struct sockaddr *)&client_addr, client_addr_len);
}

