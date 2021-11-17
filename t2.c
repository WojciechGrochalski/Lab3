#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

bool check_exit_signal(char *buf)
{
  char *flag;

  flag = strstr(buf, "exit");

  if (flag != NULL)
  {
    return true;
  }
  else
  {
    return false;
  }
}
int main()
{
  int fd1;

  // FIFO file path
  char *myfifo = "/tmp/myfifo1";

  // Creating the named file(FIFO)
  mkfifo(myfifo, 0666);

  char read_buf[80];
  int count = 0;
  char response[] = "Recived message";
  while (1)
  {
    // Open FIFO
    fd1 = open(myfifo, O_RDONLY);
    if (fd1 == -1)
    {
      perror("\nCan't open fifo to write\n");
    }
    memset(read_buf, 0, sizeof(read_buf));
    count = read(fd1, read_buf, 80);
    if (count == -1)
    {
      perror("\nCan't read from fifo\n");
    }

    if (count == 0)
    {
      printf("Recived empty message\n");
    }

    if (count > 0)
    {
      if (check_exit_signal(read_buf))
      {
        break;
      }
      // Print the read string and close
      printf("Recived: %d bytes with message ='%s'\n", count, read_buf);

      fd1 = open(myfifo, O_WRONLY);
      if (fd1 == -1)
      {
        perror("\nCan't open fifo to write\n");
      }

      write(fd1, read_buf, count);
      close(fd1);
    }
  }
  close(fd1);
  printf("Exiting program...\n");
  
  return 0;
}