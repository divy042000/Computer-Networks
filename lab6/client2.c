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


int main(int argc, char *argv[]) {
    struct hostent *hp;
    struct sockaddr_in sin;
    char *host;
    char buffer[MAX_LINE];
    int s;
    int len;

    if (argc < 4 || strcmp(argv[1], "-f") != 0) {
        fprintf(stderr, "usage: %s -f filename hostname\n", argv[0]);
        exit(1);
    }

    host = argv[3];

    /* translate host name into peer's IP address */
    hp = gethostbyname(host);
    if (!hp) {
        fprintf(stderr, "%s: unknown host: %s\n", argv[0], host);
        exit(1);
    } else {
        printf("Client's remote host: %s\n", host);
    }

    /* build address data structure */
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
    sin.sin_port = htons(SERVER_PORT);

    /* active open */
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        printf("simplex-talk: socket");
    } else {
        printf("Client created socket.\n");
    }

    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        printf("simplex-talk: connect");
        close(s);
        exit(1);
    } else {
        printf("Client connected.\n");
    }

    // Combine "GET" statement and file name into a single string with whitespace
    
    char req[MAX_LINE];
    
    strcpy(req, "GET ");  // Copy "GET " to the request buffer
    strcat(req, argv[2]); // Concatenate the file name

    // Send the request to the server
    
    len = strlen(req); // finding the length 
    
    if (send(s, req, len, 0) == -1) {
        printf("error in sending the filename !");
        close(s);
        exit(1);
    }
    printf("sending the file name to the server : %s\n", req);

    // Receive response from the server

    int file_size;
    int byte_read = recv(s, &file_size, sizeof(file_size), 0);
    if (byte_read < 0) {
        printf("error in receiving the file");
        close(s);
        exit(1);
    } else if (byte_read == 0) {
        fprintf(stderr, "Connection closed by server\n");
        close(s);
        exit(1);
    }	
    if (file_size == -1) {
        printf("File not found on server.\n");
        close(s);
        exit(1);
    }

    printf("File Size: %d\n", file_size);





    
     FILE *fp = fopen("received_sample.txt", "wb");
    if (fp == NULL) {
        printf("error in writting the file !");
        close(s);
        exit(1);
    }
    
    int data=0;
    
    while ((data = recv(s, buffer, MAX_LINE - 1, 0)) > 0) {
         if (data < 0) {
            printf("file not found !");
            fclose(fp);
            close(s);
            exit(1);
        } else if (data == 0) {
            printf("Connection closed by server\n");
            fclose(fp);
            close(s);
            exit(1);
        }
        else
        {
        buffer[data] = '\0';
        printf("%s", buffer);
        fwrite(buffer, 1, data, fp);
        }    
    }
    if(data<0)
    {
    printf("file not found !");
    }
    // Close the socket
    close(s);

    return 0;
}

