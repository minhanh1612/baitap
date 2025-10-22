#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int fd[2];
    pid_t pid1, pid2;

    // 1. Tạo pipe
    if (pipe(fd) == -1) {
        perror("pipe failed");
        exit(1);
    }

    // 2. Tạo tiến trình con 1 - chạy "ls -l"
    pid1 = fork();
    if (pid1 < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid1 == 0) {
        // Tiến trình con 1
        close(fd[0]);                // Đóng đầu đọc pipe
        dup2(fd[1], STDOUT_FILENO);  // Chuyển hướng stdout -> đầu ghi pipe
        close(fd[1]);                // Đóng đầu ghi (đã dup2 rồi)

        execlp("ls", "ls", "-l", NULL);
        perror("execlp ls failed");
        exit(1);
    }

    // 3. Tạo tiến trình con 2 - chạy "wc -l"
    pid2 = fork();
    if (pid2 < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid2 == 0) {
        // Tiến trình con 2
        close(fd[1]);                // Đóng đầu ghi pipe
        dup2(fd[0], STDIN_FILENO);   // Chuyển hướng stdin <- đầu đọc pipe
        close(fd[0]);                // Đóng đầu đọc (đã dup2 rồi)

        execlp("wc", "wc", "-l", NULL);
        perror("execlp wc failed");
        exit(1);
    }

    // 4. Trong tiến trình cha
    close(fd[0]);
    close(fd[1]);

    // Đợi cả hai tiến trình con hoàn thành
    wait(NULL);
    wait(NULL);

    return 0;
}
