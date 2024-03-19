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
#include <stdbool.h>
#include <sys/wait.h>

#define SERVER_PORT 5432
#define BUF_SIZE 4096


bool stopStream(char* message)
{
if(strcmp(message,"BYE")==0)
{
printf("Terminating the streaming process!");
exit(EXIT_SUCCESS);
return true;
}
else
{
return false;
}
}

bool stopDownloading(char* message)
{
if(strcmp(message,"BYE")==0)
{
printf("Terminating the Downloading process !");
exit(EXIT_SUCCESS);
return true;
}
else
{
return false;
}
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <host> <download file> <seeking file>\n", argv[0]);
        return 1;
    }

    char *host = argv[1];
    char *downloadfile = argv[2];
    char *seekingfile = argv[3];

    int s = socket(AF_INET, SOCK_DGRAM, 0); 
    if (s < 0) {
        perror("client: socket");
        return 1;
    }

    struct hostent *hp = gethostbyname(host);
    if (!hp) {
        fprintf(stderr, "client: unknown host: %s\n", host);
        close(s);
        return 1;
    }

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    memcpy(&sin.sin_addr, hp->h_addr, hp->h_length);
    sin.sin_port = htons(SERVER_PORT);

    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("client: connect");
        close(s);
        return 1;
    }

 
    if (strlen(seekingfile) > 0) {
        if (send(s, seekingfile, strlen(seekingfile) + 1, 0) < 0) {
            perror("Client: send()");
            close(s);
            return 1;
        }
    }

   
    char sendBuf[BUF_SIZE];
    char recvBuf[BUF_SIZE];
    fgets(sendBuf, sizeof(sendBuf), stdin);
    sendBuf[strcspn(sendBuf, "\n")] = '\0';
    if (send(s, sendBuf, strlen(sendBuf) + 1, 0) < 0) {
        perror("Client: send()");
        close(s);
        return 1;
    }

    printf("Client will get data from %s:%d.\n", host, SERVER_PORT);
    printf("To play the music, pipe the downloaded file to a player, e.g., ALSA, SOX, VLC: cat %s | vlc -\n", downloadfile);

    int createpipe[2];
    if (pipe(createpipe) == -1) {
        perror("Error in creating endpoints of the pipe");
        close(s);
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("Forking the process failed");
        close(s);
        return 1;
    } else if (pid == 0) {
         close(createpipe[1]);
        FILE *fp = fopen(downloadfile, "w");
        if (fp == NULL) {
            perror("fopen");
            close(createpipe[0]);
            close(s);
            return 1;
        }
        ssize_t len;
        while ((len = read(createpipe[0],sendBuf, sizeof(sendBuf))) > 0) {
          if(stopDownloading(sendBuf))
          {
          fclose(fp);
          close(createpipe[0]);
          break;
          }
          else
          {
           fwrite(sendBuf, 1, len, fp);
          }
         }
        
    } else {
       close(createpipe[0]);
        FILE *vlc_pipe = popen("vlc -", "w");
        if (vlc_pipe == NULL) {
            perror("Error in opening the file");
            close(createpipe[1]);
            close(s);
            return 1;
        }
        ssize_t len;
        while ((len = recv(s,recvBuf, sizeof(recvBuf),0)) > 0) {
           if(stopStream(recvBuf))
           {
            pclose(vlc_pipe);
            close(createpipe[1]);
            break;
           }
           else
           {
            fwrite(recvBuf, 1, len, vlc_pipe);
            fflush(vlc_pipe);
            write(createpipe[1],recvBuf,len);
           }
        }   
    }

    close(s);
    return 0;
}
