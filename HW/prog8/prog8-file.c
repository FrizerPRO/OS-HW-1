#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>



void revstr(char *str1, int begin, int end) {
    int i, len, temp;
    len = end - begin;
    for (i = 0; i < len/2 ; i++) {
        temp = str1[begin + i];
        str1[begin + i] = str1[begin + len - i - 1];
        str1[begin + len - i - 1] = temp;
    }
}

void reverse_chars(char string[]) {
    int size = strlen(string);
    for (int i = 0; i < size; ++i) {
        int j = i;
        while (((string[j] >= 'a' && string[j] <= 'z') ||
                (string[j] >= 'A' && string[j] <= 'Z')) &&
               j < size) {
            j += 1;
        }
        if (j - i != 0) {
            revstr(string, i, j);
            i = j - 1;
        }
    }
}

const int mes_size = 5000;

const char *name1 = "pipe1.fifo";
const char *name2 = "pipe2.fifo";

int main(int argc, char *argv[]) {
    (void)umask(0);

    if(argc != 3){
        printf("./prog.out read.txt write.txt\n");
        return 0;
    }
    
    int    fd1,fd2;
    int size;
    char str_buf[mes_size + 1];
    memset(str_buf, 0, mes_size + 1);

    mknod(name1, S_IFIFO | 0666, 0);
    mknod(name2, S_IFIFO | 0666, 0);
    
    // Первый процесс
    int file_to_read = 0;

    if((file_to_read = open(argv[1], O_RDONLY , 0666)) < 0) {
        printf("Can\'t open file\n");
        exit(-1);
    }

    size = read(file_to_read, str_buf, mes_size); // считываем из файла

    if(close(file_to_read) < 0) {
        printf("Can\'t close file\n");
    }

    if((fd1 = open(name1, O_WRONLY)) < 0){
        printf("Can\'t open FIFO for writting\n");
        exit(-1);
    }

    size = write(fd1, str_buf, mes_size); // записываем в первый канал

    if(size != mes_size){
        printf("Can\'t write all string to FIFO\n");
        exit(-1);
    }
    if(close(fd1) < 0) {
        printf("parent: Can\'t close writing side of FIFO\n");
        exit(-1);
    }


    if((fd2 = open(name2, O_RDONLY)) < 0){
        printf("Can\'t open FIFO for reading\n");
        exit(-1);
    }

    size = read(fd2, str_buf, mes_size);// считываем из 2-го канала
    if(size < 0){
        printf("Can\'t read string from FIFO\n");
        exit(-1);
    }
    if(close(fd2) < 0){
        printf("Can\'t close FIFO\n");
        exit(-1);
    }


    int file_to_write = 0;

    if((file_to_write = open(argv[2], O_WRONLY | O_CREAT, 0666)) < 0) {
        printf("Can\'t open file\n");
        exit(-1);
    }
    printf("%s \n",str_buf);
    size = write(file_to_write, str_buf, strlen(str_buf)); // записываем в файл
    if(size != strlen(str_buf)) {
        printf("Can\'t write all string\n");
        exit(-1);
    }
    if(close(file_to_write) < 0) {
        printf("Can\'t close file\n");
    }
    return 0;
}
