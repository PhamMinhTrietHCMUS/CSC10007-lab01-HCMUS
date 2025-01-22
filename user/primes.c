#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void seive(int fd) __attribute__((noreturn));
void seive(int fd)
{
  int prime;
  if(read(fd, &prime, 4) == 0)
  {
    close(fd);
    exit(0);
  }
  printf("prime %d\n",prime);
  int p[2];
  pipe(p);

  int pid = fork();
  if(pid == 0)
  {
    close(p[1]);
    close(fd);
    seive(p[0]);
    close(p[0]);
  }
  else
  {
    close(p[0]);
    int num;
    while(read(fd, &num, 4) > 0)
    {
      if(num%prime)
        write(p[1],&num,4);
    }
    close(fd);
    close(p[1]);
    wait(0);
    exit(0);
  }
}

int main(int agrc, char* argv[])
{
  int p[2];
  pipe(p);

  if(fork() == 0)
  {
    close(p[1]);
    seive(p[0]);
    close(p[0]);
    exit(0);
  }
  else
  {
    close(p[0]);
    for(int i = 2; i <= 280; i++)
      write(p[1], &i, 4);
    close(p[1]);
    wait(0);
    exit(0);
  }
}
