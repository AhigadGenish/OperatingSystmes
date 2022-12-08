#include <stdio.h>
#include <errno.h>
#include <sys/fcntl.h>
#include <stdlib.h>
// ahigad genish
// ahigad.genish@gmail.com

// input: program name and two file path's.
// output : check if the content of those file is equal, if yes return 2, else return 1.
int main(int argc, char* argv[]) {

    // initialize variables
    int fd1, fd2, r1, r2;
    char ch1, ch2;
    int ans = 1;

    // check for 2 argumemts;
    if (argc != 3) {
        perror(" please insert 2 paths ");
        exit(-1);
    }
    // open file 1;
    fd1 = open(argv[1], O_RDONLY);
    if (fd1 < 0) 
    {
        perror(" cannot open  the first file  ");   
        exit(-1);
    }
    // open file2;
    fd2 = open(argv[2], O_RDONLY);
    if (fd2 < 0) {
        perror(" cannot open the second file  ");   
        close(fd1);
        exit(-1);

    }
    // compare content of files:
    while (1) {
        r1 = read(fd1, &ch1, 1);
        r2 = read(fd2, &ch2, 1);
        if (r1 == 0 && r2 == 0) {
            ans = 2;
            break;
        }
        else if (r1 == -1 || r2 == -1) {
            ans = -1;
            break;
        }
        else if (r1 == 0 || r2 == 0 || ch1 != ch2) {
            ans = 1;
            break;
        }
    }
    // close files
    close(fd1);
    close(fd2);
    // return by exit
    exit(ans);

}