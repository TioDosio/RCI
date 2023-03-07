#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
void join()
{
    printf("Joining...\n");
}

int main()
{
    int fd1, fd2;
    fd_set rfds;
    struct timeval tv;
    int retval;
    char buf[256];

    // open file descriptors
    fd1 = fileno(stdin);
    if (fd1 == -1)
    {
        perror("fileno");
        exit(EXIT_FAILURE);
    }

    /*fd2 = open("file2.txt", O_RDONLY);
    if (fd2 == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }*/

    // wait up to 5 seconds for input on either file descriptor
    while (1)
    {
        FD_ZERO(&rfds);
        FD_SET(fd1, &rfds);
        // FD_SET(fd2, &rfds);

        tv.tv_sec = 30;
        tv.tv_usec = 0;

        retval = select(FD_SETSIZE, &rfds, NULL, NULL, &tv);
        if (retval == -1)
        {
            perror("select");
            exit(EXIT_FAILURE);
        }
        else if (retval > 0)
        {
            if (FD_ISSET(fd1, &rfds))
            {
                fgets(buf, sizeof(buf), stdin);
                if (strncmp(buf, "join", 4) == 0)
                {
                    join();
                }
            }
            /*if (FD_ISSET(fd2, &rfds))
            {
                printf("File descriptor 2 is readable.\n");
            }*/
        }
        else
        {
            printf("No data within 5 seconds.\n");
        }
    }

    // close file descriptors
    close(fd1);
    // close(fd2);

    return 0;
}
