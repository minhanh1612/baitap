#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

// khai bao bien tham so duong dan
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <file_path>\n", argv[0]);
        return 1;
    }

    struct stat fileStat;
    if (lstat(argv[1], &fileStat) < 0) {
        perror("lstat");
        return 1;
    }

    // In đường dẫn
    printf("File Path: %s\n", argv[1]);

    // Loại file
    char *fileType;
    if (S_ISREG(fileStat.st_mode))
        fileType = "Regular File";
    else if (S_ISDIR(fileStat.st_mode))
        fileType = "Directory";
    else if (S_ISLNK(fileStat.st_mode))
        fileType = "Symbolic Link";
    else
        fileType = "Other";
    printf("File Type: %s\n", fileType);

    // Kích thước
    printf("Size: %ld bytes\n", fileStat.st_size);

    // Thời gian sửa đổi
    char timebuf[100];
    struct tm *timeinfo = localtime(&fileStat.st_mtime);
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", timeinfo);
    printf("Last Modified: %s\n", timebuf);

    return 0;
}