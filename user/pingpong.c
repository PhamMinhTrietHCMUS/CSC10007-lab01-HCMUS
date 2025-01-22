#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[])
{
    char received;
    int p[2];
    pipe(p);
    int pid = fork();

    if(pid == 0) // children
    {
        read(p[0], &received, 1);
        close(p[0]);
        printf("%d: ping received\n", getpid());
        write(p[1], "a", 1);
        close(p[1]);
    }
    else // parent
    {
        write(p[1], "b", 1);
        close(p[1]);
        wait(0);
        read(p[0], &received, 1);
        printf("%d: pong received\n", getpid());
        close(p[0]);
    }
    exit(0);
}
