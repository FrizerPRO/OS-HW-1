#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>


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
    
    int    fd1,fd2;
    int size;
    char str_buf[mes_size + 1];
    memset(str_buf, 0, mes_size + 1);

    mknod(name1, S_IFIFO | 0666, 0);
    mknod(name2, S_IFIFO | 0666, 0);
    fd2 = 0;

    // второй процесс
    if((fd1 = open(name1, O_RDONLY)) < 0){
        printf("Can\'t open FIFO for reading\n");
        exit(-1);
    }
    
    size = read(fd1, str_buf, mes_size); // считываем из первого канала
    
    if(size < 0){
        printf("Can\'t read string from FIFO\n");
        exit(-1);
    }
    reverse_chars(str_buf); // выполняем преобразование строки
    if(close(fd1) < 0){
        printf("child: Can\'t close FIFO\n");
        exit(-1);
    }
    
    if((fd2 = open(name2, O_WRONLY)) < 0){
        printf("Can\'t open FIFO for writting\n");
        exit(-1);
    }
    
    size = write(fd2, str_buf, mes_size); // записываем во второй канал
    
    if(size != mes_size){
        printf("Can\'t write all string to FIFO\n (size = %d)",size);
        exit(-1);
    }
    if(close(fd2) < 0) {
        printf("parent: Can\'t close FIFO\n");
        exit(-1);
    }
    return 0;
}
