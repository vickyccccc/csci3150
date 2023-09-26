#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/*

Under the same folder, file input.txt.
Each line contains a positive integer number and we guarantee the number is smaller than 1000.
You need to read the number in each line, add it by one, and write them line by line to another file called output.txt

NOTE: Please don't hardcode the results in your program because we change the content in input.txt when grading.

TIP: You can use sscanf to convert char array to int and sprintf to convert int to char array.

*/
int addOne(char *str_of_int)
{
    if (strlen(str_of_int) == 0)
    {
        printf("\tnull\n");
        return 0;
    }

    int *number; // to store result of sscanf()
    number = malloc(sizeof(int));

    // convert char array to int and add it by one
    sscanf(str_of_int, "%d", number);
    printf("\t\t\t%d\n", *number);
    (*number)++;
    printf("\t\t\t%d\n", *number);

    // convert int to char array
    sprintf(str_of_int, "%d", *number);
    printf("\t\t%s\n", str_of_int);

    // free memory
    free(number);

    return strlen(str_of_int);
}

int main()
{

    // read the input.txt
    int fd_input;
    if ((fd_input = open("input.txt", O_RDONLY)) < 0)
    {
        printf("Error in open()\n");
        exit(-1);
    }

    // create the output.txt
    int fd_output;
    if ((fd_output = open("output.txt", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR)) < 0)
    {
        printf("Error in open()\n");
        exit(-1);
    }

    char *buf; // to store read data
    buf = malloc(sizeof(char));

    int not_eof = read(fd_input, buf, sizeof(char));
    printf("%s\n", not_eof && 1 ? "read" : "eof");

    char *str_of_int; // to store numbers on each line
    str_of_int = malloc(4 * sizeof(char));

    while (not_eof)
    {
        if (*buf == '\n')
        {
            printf("endline\n");

            int len = addOne(str_of_int);

            // write them line by line
            if (len > 0)
                write(fd_output, str_of_int, len * sizeof(*str_of_int));
            write(fd_output, buf, sizeof(char));

            // clear string
            memset(str_of_int, '\0', 4 * sizeof(char));
        }
        else
        {
            printf("\t%s\n", buf);
            strcat(str_of_int, buf);
            printf("\t\t%s\n", str_of_int);
        }

        not_eof = read(fd_input, buf, sizeof(char));
        printf("%s\n", not_eof && 1 ? "read" : "eof");
    }

    int len = addOne(str_of_int);
    if (len > 0)
        write(fd_output, str_of_int, 4 * sizeof(*str_of_int));

    // free memory
    free(buf);
    free(str_of_int);

    // close file
    close(fd_input);
    close(fd_output);

    return 0;
}