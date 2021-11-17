#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "err.h"
#include <sys/wait.h>
#include <stdbool.h>

#define LINE_SIZE 1024

static int prompt_user(const char *msg, char *buf, int buf_size)
{
    char line[LINE_SIZE];
    int line_len;

    for (;;)
    {
        printf("%s: ", msg);
        if (fgets(line, LINE_SIZE, stdin) == 0)
        {
            printf("\n");
            exit(0);
        }

        line_len = strlen(line);
        if (line[line_len - 1] != '\n')
        {
            printf(" > Line too long or end of input\n");
            continue;
        }
        line[--line_len] = 0;
        if (line_len >= buf_size)
        {
            printf(" > Response too long\n");
            continue;
        }
        if (line_len == 0)
        {
            printf(" > Value required\n");
            continue;
        }
        strncpy(buf, line, buf_size);
        return line_len;
    }
}
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
    int fd;
    int count;
    char write_buf[80], read_buf[80];
    // FIFO file path
    char *myfifo = "/tmp/myfifo1";

    // Creating the named file(FIFO)
    mkfifo(myfifo, 0666);

    while (true)
    {
        // Open FIFO for writing
        fd = open(myfifo, O_WRONLY);
        // Take an input from user.
        count = prompt_user("Write message", write_buf, sizeof(write_buf));
        if (check_exit_signal(write_buf))
        {
            // Shutdown second proces
            write(fd, write_buf, count);
            break;
        }

        // Write the input on FIFO
        write(fd, write_buf, count);

        close(fd);

        // Open FIFO for Read only
        fd = open(myfifo, O_RDONLY);

        // Read from FIFO
        count = read(fd, write_buf, count);
        if (count == -1)
        {
            perror("\nCan't read from fifo\n");
        }
        // Print the read message
        if (count > 0)
        {
            printf("Second proces recived message: %s\n", write_buf);
        }
        close(fd);
    }
    close(fd);
    if (unlink(myfifo) != 0)
    {
        perror("Can't delete fifo");
    }
    else
    {
        printf("delete fifo\n");
    }
    printf("Exiting program...\n");

    return 0;
}