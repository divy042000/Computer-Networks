#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<dirent.h>
#include<time.h>
#include<sys/wait.h>
int main()
{
    pid_t pid;
    pid_t parent;
    printf("Parent PID : %d \n",getpid());
    parent=getpid();
    if((pid=fork())==-1)
    {
        printf("Failed to Fork");
        exit(EXIT_FAILURE);
    }

    if(pid==0)

{
        printf("Parent PID : %d \n",parent);
        printf("Child 1 PID : %d \n",getpid());
        time_t t=time(NULL);
        struct tm tm=*localtime(&t);
        char s[64];
        strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", &tm);
        printf("Current time: %s\n", s);
        exit(EXIT_SUCCESS);
    }

    if((pid=fork())==-1)
    {
        printf("Failed to Fork");
        exit(EXIT_FAILURE);
    }

    if(pid==0)
    {
        printf("Parent PID : %d \n",parent);
        printf("Child 2 PID : %d \n",getpid());
        DIR *d;
        struct dirent *dir;
        d = opendir(".");
        if (d) {
            while ((dir = readdir(d)) != NULL) {
                printf("%s\n", dir->d_name);
            }
            closedir(d);
        }
        exit(EXIT_SUCCESS);
    }

    wait(NULL);
    wait(NULL);

    return 0;
}
