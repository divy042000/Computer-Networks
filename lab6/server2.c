#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define SERVER_PORT 5432
#define MAX_PENDING 5
#define MAX_LINE 256

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main() {
    struct sockaddr_in sin;
    char buf[MAX_LINE];
    socklen_t len;
    int s, new_s;
    char str[INET_ADDRSTRLEN];
    
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(SERVER_PORT);

    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        error("simplex-talk: socket");
    }

    inet_ntop(AF_INET, &(sin.sin_addr), str, INET_ADDRSTRLEN);
    printf("Server is using address %s and port %d.\n", str, SERVER_PORT);

    if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
        error("simplex-talk: bind");
    } else {
        printf("Server bind done.\n");
    }

    listen(s, MAX_PENDING);

    while (1) {
        if ((new_s = accept(s, (struct sockaddr *)&sin, &len)) < 0) {
            error("simplex-talk: accept");
        }
        printf("Server Listening.\n");

        // receiving the file name from the client
        memset(buf, 0, MAX_LINE);  // memory allocation in the buffer
        
        int bytes_received = recv(new_s, buf, sizeof(buf), 0);
        if (bytes_received < 0) {
            error("file not received !");
        }
        printf("received file name from the client : %s\n", buf);

        // extracting the filename from the string and skipping up the "GET "
        char *actual_filename = buf + 4;


        // opening the file in the reading format in the binary format 
        FILE *fl = fopen(actual_filename, "rb");
        
        if (!fl) {
             // if the file is not found it will send the -1 to the client representing the file not found
            int fsize = -1;
            send(new_s, &fsize, sizeof(fsize), 0);
        } else {
            fseek(fl, 0L, SEEK_END);  //getting the file pointer to the end location 
            long fsize = ftell(fl);  // getting the file size
            rewind(fl);             // getting the pointer to the starting location
            send(new_s, &fsize, sizeof(fsize), 0); // sending the file size 
            
            int bytes_rd;
            
            // inner reading loop begins for the file reading purposes
            while ((bytes_rd = fread(buf, 1, sizeof(buf), fl)) > 0) {
            send(new_s, buf, bytes_rd, 0);
            }
            if(bytes_rd<0)
            {
            printf("error in reading the file :");
            }
            fclose(fl);
        }

        close(new_s);
    }

    return 0;
}

