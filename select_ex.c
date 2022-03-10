#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>

int main(void)
{
    char in_str[128];

    fd_set inputs, testfds;
    struct timeval timeout;

    int i,out_fds,n;

    FD_ZERO(&inputs); // Clear inputs
    FD_SET(0,&inputs); // Set standard input channel on

    printf("Size of fd_set: %d\n",sizeof(fd_set));
    printf("Value of FD_SETSIZE: %d\n",FD_SETSIZE);

    while(1)
    {
        testfds=inputs;
        timeout.tv_sec=10;
        timeout.tv_usec=0;

        printf("testfds byte: %d\n",((char *)&testfds)[0]);

        out_fds=select(FD_SETSIZE,&testfds,(fd_set *)NULL,(fd_set *)NULL,NULL);

        printf("Time = %d and %d\n",timeout.tv_sec,timeout.tv_usec);

        printf("testfds byte: %d\n",((char *)&testfds)[0]);

        switch(out_fds)
        {
        case 0:
            printf("Timeout event\n");
            break;
        case -1:
            perror("select");
            exit(1);
        default:
            if(FD_ISSET(0,&testfds))
            {
                if((n=read(0,in_str,127))!=0)
                {
                    if(n==-1)
                        exit(1);
                    in_str[n]=0;
                    printf("From keyboard: %s\n",in_str);
                }
            }

        }
        printf("testefds: %d\n", ((char *)&testfds));

    }

   


}
